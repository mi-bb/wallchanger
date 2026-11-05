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
#endif
#if defined(__FreeBSD__)
#include <sys/param.h>
#include <sys/user.h>
#include <sys/sysctl.h>
#include <libprocstat.h>
#include <pwd.h>
#endif
#include <err.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <ctype.h>
#include "process.h"
/*----------------------------------------------------------------------------*/
#if defined(__linux__)
/**
 * @var   BUFF_SIZE
 * @brief File read buffer size
 */
constexpr int BUFF_SIZE = 512;
/*----------------------------------------------------------------------------*/
/**
 * @fn  static char * read_proc_file_data (const char *file_name,
 *                                         int        *error)
 *
 * @brief  Read data from file and return it in string.
 *
 * @param[in]  file_name  File name
 * @param[out] error      Error output
 * @return     New string with file content, it should be later freed using
 *             free function.
 *
 * @fn  static int string_of_numbers (const char *str)
 *
 * @brief  Check if given string is made of numbers.
 *
 * @param[in] str  Input string
 * @return    1 if string is made of numbers, 0 if it isn't
 *
 * @fn  static uid_t file_check_uid (const char *path,
 *                                   int        *error)
 *
 * @brief  Return file user id.
 *
 * @param[in]  path   File name
 * @param[out] error  Error output
 * @return     File UID
 *
 * @fn  static void string_remove_newline (char *str)
 *
 * @brief  Find and remove (replace with null char) new line character 0A.
 *
 * @param[in,out] str  Input string
 * @return         none
 */
/*----------------------------------------------------------------------------*/
static char * read_proc_file_data   (const char *file_name,
                                     int        *error);

static int    string_of_numbers     (const char *str) __attribute__ ((pure));

static uid_t  file_check_uid        (const char *path,
                                     int        *error);

static void   string_remove_newline (char       *str);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read data from file and return it in string.
 */
