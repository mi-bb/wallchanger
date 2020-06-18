/**
 * @file  errs.c
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
 * @brief  Error info
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdio.h>
#include "errs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Function returns string with error message.
 */
const char *
err_get_message (const int i_err)
{
    const char *s_res = NULL;

    switch (i_err) {

        case ERR_OK:
            s_res = "All OK";
            break;

        case ERR_FILE:
            s_res = "File error (cannot access / bad permissions)";
            break;

        case ERR_FILE_RW:
            s_res = "File read / write error";
            break;

        case ERR_FILE_CR:
            s_res = "File / directory can not be created";
            break;

        case ERR_FILE_RM:
            s_res = "File / directory can not be removed";
            break;

        case ERR_FILE_EX:
            s_res = "File does not exist";
            break;

        case ERR_ALLOC:
            s_res = "Alloc error";
            break;

        case ERR_TYPE:
            s_res = "Wrong type passed";
            break;

        case ERR_CFG_NOF:
            s_res = "Could not find a config file";
            break;

        default:
           break;
    }

    return s_res;
}
/*----------------------------------------------------------------------------*/

