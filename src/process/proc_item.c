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
#include "cres.h"
#include "proc_item.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Initialize ProcItem item.
 *
 * @param[out] item  ProcItem item
 * @return     none
 */
static void
proc_item_init (ProcItem *item)
{
    item->s_pid  = nullptr;
    item->s_name = nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set PID in ProcItem item.
 */
void
proc_item_set_pid (ProcItem   *item,
                   const char *pid)
{
    size_t len = 0;

    len = strlen (pid);
    cres ((void **) &item->s_pid, (len + 1), sizeof (char));

    memcpy (item->s_pid, pid, len);
    item->s_pid [len] = '\0';
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set name in ProcItem item.
 */
void
proc_item_set_name (ProcItem   *item,
                    const char *name)
{
    size_t len = 0;

    len = strlen (name);
    cres ((void **) &item->s_name, (len + 1), sizeof (char));

    memcpy (item->s_name, name, len);
    item->s_name [len] = '\0';
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free ProcItem item.
 */
void
proc_item_free (ProcItem *item)
{
    free (item->s_pid);
    free (item->s_name);
    free (item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new ProcItem item.
 */
ProcItem *
proc_item_new (void)
{
    ProcItem *item = nullptr;

    if ((item = malloc (sizeof (ProcItem))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    proc_item_init (item);

    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new ProcItem item from given pid and name string.
 */
ProcItem *
proc_item_new_from_data (const char *pid,
                         const char *name)
{
    ProcItem *item;

    item = proc_item_new ();

    proc_item_set_pid (item, pid);
    proc_item_set_name (item, name);

    return item;
}
/*----------------------------------------------------------------------------*/

