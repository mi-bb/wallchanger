/**
 * @file  procitem.c
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
#include "procitem.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Initialize ProcItem item.
 *
 * @param[out] pi_item  ProcItem item
 * @return     none
 */
static void
procitem_init (ProcItem *pi_item)
{
    pi_item->s_pid  = nullptr;
    pi_item->s_name = nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set PID in ProcItem item.
 */
void
procitem_set_pid (ProcItem   *pi_item,
                  const char *s_pid)
{
    size_t ui_len = 0;

    ui_len = strlen (s_pid);
    cres ((void **) &pi_item->s_pid, (ui_len + 1), sizeof (char));

    memcpy (pi_item->s_pid, s_pid, ui_len);
    pi_item->s_pid [ui_len] = '\0';
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set name in ProcItem item.
 */
void
procitem_set_name (ProcItem   *pi_item,
                   const char *s_name)
{
    size_t ui_len = 0;

    ui_len = strlen (s_name);
    cres ((void **) &pi_item->s_name, (ui_len + 1), sizeof (char));

    memcpy (pi_item->s_name, s_name, ui_len);
    pi_item->s_name [ui_len] = '\0';
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free ProcItem item.
 */
void
procitem_free (ProcItem *pi_item)
{
    free (pi_item->s_pid);
    free (pi_item->s_name);
    free (pi_item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new ProcItem item.
 */
ProcItem *
procitem_new (void)
{
    ProcItem *pi_new = nullptr;

    if ((pi_new = malloc (sizeof (ProcItem))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    procitem_init (pi_new);

    return pi_new;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new ProcItem item from given pid and name string.
 */
ProcItem *
procitem_new_from_data (const char *s_pid,
                        const char *s_name)
{
    ProcItem *pi_new;

    pi_new = procitem_new ();

    procitem_set_pid (pi_new, s_pid);
    procitem_set_name (pi_new, s_name);

    return pi_new;
}
/*----------------------------------------------------------------------------*/

