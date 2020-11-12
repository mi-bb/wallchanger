/**
 * @file  fourstrings.h
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
#ifndef FOURSTRINGS_H
#define FOURSTRINGS_H
/*----------------------------------------------------------------------------*/
/**
 * @struct FourStrings
 *
 * @brief  Structure for storing four strings.
 *
 * @var   FourStrings::s_str1
 * @brief First string
 *
 * @var   FourStrings::s_str2
 * @brief Second string
 *
 * @var   FourStrings::s_str3
 * @brief Third string
 *
 * @var   FourStrings::s_str4
 * @brief Fourth string
 *
 * @var   FourStrings::s_str
 * @brief Pointer array to strings
 */
typedef struct
FourStrings {
    char *s_str1;
    char *s_str2;
    char *s_str3;
    char *s_str4;
    char **s_str[4];
} FourStrings;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new FourStrings item.
 *
 * @return  New FourStrings item.
 */
FourStrings * fourstrings_new  (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free FourStrings item data.
 *
 * @param[out] fs_data  FourStrings item to free.
 * @return     none
 */
void          fourstrings_free (FourStrings *fs_data);
/*----------------------------------------------------------------------------*/
#endif

