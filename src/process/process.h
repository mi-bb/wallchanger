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
 * @fn  ProcList * process_get (const char *name,
 *                              pid_t       exclude_pid)
 *
 * @brief  Get running processes of the current user.
 *
 * On Linux the function scans the /proc directory, on FreeBSD it uses
 * libprocstat. Zombie processes (already exited, waiting to be reaped) are
 * left out. Process names are matched with process_list_contains rules, so
 * a name longer than the system keeps still matches its shortened form.
 * List with processes is returned, it should be freed later with
 * proc_list_free function. If the process list can not be read, the
 * returned list is empty.
 *
 * @param[in] name         Name of process or null to get all processes
 * @param[in] exclude_pid  PID of process to leave out, or 0 to keep all
 * @return    ProcList list with found processes
 *
 * @fn  bool process_list_contains (const ProcList *list,
 *                                  const char     *name)
 *
 * @brief  Check if list contains a process with the given name.
 *
 * The system keeps only the first 15 (Linux) or 19 (FreeBSD) characters of
 * a process name, so a process name of exactly that length also matches
 * a longer name that starts with it.
 *
 * @param[in] list  ProcList list to search
 * @param[in] name  Name of process to find
 * @return    true if a process with the name is on the list, false if not
 */
/*----------------------------------------------------------------------------*/
[[nodiscard]]
ProcList * process_get           (const char     *name,
                                  pid_t           exclude_pid)
           __attribute__ ((returns_nonnull));

[[nodiscard]]
bool       process_list_contains (const ProcList *list,
                                  const char     *name)
           __attribute__ ((pure));
/*----------------------------------------------------------------------------*/
/**
 * @fn  int process_count_except_current (const char *name)
 *
 * @brief  Count current user processes with the given name, the calling
 *         process is not counted.
 *
 * @param[in] name  Name of process or null to count all processes
 * @return    Number of found processes
 *
 * @fn  int process_kill_all_except_current (const char *name)
 *
 * @brief  Send SIGTERM to all current user processes with the given name,
 *         except the calling process.
 *
 * A warning is printed for every process that could not be signalled.
 *
 * @param[in] name  Name of process to kill, null kills nothing
 * @return    Number of processes signalled successfully
 */
/*----------------------------------------------------------------------------*/
int        process_count_except_current    (const char *name);

int        process_kill_all_except_current (const char *name);
/*----------------------------------------------------------------------------*/
#endif

