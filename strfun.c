/**
 * @file  strfun.c
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
#include <stdlib.h>
#include <string.h>
#include "strfun.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate string.
 */
char *
str_dup (const char *s_str)
{
    char   *s_res  = NULL;
    size_t  ul_len = 0;

    if (s_str == NULL)
        return NULL;

    ul_len = strlen (s_str);

    if (ul_len == 0)
        return NULL;

    s_res = calloc (ul_len+1, sizeof (char));

    if (s_res == NULL)
        return NULL;

    memcpy (s_res, s_str, ul_len);

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate n bytes of string.
 */
char *
str_ndup (const char   *s_str,
          const size_t  ul_len)
{
    char *s_res = NULL;

    if (s_str == NULL || ul_len == 0)
        return NULL;

    s_res = calloc (ul_len + 1, sizeof (char));

    if (s_res == NULL)
        return NULL;

    memcpy (s_res, s_str, ul_len);

    return s_res;
}
/*----------------------------------------------------------------------------*/

