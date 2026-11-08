/**
 * @file  proc_item.c
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
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "proc_item.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free ProcItem item.
 */
void
proc_item_free (ProcItem *item)
{
    free (item->name);
    free (item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new ProcItem item with the given pid and a copy of name.
 */
ProcItem *
proc_item_new (pid_t       pid,
               const char *name)
{
    ProcItem *item = nullptr;

    if ((item = malloc (sizeof (ProcItem))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    item->pid = pid;

    if ((item->name = strdup (name)) == nullptr)
        err (EXIT_FAILURE, nullptr);

    return item;
}
/*----------------------------------------------------------------------------*/

