/**
 * @file  process.h
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
#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>
#include "proc_list.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   PROC_LINUX_CMDLINE
 * @brief Read from cmdline file instead from comm
 *
 * @var   PROC_OPT_NONE
 * @brief No special options for finding processes
 *
 * @var   PROC_OPT_CHK_FIRST
 * @brief Stops searching for process when first one is found
 */
/* Used in #if, must stay a macro. */
#define PROC_LINUX_CMDLINE 0

constexpr int PROC_OPT_NONE      = 0x00;
constexpr int PROC_OPT_CHK_FIRST = 0x01;
/*----------------------------------------------------------------------------*/
/**
 * @fn  ProcList * process_get_opt (uid_t       uid,
 *                                  const char *name,
 *                                  int         opts,
 *                                  int         exclude_pid)
 *
 * @brief  Get processes to ProcList list, more options function version.
 *
 * On Linux:
 * Function scans proc directory, looks for pids directories owned by user
 * with uid user id, scans comm or cmdline files with process commands.
 * On FreeBSD:
 * Function uses libprocstat functions to retrieve process list.
 * Found process pids and names are stored in ProcItem items in ProcList list.
 * exclude_pid contains process pid number to not include in list.
 * List with processes is returned, it should be freed later with
 * proc_list_free function.
 *
 * @param[in] uid          Process owner user id
 * @param[in] name         Name of process or null to get all processes
 * @param[in] opts         Options for getting processes
 * @param[in] exclude_pid  Exclude process pid or 0 to not exclude
 * @return    ProcList list with found processes
 *
 * @fn  ProcList * process_get (const char *name)
 *
 * @brief  Get current user processes with no options to ProcList list.
 *
 * @param[in] name  Name of process or null to get all processes
 * @return    ProcList list with found processes
 */
/*----------------------------------------------------------------------------*/
ProcList * process_get_opt    (uid_t       uid,
                               const char *name,
                               int         opts,
                               int         exclude_pid)
           __attribute__ ((returns_nonnull));


ProcList * process_get        (const char *name)
           __attribute__ ((returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @fn  ProcItem * process_exists_opt (uid_t       uid,
 *                                     const char *name,
 *                                     int         opts,
 *                                     int         exclude_pid)
 *
 * @brief  Checks for process existence, returns first found process as
 *         ProcItem item. Function with more options.
 *
 * Find and return first found process for user with uid user id and the given
 * process name, additional options passed in opts, exclude_pid contains
 * process id not to include in searching.
 * Returned ProcItem should be later freed using proc_item_free function.
 *
 * @param[in] uid          Process owner user id
 * @param[in] name         Name of process or null to get any process
 * @param[in] opts         Options for getting process
 * @param[in] exclude_pid  Exclude process pid
 * @return    ProcItem with found process info
 *
 * @fn  ProcItem * process_exists (const char *name)
 *
 * @brief  Checks for process existence, returns first found process as
 *         ProcItem item.
 *
 * @param[in] name  Name of process or null to get any process
 * @return    ProcItem with found process info
 *
 * @fn  bool process_exists_b (const char *name)
 *
 * @brief  Checks for process existence, returns true or false depending on
 *         process.
 *
 * @param[in] name  Name of process or null to get any process
 * @return    true if process exists or false if not.
 */
/*----------------------------------------------------------------------------*/
ProcItem * process_exists_opt (uid_t       uid,
                               const char *name,
                               int         opts,
                               int         exclude_pid);

ProcItem * process_exists     (const char *name);

bool       process_exists_b   (const char *name);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int process_count_opt (uid_t       uid,
 *                             const char *name,
 *                             int         opts,
 *                             int         exclude_pid)
 *
 * @brief  Count processes with given name. Function with more options.
 *
 * Count processes for user with uid ID, with the given name or
 * all if name is set to null, additional options are passed using opts,
 * exclude_pid contains process id not to include in counting.
 * Function returns number of found processes.
 *
 * @param[in] uid          Process owner user id
 * @param[in] name         Name of process or null to count all processes
 * @param[in] opts         Options for getting processes
 * @param[in] exclude_pid  Exclude process pid
 * @return    Number of found processes
 *
 * @fn  int process_count (const char *name)
 *
 * @brief  Count current user processes with the given name.
 *
 * @param[in] name  Name of process or null to count all processes
 * @return    Number of found processes
 *
 * @fn  int process_count_except_current (const char *name)
 *
 * @brief  Count current user processes with the given name, function
 *         doesn't count current process if it is on list.
 *
 * @param[in] name  Name of process or null to count all processes
 * @return    Number of found processes
 */
/*----------------------------------------------------------------------------*/
int        process_count_opt            (uid_t       uid,
                                         const char *name,
                                         int         opts,
                                         int         exclude_pid);

int        process_count                (const char *name);

int        process_count_except_current (const char *name);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void process_kill_opt (uid_t       uid,
 *                             const char *name,
 *                             int         opts,
 *                             int         exclude_pid)
 *
 * @brief  Kill process with the given name. Function with more options.
 *
 * Kill process owned by user with uid user id, with the given name
 * and options passed in opts, exclude_pid contains id of process not to kill.
 *
 * @param[in] uid          Process owner user id
 * @param[in] name         Name of process to kill
 * @param[in] opts         Options for getting processes
 * @param[in] exclude_pid  Exclude process pid
 * @return    none
 *
 * @fn  void process_kill (const char *name)
 *
 * @brief  Kill current user process with the given name.
 *
 * @param[in] name  Name of process to kill
 * @return    none
 *
 * @fn  void process_kill_except_current (const char *name)
 *
 * @brief  Kills process like process_kill without killing current process.
 *
 * @param[in] name  Name of process to kill
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void       process_kill_opt            (uid_t       uid,
                                        const char *name,
                                        int         opts,
                                        int         exclude_pid);

void       process_kill                (const char *name);

void       process_kill_except_current (const char *name);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int process_kill_all_opt (uid_t       uid,
 *                                const char *name,
 *                                int         opts,
 *                                int         exclude_pid)
 *
 * @brief  Kill all process with the given name. Function with more
 *         options.
 *
 * Kill all process owned by user with uid user id, with the given name
 * and options passed in opts, exclude_pid contains id of process not to kill.
 *
 * @param[in] uid          Process owner user id
 * @param[in] name         Name of process to kill
 * @param[in] opts         Options for getting processes
 * @param[in] exclude_pid  Exclude process pid
 * @return    Number of killed processes.
 *
 * @fn  int process_kill_all (const char *name)
 *
 * @brief  Kill all current user processes with the given name.
 *
 * @param[in] name  Name of process to kill
 * @return    Number of killed processes.
 *
 * @fn  int process_kill_all_except_current (const char *name)
 *
 * @brief  Kills all processes like process_kill without killing current
 *         process.
 *
 * @param[in] name  Name of process to kill
 * @return    Number of killed processes.
 */
/*----------------------------------------------------------------------------*/
int        process_kill_all_opt            (uid_t       uid,
                                            const char *name,
                                            int         opts,
                                            int         exclude_pid);

int        process_kill_all                (const char *name);

int        process_kill_all_except_current (const char *name);
/*----------------------------------------------------------------------------*/
#endif

