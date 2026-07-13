/**
 * @file  procfn.c
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
 * @author Michal Babik <michal.babik@pm.me>
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
#include "procfn.h"
/*----------------------------------------------------------------------------*/
#if defined(__linux__)
/**
 * @def   BUFF_SIZE
 * @brief File read buffer size
 */
#define BUFF_SIZE 512
/*----------------------------------------------------------------------------*/
/**
 * @fn  static char * read_proc_file_data (const char *s_fname,
 *                                         int        *i_err)
 *
 * @brief  Read data from file and return it in string.
 *
 * @param[in]  s_fname  File name
 * @param[out] i_err    Error output
 * @return     New string with file content, it should be later freed using
 *             free function.
 *
 * @fn  static int string_of_numbers (const char *s_str)
 *
 * @brief  Check if given string is made of numbers.
 *
 * @param[in] s_str  Input string
 * @return    1 if string is made of numbers, 0 if it isn't
 *
 * @fn  static uid_t file_check_uid (const char *s_path,
 *                                   int        *i_err)
 *
 * @brief  Return file user id.
 *
 * @param[in]  s_path  File name
 * @param[out] i_err   Error output
 * @return     File UID
 *
 * @fn  static void string_remove_newline (char *s_str)
 *
 * @brief  Find and remove (replace with null char) new line character 0A.
 *
 * @param[in,out] s_str  Input string
 * @retrn         none
 */
/*----------------------------------------------------------------------------*/
static char * read_proc_file_data   (const char *s_fname,
                                     int        *i_err);

static int    string_of_numbers     (const char *s_str) __attribute__ ((pure));

static uid_t  file_check_uid        (const char *s_path,
                                     int        *i_err);

static void   string_remove_newline (char       *s_str);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read data from file and return it in string.
 */
