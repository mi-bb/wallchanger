/**
 * @file  errors.c
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
 * @brief  Error info
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <sysexits.h>
#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Function returns string with error message.
 */
const char *
err_get_message (const int err)
{
    const char *result = nullptr;

    switch (err) {

        case ERR_OK:
            result = "All OK";
            break;

        case ERR_FILE:
            result = "File error (cannot access / bad permissions)";
            break;

        case ERR_FILE_RW:
            result = "File read / write error";
            break;

        case ERR_FILE_CR:
            result = "File / directory can not be created";
            break;

        case ERR_FILE_RM:
            result = "File / directory can not be removed";
            break;

        case ERR_FILE_EX:
            result = "File does not exist";
            break;

        case ERR_ALLOC:
            result = "Alloc error";
            break;

        case ERR_TYPE:
            result = "Wrong type passed";
            break;

        case ERR_CFG_NOF:
            result = "Could not find a config file";
            break;

        default:
           break;
    }
    return result;
}
/*----------------------------------------------------------------------------*/
int
err_ex_code (const int err)
{
    int result = EX_OK;

    switch (err) {

        case ERR_OK:
            result = EX_OK;
            break;
        case ERR_FILE:
            result = EX_NOINPUT;
            break;
        case ERR_FILE_RW:
            result = EX_IOERR;
            break;
        case ERR_FILE_CR:
            result = EX_CANTCREAT;
            break;
        case ERR_FILE_RM:
            result = EX_IOERR;
            break;
        case ERR_FILE_EX:
            result = EX_NOINPUT;
            break;
        case ERR_ALLOC:
            result = EXIT_FAILURE;
            break;
        case ERR_TYPE:
            result = EXIT_FAILURE;
            break;
        case ERR_CFG_NOF:
            result = EX_UNAVAILABLE;
            break;
        default:
           break;
    }
    return result;
}
/*----------------------------------------------------------------------------*/

