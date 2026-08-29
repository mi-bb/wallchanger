/**
 * @file  hashfun.c
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
 * @brief  Hash function
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include "hashfun.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  djb2 hash function.
 */
uint_fast32_t
hash (const char * __restrict str)
{
    uint_fast32_t hash = 5381;
    int_fast32_t  c    = 0;

    while ((c = *str++)) {

        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}
/*----------------------------------------------------------------------------*/