static char *
read_proc_file_data (const char  *s_fname,
                     int         *i_err)
{
    char   *s_data = NULL;          /* Result data from file */
    FILE   *f_file;                 /* Data file */
    size_t  ui_count = 0;           /* Read data count */
    char    s_buff [BUFF_SIZE + 1]; /* Buffer for data */

    f_file = fopen (s_fname, "rb");
    /* Error opening file */
    if (f_file == NULL) {
        *i_err = 1;
        /* warn ("%s", s_fname); */
        return NULL;
    }
    ui_count = fread (s_buff, 1, BUFF_SIZE, f_file);
    if ((s_data = malloc ((ui_count + 1) * sizeof (char))) == NULL) {
        err (EXIT_FAILURE, NULL);
    }
    memcpy (s_data, s_buff, ui_count);
    s_data[ui_count] = '\0';
    fclose (f_file);
    *i_err = 0;

    return s_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if given string is made of numbers.
 */
static int
string_of_numbers (const char * __restrict s_str)
{
    int i_res = 0;
    while (*s_str && (i_res = isdigit (*s_str++)));
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Return file user id.
 */
static uid_t
file_check_uid (const char *s_path,
                int        *i_err)
{
    struct stat st;

    if (stat (s_path, &st) == -1) {
        /* warn ("Stat error"); */
        *i_err = 1;
    }
    else {
        *i_err = 0;
        return st.st_uid;
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
static void
string_remove_newline (char *s_str)
{
    char *s_p = strchr (s_str, 0x0A);
    if (s_p != NULL) {
        *s_p = '\0';
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get processes to ProcList list, more options function version.
 */
ProcList *
process_get_opt (uid_t       uid_id,
                 const char *s_name,
                 int         i_opt,
                 int         i_exc_pid)
{
    ProcList   *pl_list = NULL;       /* Proclist item to return */
    const char *s_path  = "/proc/";   /* Path to proc directory */
    #if PROC_LINUX_CMDLINE == 1
    const char *s_file  = "/cmdline"; /* cmdline path */
    #else
    const char *s_file  = "/comm";    /* comm path */
    #endif
    DIR        *dr;                   /* Dirent directory */
    size_t      ui_plen = 0;          /* Proc path length */
    size_t      ui_flen = 0;          /* cmdline path length */
    size_t      ui_dlen = 0;          /* Length of dir inside proc dir */
    int         i_err   = 0;          /* Error output */
    char       *s_fn    = NULL;       /* Alloced string for full cmdline path */
    char       *s_data  = NULL;       /* Buffer for cmdline content */
    struct dirent *de;                /* Dirent struct */

    pl_list = proclist_new ();

    ui_plen = strlen (s_path);
    ui_flen = strlen (s_file);

    dr = opendir (s_path); 
    if (dr == NULL) {
        /* warn ("%s", s_path); */
        return pl_list;
    }
    while ((de = readdir(dr)) != NULL) {
        if (de->d_type == DT_DIR && string_of_numbers (de->d_name)) {

            if (i_exc_pid && (atoi (de->d_name) == i_exc_pid))
                continue;

            ui_dlen = strlen (de->d_name);

            s_fn = malloc ((ui_plen + ui_flen + ui_dlen + 1) * sizeof (char));
            if (s_fn == NULL) {
                err (EXIT_FAILURE, NULL);
            }
            memcpy (s_fn, s_path, ui_plen);
            memcpy (s_fn + ui_plen, de->d_name, ui_dlen);
            s_fn [ui_plen + ui_dlen] = '\0';

            if (file_check_uid (s_fn, &i_err) == uid_id && !i_err) {
                memcpy (s_fn + ui_plen + ui_dlen, s_file, ui_flen);
                s_fn [ui_plen + ui_dlen + ui_flen] = '\0';

                s_data = read_proc_file_data (s_fn, &i_err);
                if (s_data != NULL && !i_err) {
                    #if PROC_LINUX_CMDLINE == 0
                    string_remove_newline (s_data);
                    #endif
                    if (s_name == NULL) {
                        proclist_insert (pl_list,
                                procitem_new_from_data (de->d_name, s_data));
                    }
                    else if (strcmp (s_data, s_name) == 0) {
                        proclist_insert (pl_list,
                                procitem_new_from_data (de->d_name, s_data));
                        if (i_opt & PROC_OPT_CHK_FIRST) {
                            free (s_data);
                            free (s_fn);
                            break;
                        }
                    }
                }
                free (s_data);
            }
            free (s_fn);
        }
    }
    closedir (dr);

    return pl_list;
}
#endif
/*----------------------------------------------------------------------------*/
#if defined(__FreeBSD__)
/**
 * @brief  Get processes to ProcList list, more options function version.
 */
ProcList *
process_get_opt (uid_t       uid_id,
                 const char *s_name,
                 int         i_opt,
                 int         i_exc_pid)
{
    ProcList          *pl_list = NULL;  /* Proclist item to return */
    unsigned int       cnt     = 0;     /* Number of found processes */
    unsigned int       i       = 0;     /* i */
    const char        *s_cmd   = NULL;  /* Process command string */
    struct kinfo_proc *proc;            /* kinfo_proc structure */
    struct procstat   *procstat;        /* procstat structure */
    char               s_pidbuff [20];  /* Buffer for process pid string */

    pl_list  = proclist_new ();
    procstat = procstat_open_sysctl ();
    if (procstat == NULL) {
        errx (EXIT_FAILURE, "error while procstat_open()");
    }
    proc = procstat_getprocs (procstat, KERN_PROC_UID, (int) uid_id, &cnt);
    if (proc == NULL) {
        errx (EXIT_FAILURE, "error while procstat_getprocs()");
    }
    for (i = 0; i < cnt; i++) {
        if (proc[i].ki_stat == SZOMB)
            continue;
        if (i_exc_pid && (i_exc_pid == proc[i].ki_pid))
            continue;
        s_cmd = proc[i].ki_comm;
        snprintf (s_pidbuff, sizeof (s_pidbuff), "%d", proc[i].ki_pid);
        if (s_name == NULL) {
            proclist_insert (pl_list, procitem_new_from_data (s_pidbuff, s_cmd));
        }
        else if (strcmp (s_cmd, s_name) == 0) {
            proclist_insert (pl_list, procitem_new_from_data (s_pidbuff, s_cmd));
            if (i_opt & PROC_OPT_CHK_FIRST) {
                break;
            }
        }
    }
    procstat_freeprocs (procstat, proc);
    procstat_close (procstat);
    return pl_list;
}
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get current user processes with no options to ProcList list.
 */
ProcList *
process_get (const char *s_name)
{
    return process_get_opt (getuid (), s_name, PROC_OPT_NONE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks for process existence, returns first found process as
 *         ProcItem item. Function with more options.
 */
ProcItem *
process_exists_opt (uid_t       uid_id,
                    const char *s_name,
                    int         i_opt,
                    int         i_exc_pid)
{
    ProcItem *pi_item = NULL;

    ProcList *pl_list = process_get_opt (uid_id,
                                         s_name,
                                         i_opt | PROC_OPT_CHK_FIRST,
                                         i_exc_pid);
    if (proclist_get_cnt (pl_list) > 0) {
        pi_item = procitem_copy (proclist_get_item (pl_list, 0));
    }
    proclist_free (pl_list);
    return pi_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks for process existence, returns first found process as
 *         ProcItem item.
 */
ProcItem *
process_exists (const char *s_name)
{
    return process_exists_opt (getuid (), s_name, PROC_OPT_NONE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks for process existence, returns 1 or 0 depending on process.
 */
int
process_exists_b (const char *s_name)
{
    int       i_res   = 0;
    ProcItem *pi_item;

    pi_item = process_exists_opt (getuid (), s_name, PROC_OPT_NONE, 0);
    if (pi_item != NULL) {
        procitem_free (pi_item);
        i_res = 1;
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count processes with given name. Function with more options.
 */
int
process_count_opt (uid_t       uid_id,
                   const char *s_name,
                   int         i_opt,
                   int         i_exc_pid)
{
    ProcList *pl_list;
    int       i_res = 0;

    pl_list = process_get_opt (uid_id, s_name, i_opt, i_exc_pid);
    i_res = (int) proclist_get_cnt (pl_list);
    proclist_free (pl_list);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count current user processes with name given in s_name.
 */
int
process_count (const char *s_name)
{
    return process_count_opt (getuid (), s_name, PROC_OPT_NONE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count current user processes with name given in s_name, function
 *         doesn't count current process if it is on list.
 */
int
process_count_except_current (const char *s_name)
{
    return process_count_opt (getuid (), s_name, PROC_OPT_NONE, getpid ());
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill process with name passed in s_name. Function with more options.
 */
void
process_kill_opt (uid_t       uid_id,
                  const char *s_name,
                  int         i_opt,
                  int         i_exc_pid)
{
    ProcItem *pi_item;
    pid_t     pi_pid = 0;

    pi_item = process_exists_opt (uid_id, s_name, i_opt, i_exc_pid);
    if (pi_item != NULL) {
        #ifdef DEBUG
        printf ("kill %s %s\n", 
                procitem_get_name (pi_item),
                procitem_get_pid (pi_item));
        #endif
        pi_pid = atoi (procitem_get_pid (pi_item));
        kill (pi_pid, SIGTERM);
        procitem_free (pi_item);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill current user process with name given in s_name string.
 */
void
process_kill (const char *s_name)
{
    process_kill_opt (getuid (), s_name, PROC_OPT_NONE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kills process like process_kill without killing current process.
 */
void
process_kill_except_current (const char *s_name)
{
    process_kill_opt (getuid (), s_name, PROC_OPT_NONE, getpid ());
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill all process with name passed in s_name. Function with more
 *         options.
 */
int
process_kill_all_opt (uid_t       uid_id,
                      const char *s_name,
                      int         i_opt,
                      int         i_exc_pid)
{
    ProcList      *pl_list;
    uint_fast32_t  i_cnt  = 0;
    uint_fast32_t  i      = 0;
    pid_t          pi_pid = 0;

    pl_list = process_get_opt (uid_id, s_name, i_opt, i_exc_pid);
    i_cnt   = proclist_get_cnt (pl_list);

    for (i = 0; i < i_cnt; ++i) {
        pi_pid = atoi (procitem_get_pid (proclist_get_item (pl_list, i)));
        #ifdef DEBUG
        printf ("killing %s %s\n", 
                procitem_get_name (proclist_get_item (pl_list, i)),
                procitem_get_pid (proclist_get_item (pl_list, i)));
        #endif
        kill (pi_pid, SIGTERM);
    }
    proclist_free (pl_list);

    return (int) i_cnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kill all current user processes with name given in s_name string.
 */
int
process_kill_all (const char *s_name)
{
    return process_kill_all_opt (getuid (), s_name, PROC_OPT_NONE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kills all processes like process_kill without killing current
 *         process.
 */
int
process_kill_all_except_current (const char *s_name)
{
    return process_kill_all_opt (getuid (), s_name, PROC_OPT_NONE, getpid ());
}
/*----------------------------------------------------------------------------*/

