/**
 * @file  errs.h
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
#ifndef ERRS_H
#define ERRS_H
/*----------------------------------------------------------------------------*/
/**
 * @brief  My error values.
 */
enum e_error_values : int {
    ERR_OK = 0,  /**< OK */
    ERR_FILE,    /**< File error (cannot access / bad permissions) */
    ERR_FILE_RW, /**< File read / write error */
    ERR_FILE_CR, /**< File can not be created */
    ERR_FILE_RM, /**< File can not be removed */
    ERR_FILE_EX, /**< File does not exist */
    ERR_ALLOC,   /**< Alloc error */
    ERR_TYPE,    /**< Wrong type */
    ERR_CFG_NOF  /**< No config file found */
};
/*----------------------------------------------------------------------------*/
/* The GNU __attribute__ spellings used here and in fdfn.h, hashfun.h,
 * jsfun.h, dirlist.h, dlgsmsg.h and procitem.h are kept on purpose: C23's
 * nearest equivalents are not usable yet. [[unsequenced]] has to follow the
 * parameter list on GCC, and Clang does not know it at all (it is dropped
 * with -Wunknown-attributes), so it would silently lose the optimization on
 * one of the two supported compilers. [[nodiscard]], which both compilers
 * implement, is used in the standard form where it applies. */
/*----------------------------------------------------------------------------*/
/**
 * @brief  Function returns string with error message.
 *
 * @param[in]  i_err   Error number
 * @return     String with message
 */
[[nodiscard]]
const char * err_get_message (const int i_err) __attribute__ ((const));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get exit error codes for program errors.
 *
 * @param[in]  i_err   Error number
 * @return     Integer with exit error code
 */
[[nodiscard]]
int          err_ex_code     (const int i_err) __attribute__ ((const));
/*----------------------------------------------------------------------------*/
#endif
