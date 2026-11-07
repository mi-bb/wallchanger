/**
 * @file  cres.c
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
 * @brief  Function to create / resize dynamic array
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <err.h>
#include "cres.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create or resize dynamic array
 */
void
cres (void        **pointer,
      const size_t  count,
      const size_t  size)
{
    void *tmp = nullptr; /* Temp pointer for realloc */

    if (size == 0 || count == 0) {
        if (*pointer != nullptr) {
            free (*pointer);
            *pointer = nullptr;
        }
        return;
    }
    else {
        if (*pointer == nullptr) {
            *pointer = malloc (count * size);

            if (*pointer == nullptr) {
                err (EXIT_FAILURE, nullptr);
            }
        }
        else {
            tmp = realloc (*pointer, count * size);

            if (tmp == nullptr) {
                free (*pointer);
                err (EXIT_FAILURE, nullptr);
            }
            else {
                *pointer = tmp;
            }
        }
    }
}
/*----------------------------------------------------------------------------*/

