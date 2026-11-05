/**
 * @file  process.c
 * @copyright Copyright (C) 2020-2026 Michał Bąbik
 *
 * This file is part of Wall Changer.
 *
 * Wall Changer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Wall Changer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Wall Changer.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @brief  Functions for getting process info.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#if defined(__linux__)
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#endif
#if defined(__FreeBSD__)
#include <sys/param.h>
#include <sys/user.h>
#include <sys/sysctl.h>
#include <libprocstat.h>
#endif
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include "process.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   PROC_NAME_MAX
 * @brief Longest process name the system reports, longer names are cut
 *        to this length (TASK_COMM_LEN - 1 on Linux, MAXCOMLEN on FreeBSD)
 */
#if defined(__FreeBSD__)
constexpr size_t PROC_NAME_MAX = MAXCOMLEN;
#else
constexpr size_t PROC_NAME_MAX = 15;
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if process name read from the system matches the given name.
 *
 * @param[in] proc_name  Process name as reported by the system
 * @param[in] name       Name to compare with
 * @return    true if names match, false if not
 */
[[nodiscard]] static bool
process_name_matches (const char *proc_name,
                      const char *name)
{
    if (strcmp (proc_name, name) == 0)
        return true;

    /* The name was cut by the system, compare the part that is left */
    return strlen (proc_name) == PROC_NAME_MAX &&
           strlen (name)       > PROC_NAME_MAX &&
           strncmp (proc_name, name, PROC_NAME_MAX) == 0;
}
/*----------------------------------------------------------------------------*/
#if defined(__linux__)
/**
 * @var   STAT_BUFF_SIZE
 * @brief Buffer size for the start of /proc/PID/stat, pid, name and state
 *        are all within the first few dozen bytes
 */
constexpr size_t STAT_BUFF_SIZE = 256;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert a /proc entry name to a PID.
 *
 * @param[in]  str  /proc directory entry name
 * @param[out] pid  Converted PID
 * @return     true if str is made of digits only, false if not
 */
