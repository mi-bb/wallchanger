/**
 * @file  procfn.h
 * @copyright Copyright (C) 2020 Michał Bąbik
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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef PROCFN_H
#define PROCFN_H

#include <sys/types.h>
#include "proclist.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   PROC_LINUX_CMDLINE
 * @brief Read from cmdline file instead from comm
 *
 * @def   PROC_OPT_NONE
 * @brief No special options for finding processes
 *
 * @def   PROC_OPT_CHK_FIRST
 * @brief Stops searching for process when first one is found
 */
#define PROC_LINUX_CMDLINE 0

#define PROC_OPT_NONE           0x00
#define PROC_OPT_CHK_FIRST      0x01
/*----------------------------------------------------------------------------*/
/**
 * @fn  ProcList * process_get_opt (uid_t       uid_id,
 *                                  const char *s_name,
 *                                  int         i_opt,
 *                                  int         i_exc_pid)
 *
 * @brief  Get processes to ProcList list, more options function version.
 *
 * On Linux:
 * Function scans proc directory, looks for pids directories owned by user
 * with uid_id user id, scans comm or cmdline files with process commands.
 * On FreeBSD:
 * Function uses libprocstat functions to retrieve process list.
 * Found process pids and names are stored in ProcItem items in ProcList list.
 * i_exc_pid contains process pid number to not include in list.
 * List with processes is returned, it should be freed later with
 * proclist_free function.
 *
 * @param[in] uid_id     Process owner user id
 * @param[in] s_name     Name of process or null to get all processes
 * @param[in] i_opt      Options for getting processes
 * @param[in] i_exc_pid  Exclude process pid or 0 to not exclude
 * @return    ProcList list with found processes
 *
 * @fn  ProcList * process_get (const char *s_name)
 *
 * @brief  Get current user processes with no options to ProcList list.
 *
 * @param[in] s_name  Name of process or null to get all processes
 * @return    ProcList list with found processes
 */
/*----------------------------------------------------------------------------*/
ProcList * process_get_opt    (uid_t       uid_id,
                               const char *s_name,
                               int         i_opt,
                               int         i_exc_pid)
           __attribute__ ((returns_nonnull));


ProcList * process_get        (const char *s_name)
           __attribute__ ((returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @fn  ProcItem * process_exists_opt (uid_t       uid_id,
 *                                     const char *s_name,
 *                                     int         i_opt,
 *                                     int         i_exc_pid)
 *
 * @brief  Checks for process existence, returns first found process as
 *         ProcItem item. Function with more options.
 *
 * Find and return first found process for user with uid_id user id, s_name
 * process name s_name, additional options passed in i_opt, i_exc_pid contains
 * process id not to include in searching.
 * Returned ProcItem should be later freed using procitem_free function.
 *
 * @param[in] uid_id     Process owner user id
 * @param[in] s_name     Name of process or null to get any process
 * @param[in] i_opt      Options for getting process
 * @param[in] i_exc_pid  Exclude process pid
 * @return    ProcItem with found process info
 *
 * @fn  ProcItem * process_exists (const char *s_name)
 *
 * @brief  Checks for process existence, returns first found process as
 *         ProcItem item.
 *
 * @param[in] s_name  Name of process or null to get any process
 * @return    ProcItem with found process info
 */
/*----------------------------------------------------------------------------*/
ProcItem * process_exists_opt (uid_t       uid_id,
                               const char *s_name,
                               int         i_opt,
                               int         i_exc_pid);

ProcItem * process_exists     (const char *s_name);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int process_count_opt (uid_t       uid_id,
 *                             const char *s_name,
 *                             int         i_opt,
 *                             int         i_exc_pid)
 *
 * @brief  Count processes with given name. Function with more options.
 *
 * Count processes for user with uid_id ID, with name given in s_name or
 * all if s_name is set to null, additional options are passed using i_opt,
 * i_exc_pid contains process id not to include in counting.
 * Function returns number of found processes.
 *
 * @param[in] uid_id     Process owner user id
 * @param[in] s_name     Name of process or null to count all processes
 * @param[in] i_opt      Options for getting processes
 * @param[in] i_exc_pid  Exclude process pid
 * @return    Number of found processes
 *
 * @fn  int process_count (const char *s_name)
 *
 * @brief  Count current user processes with name given in s_name.
 *
 * @param[in] s_name  Name of process or null to count all processes
 * @return    Number of found processes
 *
 * @fn  int process_count_except_current (const char *s_name)
 *
 * @brief  Count current user processes with name given in s_name, function
 *         doesn't count current process if it is on list.
 *
 * @param[in] s_name  Name of process or null to count all processes
 * @return    Number of found processes
 */
/*----------------------------------------------------------------------------*/
int        process_count_opt            (uid_t       uid_id,
                                         const char *s_name,
                                         int         i_opt,
                                         int         i_exc_pid);

int        process_count                (const char *s_name);

int        process_count_except_current (const char *s_name);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void process_kill_opt (uid_t       uid_id,
 *                             const char *s_name,
 *                             int         i_opt,
 *                             int         i_exc_pid)
 *
 * @brief  Kill process with name passed in s_name. Function with more options.
 *
 * Kill process owned by user with uid_id user id, name as in s_name string
 * and options passed in i_opt, i_exc_pid contains id of process not to kill.
 *
 * @param[in] uid_id     Process owner user id
 * @param[in] s_name     Name of process to kill
 * @param[in] i_opt      Options for getting processes
 * @param[in] i_exc_pid  Exclude process pid
 * @return    none
 *
 * @fn  void process_kill (const char *s_name)
 *
 * @brief  Kill current user process with name given in s_name string.
 *
 * @param[in] s_name  Name of process to kill
 * @return    none
 *
 * @fn  void process_kill_except_current (const char *s_name)
 *
 * @brief  Kills process like process_kill without killing current process.
 *
 * @param[in] s_name  Name of process to kill
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void       process_kill_opt            (uid_t       uid_id,
                                        const char *s_name,
                                        int         i_opt,
                                        int         i_exc_pid);

void       process_kill                (const char *s_name);

void       process_kill_except_current (const char *s_name);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int process_kill_all_opt (uid_t       uid_id,
 *                                const char *s_name,
 *                                int         i_opt,
 *                                int         i_exc_pid)
 *
 * @brief  Kill all process with name passed in s_name. Function with more
 *         options.
 *
 * Kill all process owned by user with uid_id user id, name as in s_name string
 * and options passed in i_opt, i_exc_pid contains id of process not to kill.
 *
 * @param[in] uid_id     Process owner user id
 * @param[in] s_name     Name of process to kill
 * @param[in] i_opt      Options for getting processes
 * @param[in] i_exc_pid  Exclude process pid
 * @return    Number of killed processes.
 *
 * @fn  void process_kill_all (const char *s_name)
 *
 * @brief  Kill all current user processes with name given in s_name string.
 *
 * @param[in] s_name  Name of process to kill
 * @return    Number of killed processes.
 *
 * @fn  void process_kill_all_except_current (const char *s_name)
 *
 * @brief  Kills all processes like process_kill without killing current
 *         process.
 *
 * @param[in] s_name  Name of process to kill
 * @return    Number of killed processes.
 */
/*----------------------------------------------------------------------------*/
int        process_kill_all_opt            (uid_t       uid_id,
                                            const char *s_name,
                                            int         i_opt,
                                            int         i_exc_pid);

int        process_kill_all                (const char *s_name);

int        process_kill_all_except_current (const char *s_name);
/*----------------------------------------------------------------------------*/
#endif

