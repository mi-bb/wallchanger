/**
 * @file  proc_list.c
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
 * @brief  ProcList structure and functions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <err.h>
#include "proc_list.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   PROC_LIST_INITIAL_CAPACITY
 * @brief Number of item slots allocated on the first insert
 */
constexpr uint_fast32_t PROC_LIST_INITIAL_CAPACITY = 16;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free ProcList list.
 */
void
proc_list_free (ProcList *list)
{
    while (list->cnt--) {
        proc_item_free (list->pi_list[list->cnt]);
    }
    free (list->pi_list);
    free (list);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new ProcList list.
 */
ProcList *
proc_list_new (void)
{
    ProcList *list = nullptr;

    if ((list = malloc (sizeof (ProcList))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    list->cnt      = 0;
    list->capacity = 0;
    list->pi_list  = nullptr;

    return list;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert ProcItem to proc_list, the list takes ownership of item.
 */
void
proc_list_insert (ProcList *list,
                  ProcItem *item)
{
    ProcItem      **resized  = nullptr;
    uint_fast32_t   capacity = 0;

    if (list->cnt == list->capacity) {
        capacity = list->capacity ? list->capacity * 2
                                  : PROC_LIST_INITIAL_CAPACITY;
        resized = realloc (list->pi_list, capacity * sizeof (ProcItem *));
        if (resized == nullptr) {
            err (EXIT_FAILURE, nullptr);
        }
        list->pi_list  = resized;
        list->capacity = capacity;
    }
    list->pi_list [list->cnt++] = item;
}
/*----------------------------------------------------------------------------*/

