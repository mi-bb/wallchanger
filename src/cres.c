/**
 * @file  cres.c
 * @copyright Copyright (C) 2019 Michał Bąbik
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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */

#include <stdlib.h>
#include <stdio.h>
#include "errs.h"
#include "cres.h"

/*----------------------------------------------------------------------------*/
/**
 * @brief  Create or resize dynamic array
 */
void
cres (void        **v_ptr,
      const size_t  ul_num,
      const size_t  ul_size)
{
    void *s_tmp = NULL; /* Temp pointer for realloc */

    if (ul_size == 0 || ul_num == 0) {

        if (*v_ptr != NULL) {

            free (*v_ptr);
            *v_ptr = NULL;
        }
        return;
    }
    else {
        if (*v_ptr == NULL) {

            /**v_ptr = calloc (ul_num, ul_size);*/
            *v_ptr = malloc (ul_num * ul_size);

            if (*v_ptr == NULL) {

                fputs ("Alloc error\n", stderr);
                exit (EXIT_FAILURE);
            }
        }
        else {
            s_tmp = realloc (*v_ptr, ul_num * ul_size);

            if (s_tmp == NULL) {

                free (*v_ptr);

                fputs ("Alloc error\n", stderr);
                exit (EXIT_FAILURE);
            }
            else {

            *v_ptr = s_tmp;

            }
        }
    }
}
/*----------------------------------------------------------------------------*/

