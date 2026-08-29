/**
 * @file  proclist.c
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
#include "proclist.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Initialize ProcList item.
 *
 * @param[out] pl_list  ProcList item
 * @return     none
 */
static void
proclist_init (ProcList *pl_list)
{
    pl_list->cnt = 0;
    pl_list->pi_list = nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free ProcList list.
 */
void
proclist_free (ProcList *pl_list)
{
    while (pl_list->cnt--) {
        procitem_free (pl_list->pi_list[pl_list->cnt]);
    }
    free (pl_list->pi_list);
    free (pl_list);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new ProcList list.
 */
ProcList *
proclist_new (void)
{
    ProcList *pl_new = nullptr;

    if ((pl_new = malloc (sizeof (ProcList))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    proclist_init (pl_new);

    return pl_new;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert ProcItem to ProcList.
 */
void
proclist_insert (ProcList *pl_list,
                 ProcItem *pi_item)
{
    ProcItem **pi_temp = nullptr;

    if (pl_list->pi_list == nullptr) {
        pl_list->pi_list = malloc ((pl_list->cnt + 1) * sizeof (ProcItem*));
        if (pl_list->pi_list == nullptr) {
            err (EXIT_FAILURE, nullptr);
        }
    }
    else {
        pi_temp = realloc (pl_list->pi_list,
                           (pl_list->cnt + 1) * sizeof (ProcItem*));
        if (pi_temp == nullptr) {
            proclist_free (pl_list);
            err (EXIT_FAILURE, nullptr);
        }
        pl_list->pi_list = pi_temp;
    }
    pl_list->pi_list [pl_list->cnt++] = pi_item;
}
/*----------------------------------------------------------------------------*/

