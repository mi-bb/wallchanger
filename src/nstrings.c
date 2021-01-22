/**
 * @file  nstrings.c
 * @copyright Copyright (C) 2019-2021 Michał Bąbik
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
 * @brief  n strings structure and functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdlib.h>
#include <err.h>
#include "nstrings.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new NStrings item.
 */
NStrings *
nstrings_new (const size_t ui_cnt,
              const int    i_type)
{
    NStrings *ns_data = NULL;
    size_t          i = 0;

    if ((ns_data = malloc (sizeof (NStrings))) == NULL)
        err (EXIT_FAILURE, NULL);

    if ((ns_data->s_str = malloc ((ui_cnt + 1) * sizeof (char*))) == NULL)
        err (EXIT_FAILURE, NULL);

    if (i_type == NS_VAL_NULL) {
        for (i = 0; i < ui_cnt; ++i) {
            ns_data->s_str[i] = NULL;
        }
    }
    else {
        for (i = 0; i < ui_cnt; ++i) {
            ns_data->s_str[i] = malloc (sizeof (char));
            ns_data->s_str[i][0] = '\0';
        }
    }
    ns_data->ui_cnt = ui_cnt;
    ns_data->s_str[ui_cnt] = NULL;

    return ns_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free NStrings item data.
 */
void
nstrings_free (NStrings *ns_data)
{
    for (size_t i = 0; i < ns_data->ui_cnt; ++i) {
        free (ns_data->s_str[i]);
    }
    free (ns_data);
}
/*----------------------------------------------------------------------------*/

