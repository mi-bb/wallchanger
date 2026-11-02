/**
 * @file  nstrings.h
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
#ifndef NSTRINGS_H
#define NSTRINGS_H
#include <stddef.h>
/*----------------------------------------------------------------------------*/
/**
 * Type of strings to create.
 */
enum e_nstring_type : int {
    NS_VAL_NULL,  /**< Create null strings */
    NS_VAL_EMPTY  /**< Create empty string */
};
/*----------------------------------------------------------------------------*/
/**
 * @struct NStrings
 *
 * @brief  Structure for storing n strings.
 *
 * @var   NStrings::str
 * @brief Array of strings
 *
 * @var   NStrings::count
 * @brief Number of strings
 */
typedef struct
NStrings {
    char   **str;
    size_t   count;
} NStrings;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new NStrings item.
 *
 * @param[in] count  Number of strings
 * @param[in] type   Type of strings
 * @return  New NStrings item.
 */
[[nodiscard]]
NStrings * nstrings_new  (const size_t count,
                          const int    type);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free NStrings item data.
 *
 * @param[out] nstrings  NStrings item to free.
 * @return     none
 */
void       nstrings_free (NStrings    *nstrings);
/*----------------------------------------------------------------------------*/
#endif