static char *
read_proc_file_data (const char  *file_name,
                     int         *error)
{
    char   *data = nullptr;          /* Result data from file */
    FILE   *file;                   /* Data file */
    size_t  count = 0;              /* Read data count */
    char    buffer [BUFF_SIZE + 1]; /* Buffer for data */

    file = fopen (file_name, "rb");
    /* Error opening file */
    if (file == nullptr) {
        *error = 1;
        /* warn ("%s", file_name); */
        return nullptr;
    }
    count = fread (buffer, 1, BUFF_SIZE, file);
    if ((data = malloc ((count + 1) * sizeof (char))) == nullptr) {
        err (EXIT_FAILURE, nullptr);
    }
    memcpy (data, buffer, count);
    data[count] = '\0';
    fclose (file);
    *error = 0;

    return data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if given string is made of numbers.
 */
static int
string_of_numbers (const char * __restrict str)
{
    int result = 0;
    while (*str && (result = isdigit (*str++)));
    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Return file user id.
 */
static uid_t
file_check_uid (const char *path,
                int        *error)
{
    struct stat st;

    if (stat (path, &st) == -1) {
        /* warn ("Stat error"); */
        *error = 1;
    }
    else {
        *error = 0;
        return st.st_uid;
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
static void
string_remove_newline (char *str)
{
    char *newline = strchr (str, 0x0A);
    if (newline != nullptr) {
        *newline = '\0';
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get processes to ProcList list, more options function version.
 */
ProcList *
process_get_opt (uid_t       uid,
                 const char *name,
                 int         opts,
                 int         exclude_pid)
{
    ProcList   *list   = nullptr;     /* Proclist item to return */
    const char *path   = "/proc/";    /* Path to proc directory */
    #if PROC_LINUX_CMDLINE == 1
    const char *suffix = "/cmdline";  /* cmdline path */
    #else
    const char *suffix = "/comm";     /* comm path */
    #endif
    DIR        *dir;                  /* Dirent directory */
    size_t      path_len   = 0;       /* Proc path length */
    size_t      suffix_len = 0;       /* cmdline path length */
    size_t      dir_len    = 0;       /* Length of dir inside proc dir */
    int         error      = 0;       /* Error output */
    char       *full_path  = nullptr; /* Alloced string for full cmdline path */
    char       *data       = nullptr; /* Buffer for cmdline content */
    struct dirent *de;                /* Dirent struct */

    list = proc_list_new ();

    path_len   = strlen (path);
    suffix_len = strlen (suffix);

    dir = opendir (path);
    if (dir == nullptr) {
        /* warn ("%s", path); */
        return list;
    }
    while ((de = readdir(dir)) != nullptr) {
        if (de->d_type == DT_DIR && string_of_numbers (de->d_name)) {

            if (exclude_pid && (atoi (de->d_name) == exclude_pid))
                continue;

            dir_len = strlen (de->d_name);

            full_path = malloc ((path_len + suffix_len + dir_len + 1) *
                                sizeof (char));
            if (full_path == nullptr) {
                err (EXIT_FAILURE, nullptr);
            }
            memcpy (full_path, path, path_len);
            memcpy (full_path + path_len, de->d_name, dir_len);
            full_path [path_len + dir_len] = '\0';

            if (file_check_uid (full_path, &error) == uid && !error) {
                memcpy (full_path + path_len + dir_len, suffix, suffix_len);
                full_path [path_len + dir_len + suffix_len] = '\0';

                data = read_proc_file_data (full_path, &error);
                if (data != nullptr && !error) {
                    #if PROC_LINUX_CMDLINE == 0
                    string_remove_newline (data);
                    #endif
                    if (name == nullptr) {
                        proc_list_insert (list,
                                proc_item_new_from_data (de->d_name, data));
                    }
                    else if (strcmp (data, name) == 0) {
                        proc_list_insert (list,
                                proc_item_new_from_data (de->d_name, data));
                        if (opts & PROC_OPT_CHK_FIRST) {
                            free (data);
                            free (full_path);
                            break;
                        }
                    }
                }
                free (data);
            }
            free (full_path);
        }
    }
    closedir (dir);

    return list;
}
#endif
/*----------------------------------------------------------------------------*/
#if defined(__FreeBSD__)
/**
 * @brief  Get processes to ProcList list, more options function version.
 */
ProcList *
process_get_opt (uid_t       uid,
                 const char *name,
                 int         opts,
                 int         exclude_pid)
{
    ProcList          *list = nullptr;  /* Proclist item to return */
    unsigned int       cnt  = 0;        /* Number of found processes */
    unsigned int       i    = 0;        /* i */
    const char        *cmd  = nullptr;  /* Process command string */
    struct kinfo_proc *proc;            /* kinfo_proc structure */
    struct procstat   *procstat;        /* procstat structure */
    char               pid_buff [20];   /* Buffer for process pid string */

    list     = proc_list_new ();
    procstat = procstat_open_sysctl ();
    if (procstat == nullptr) {
        errx (EXIT_FAILURE, "error while procstat_open()");
    }
    proc = procstat_getprocs (procstat, KERN_PROC_UID, (int) uid, &cnt);
    if (proc == nullptr) {
        errx (EXIT_FAILURE, "error while procstat_getprocs()");
    }
    for (i = 0; i < cnt; i++) {
        if (proc[i].ki_stat == SZOMB)
            continue;
        if (exclude_pid && (exclude_pid == proc[i].ki_pid))
            continue;
        cmd = proc[i].ki_comm;
        snprintf (pid_buff, sizeof (pid_buff), "%d", proc[i].ki_pid);
        if (name == nullptr) {
            proc_list_insert (list, proc_item_new_from_data (pid_buff, cmd));
        }
        else if (strcmp (cmd, name) == 0) {
            proc_list_insert (list, proc_item_new_from_data (pid_buff, cmd));
            if (opts & PROC_OPT_CHK_FIRST) {
                break;
            }
        }
    }
    procstat_freeprocs (procstat, proc);
    procstat_close (procstat);
    return list;
}
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get current user processes with no options to ProcList list.
 */
ProcList *
process_get (const char *name)
{
    return process_get_opt (getuid (), name, PROC_OPT_NONE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks for process existence, returns first found process as
 *         ProcItem item. Function with more options.
 */
ProcItem *
process_exists_opt (uid_t       uid,
                    const char *name,
                    int         opts,
                    int         exclude_pid)
{
    ProcItem *item = nullptr;

    ProcList *list = process_get_opt (uid,
                                      name,
                                      opts | PROC_OPT_CHK_FIRST,
                                      exclude_pid);
    if (proc_list_get_count (list) > 0) {
        item = proc_item_copy (proc_list_get_item (list, 0));
    }
    proc_list_free (list);
    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks for process existence, returns first found process as
 *         ProcItem item.
 */
ProcItem *
process_exists (const char *name)
{
    return process_exists_opt (getuid (), name, PROC_OPT_NONE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks for process existence, returns true or false depending on
 *         process.
 */
bool
process_exists_b (const char *name)
{
    bool      found = false;
    ProcItem *item;

    item = process_exists_opt (getuid (), name, PROC_OPT_NONE, 0);
    if (item != nullptr) {
        proc_item_free (item);
        found = true;
    }
    return found;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count processes with given name. Function with more options.
 */
int
process_count_opt (uid_t       uid,
                   const char *name,
                   int         opts,
                   int         exclude_pid)
{
    ProcList *list;
    int       result = 0;

    list = process_get_opt (uid, name, opts, exclude_pid);
    result = (int) proc_list_get_count (list);
    proc_list_free (list);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count current user processes with the given name.
 */
int
process_count (const char *name)
{
    return process_count_opt (getuid (), name, PROC_OPT_NONE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count current user processes with the given name, function
 *         doesn't count current process if it is on list.
 */
int
process_count_except_current (const char *name)
{
    return process_count_opt (getuid (), name, PROC_OPT_NONE, getpid ());
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill process with the given name. Function with more options.
 */
void
process_kill_opt (uid_t       uid,
                  const char *name,
                  int         opts,
                  int         exclude_pid)
{
    ProcItem *item;
    pid_t     pid = 0;

    item = process_exists_opt (uid, name, opts, exclude_pid);
    if (item != nullptr) {
        #ifdef DEBUG
        printf ("kill %s %s\n",
                proc_item_get_name (item),
                proc_item_get_pid (item));
        #endif
        pid = atoi (proc_item_get_pid (item));
        kill (pid, SIGTERM);
        proc_item_free (item);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill current user process with the given name.
 */
void
process_kill (const char *name)
{
    process_kill_opt (getuid (), name, PROC_OPT_NONE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kills process like process_kill without killing current process.
 */
void
process_kill_except_current (const char *name)
{
    process_kill_opt (getuid (), name, PROC_OPT_NONE, getpid ());
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill all process with the given name. Function with more
 *         options.
 */
int
process_kill_all_opt (uid_t       uid,
                      const char *name,
                      int         opts,
                      int         exclude_pid)
{
    ProcList      *list;
    uint_fast32_t  count = 0;
    uint_fast32_t  i     = 0;
    pid_t          pid   = 0;

    list  = process_get_opt (uid, name, opts, exclude_pid);
    count = proc_list_get_count (list);

    for (i = 0; i < count; ++i) {
        pid = atoi (proc_item_get_pid (proc_list_get_item (list, i)));
        #ifdef DEBUG
        printf ("killing %s %s\n",
                proc_item_get_name (proc_list_get_item (list, i)),
                proc_item_get_pid (proc_list_get_item (list, i)));
        #endif
        kill (pid, SIGTERM);
    }
    proc_list_free (list);

    return (int) count;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill all current user processes with the given name.
 */
int
process_kill_all (const char *name)
{
    return process_kill_all_opt (getuid (), name, PROC_OPT_NONE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kills all processes like process_kill without killing current
 *         process.
 */
int
process_kill_all_except_current (const char *name)
{
    return process_kill_all_opt (getuid (), name, PROC_OPT_NONE, getpid ());
}
/*----------------------------------------------------------------------------*/
