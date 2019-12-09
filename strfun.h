/**
 * @file  strfun.h
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
 * @brief  String operations 
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef STRFUN_H
#define STRFUN_H
#include <stddef.h>
/*----------------------------------------------------------------------------*/
/**
 * @fn        char *str_dup (const char *s_str)
 * @brief     Duplicate string.
 * @param[in] s_str  String to duplicate
 * @return    Duplicated string or null pointer
 *
 * @fn        char *str_ndup (const char *s_str, size_t st_len)
 * @brief     Duplicate n bytes of string.
 * @param[in] s_str   String to duplicate
 * @param[in] st_len  Number of bytes to duplicate
 * @return    Duplicated string or null pointer
 */
/*----------------------------------------------------------------------------*/
char * str_dup  (const char   *s_str);

char * str_ndup (const char   *s_str,
                 const size_t  ul_len);
/*----------------------------------------------------------------------------*/
#endif

