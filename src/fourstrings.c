/**
 * @file  fourstrings.c
 * @copyright Copyright (C) 2019-2020 Michał Bąbik
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
 * @brief  Four strings structure and functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdlib.h>
#include <err.h>
#include "fourstrings.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new FourStrings item.
 */
FourStrings *
fourstrings_new (void)
{
    FourStrings *fs_data = NULL;

    if ((fs_data = malloc (sizeof (FourStrings))) == NULL)
        err (EXIT_FAILURE, NULL);

    fs_data->s_str1 = NULL;
    fs_data->s_str2 = NULL;
    fs_data->s_str3 = NULL;
    fs_data->s_str4 = NULL;

    return fs_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free FourStrings item data.
 */
void
fourstrings_free (FourStrings *fs_data)
{
    free (fs_data->s_str1);
    free (fs_data->s_str2);
    free (fs_data->s_str3);
    free (fs_data->s_str4);
    free (fs_data);
}
/*----------------------------------------------------------------------------*/