[[nodiscard]] static bool
parse_pid (const char *str,
           pid_t      *pid)
{
    const char *it = str;

    if (*it == '\0')
        return false;

    for (; *it != '\0'; ++it) {
        if (*it < '0' || *it > '9')
            return false;
    }
    *pid = (pid_t) strtol (str, nullptr, 10);
    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read process name and state from its /proc/PID/stat file.
 *
 * @param[in]  proc_fd  Open /proc directory descriptor
 * @param[in]  pid_dir  Process directory name in /proc
 * @param[out] buffer   Buffer of STAT_BUFF_SIZE bytes, name points into it
 * @param[out] name     Process name
 * @param[out] state    Process state character
 * @return     true if data was read, false if not
 */
[[nodiscard]] static bool
read_proc_stat (int          proc_fd,
                const char  *pid_dir,
                char        *buffer,
                const char **name,
                char        *state)
{
    char     path [NAME_MAX + sizeof ("/stat")]; /* PID/stat path */
    char    *open_par  = nullptr; /* Bracket opening the name */
    char    *close_par = nullptr; /* Bracket closing the name */
    ssize_t  count     = 0;       /* Read data count */
    int      fd        = -1;      /* stat file descriptor */

    snprintf (path, sizeof (path), "%s/stat", pid_dir);

    if ((fd = openat (proc_fd, path, O_RDONLY | O_CLOEXEC)) == -1)
        return false;

    count = read (fd, buffer, STAT_BUFF_SIZE - 1);
    close (fd);

    if (count <= 0)
        return false;

    buffer[count] = '\0';

    /* Line starts with "PID (name) state", the name itself may contain
     * brackets and spaces, so it ends at the last closing bracket */
    open_par  = strchr  (buffer, '(');
    close_par = strrchr (buffer, ')');

    if (open_par == nullptr || close_par == nullptr ||
        close_par < open_par || close_par[1] != ' ')
        return false;

    *close_par = '\0';
    *name  = open_par + 1;
    *state = close_par[2];

    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get running processes of the current user.
 */
ProcList *
process_get (const char *name,
             pid_t       exclude_pid)
{
    ProcList      *list      = nullptr; /* Proclist item to return */
    DIR           *dir       = nullptr; /* /proc directory */
    struct dirent *entry     = nullptr; /* /proc directory entry */
    const char    *proc_name = nullptr; /* Process name */
    uid_t          uid       = getuid ();
    pid_t          pid       = 0;       /* Process PID */
    char           state     = 0;       /* Process state */
    struct stat    st;                  /* Process directory info */
    char           buffer [STAT_BUFF_SIZE]; /* Buffer for stat file data */

    list = proc_list_new ();

    if ((dir = opendir ("/proc")) == nullptr)
        return list;

    while ((entry = readdir (dir)) != nullptr) {

        if (entry->d_type != DT_DIR || !parse_pid (entry->d_name, &pid))
            continue;

        if (pid == exclude_pid)
            continue;

        /* Process directory is owned by the process user */
        if (fstatat (dirfd (dir), entry->d_name, &st, 0) == -1 ||
            st.st_uid != uid)
            continue;

        if (!read_proc_stat (dirfd (dir), entry->d_name, buffer,
                             &proc_name, &state))
            continue;

        /* Skip zombie and dead processes, they have exited already */
        if (state == 'Z' || state == 'X')
            continue;

        if (name == nullptr || process_name_matches (proc_name, name)) {
            proc_list_insert (list, proc_item_new (pid, proc_name));
        }
    }
    closedir (dir);

    return list;
}
#endif
/*----------------------------------------------------------------------------*/
#if defined(__FreeBSD__)
/**
 * @brief  Get running processes of the current user.
 */
ProcList *
process_get (const char *name,
             pid_t       exclude_pid)
{
    ProcList          *list     = nullptr; /* Proclist item to return */
    struct procstat   *procstat = nullptr; /* procstat structure */
    struct kinfo_proc *procs    = nullptr; /* Found processes */
    unsigned int       count    = 0;       /* Number of found processes */

    list = proc_list_new ();

    if ((procstat = procstat_open_sysctl ()) == nullptr)
        return list;

    procs = procstat_getprocs (procstat, KERN_PROC_UID, (int) getuid (),
                               &count);
    if (procs == nullptr) {
        procstat_close (procstat);
        return list;
    }
    for (unsigned int i = 0; i < count; ++i) {
        if (procs[i].ki_stat == SZOMB || procs[i].ki_pid == exclude_pid)
            continue;

        if (name == nullptr || process_name_matches (procs[i].ki_comm, name)) {
            proc_list_insert (list,
                    proc_item_new (procs[i].ki_pid, procs[i].ki_comm));
        }
    }
    procstat_freeprocs (procstat, procs);
    procstat_close (procstat);

    return list;
}
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if list contains a process with the given name.
 */
bool
process_list_contains (const ProcList *list,
                       const char     *name)
{
    const uint_fast32_t count = proc_list_get_count (list);

    for (uint_fast32_t i = 0; i < count; ++i) {
        if (process_name_matches (
                    proc_item_get_name (proc_list_get_item (list, i)), name))
            return true;
    }
    return false;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count current user processes with the given name, the calling
 *         process is not counted.
 */
int
process_count_except_current (const char *name)
{
    ProcList *list   = process_get (name, getpid ());
    int       result = (int) proc_list_get_count (list);

    proc_list_free (list);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Send SIGTERM to all current user processes with the given name,
 *         except the calling process.
 */
int
process_kill_all_except_current (const char *name)
{
    ProcList       *list   = nullptr; /* Processes to kill */
    const ProcItem *item   = nullptr; /* Process to kill */
    uint_fast32_t   count  = 0;       /* Number of found processes */
    int             killed = 0;       /* Number of signalled processes */

    if (name == nullptr)
        return 0;

    list  = process_get (name, getpid ());
    count = proc_list_get_count (list);

    for (uint_fast32_t i = 0; i < count; ++i) {
        item = proc_list_get_item (list, i);

        if (kill (proc_item_get_pid (item), SIGTERM) == 0) {
            ++killed;
        }
        else if (errno != ESRCH) {
            /* ESRCH means the process has exited in the meantime */
            warn ("Could not stop %s (PID %d)",
                  proc_item_get_name (item),
                  (int) proc_item_get_pid (item));
        }
    }
    proc_list_free (list);

    return killed;
}
/*----------------------------------------------------------------------------*/

