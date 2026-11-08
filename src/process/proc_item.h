/**
 * @file  proc_item.h
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
 * @brief  ProcItem structure and functions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef PROC_ITEM_H
#define PROC_ITEM_H

#include <sys/types.h>
/*----------------------------------------------------------------------------*/
/**
 * @struct ProcItem
 *
 * @brief  Single process info item with its PID and name
 *
 * @var   ProcItem::pid
 * @brief Process PID
 *
 * @var   ProcItem::name
 * @brief Process name
 */
typedef struct
ProcItem {
    pid_t  pid;
    char  *name;
} ProcItem;
/*----------------------------------------------------------------------------*/
/**
 * @fn  pid_t proc_item_get_pid (const ProcItem *item)
 *
 * @brief  Get PID from ProcItem item.
 *
 * @param[in] item  ProcItem with PID to get
 * @return    Process PID
 *
 * @fn  const char * proc_item_get_name (const ProcItem *item)
 *
 * @brief  Get name string from ProcItem item.
 *
 * @param[in] item  ProcItem with name to get
 * @return    String with name
 */
/*----------------------------------------------------------------------------*/
static inline pid_t        proc_item_get_pid  (const ProcItem *item) {
    return item->pid;
}

static inline const char * proc_item_get_name (const ProcItem *item) {
    return (const char *) item->name;
}
/*----------------------------------------------------------------------------*/
/**
 * @fn  void proc_item_free (ProcItem *item)
 *
 * @brief  Free ProcItem item.
 *
 * @param[in] item  ProcItem item to free
 * @return    none
 *
 * @fn  ProcItem * proc_item_new (pid_t       pid,
 *                                const char *name)
 *
 * @brief  Create new ProcItem item with the given pid and a copy of name.
 *
 * @param[in] pid   Process PID
 * @param[in] name  Process name
 * @return    New ProcItem item, it should be freed using proc_item_free.
 */
/*----------------------------------------------------------------------------*/
void         proc_item_free (ProcItem   *item);

[[nodiscard]]
ProcItem   * proc_item_new  (pid_t       pid,
                             const char *name)
             __attribute__ ((returns_nonnull));
/*----------------------------------------------------------------------------*/
#endif

