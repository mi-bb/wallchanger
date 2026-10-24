/**
 * @file  nstrings.c
 * @copyright Copyright (C) 2019-2026 Michał Bąbik
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
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <err.h>
#include "nstrings.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new NStrings item.
 */
NStrings *
nstrings_new (const size_t count,
              const int    type)
{
    NStrings *nstrings = nullptr;
    size_t           i = 0;

    if ((nstrings = malloc (sizeof (NStrings))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    if ((nstrings->str = malloc ((count + 1) * sizeof (char*))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    if (type == NS_VAL_NULL) {
        for (i = 0; i < count; ++i) {
            nstrings->str[i] = nullptr;
        }
    }
    else {
        for (i = 0; i < count; ++i) {
            nstrings->str[i] = malloc (sizeof (char));
            nstrings->str[i][0] = '\0';
        }
    }
    nstrings->count = count;
    nstrings->str[count] = nullptr;

    return nstrings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free NStrings item data.
 */
void
nstrings_free (NStrings *nstrings)
{
    for (size_t i = 0; i < nstrings->count; ++i) {
        free (nstrings->str[i]);
    }
    free (nstrings);
}
/*----------------------------------------------------------------------------*/

