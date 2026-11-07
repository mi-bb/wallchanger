/**
 * @file  string_functions.c
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
 * @brief  String operations 
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cres.h"
#include "string_functions.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  String compare function for getting string position function.
 */
int
str_compare (const char *a,
             const char *b)
{
    int result = 0; /* Function result */

    if (a == nullptr || b == nullptr) {
        if (a == nullptr && b == nullptr) {
            result = 0;
        }
        else {
            if (a == nullptr) {
                result = -1;
            }
            else {
                result = 1;
            }
        }
    }
    else {
        result = strcmp (a, b);
    }
    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace one string with another in a given src string.
 */ 
char *
str_replace_in (const char *src,
                const char *from,
                const char *to)
{
    const char *sp       = src; /* Copy src pointer */
    const char *pn       = nullptr;  /* Find string pointer */
    char       *result   = nullptr;  /* Result string */
    size_t      len      = 0;   /* Length to allocate */
    size_t      from_len = 0;   /* Length of from */
    size_t      to_len   = 0;   /* Length of to */

    from_len = strlen (from);
    to_len   = strlen (to);
    len      = strlen (src) + 1;

    cres ((void**) &result, len, sizeof (char));
    result[0] = '\0';

    /* while there are "replace from" in source string */
    while ((pn = strstr (sp, from)) != nullptr) {

        len += to_len;
        len -= from_len;

        cres ((void**) &result, len, sizeof (char));
        /* append original text from last found up to new found */
        strncat (result, sp, (size_t) (pn - sp));
        /* append "replace to" */
        strcat (result, to);

        /* change source pointer to "after found" */
        sp = pn + from_len; 
    }
    strcat (result, sp);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make command to set wallpaper.
 */ 
char *
str_set_up_wallpaper_command (const char *cmd,
                              const char *fname,
                              const char *sign)
{
    char *result = nullptr; /* Result string */

    if (strstr (cmd, sign) == nullptr) {
        result = str_comb (cmd, " \"");
        str_append (&result, fname);
        str_append (&result, "\"");
    }
    else {
        result = str_replace_in (cmd, sign, fname);
    }
    str_append (&result, " &");
    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Copy num chars from src to dest and add a null sign.
 */
void
str_ncpy (char         * __restrict dest,
          const char   * __restrict src,
          const size_t  num)
{
    size_t i = 0;
    while (i++ < num && *src)
        *dest++ = *src++;
    *dest = '\0'; 
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Combine 2 strings into one.
 */
char *
str_comb (const char *str1,
          const char *str2)
{
    char   *result = nullptr; /* Return string */
    size_t  str1_len = 0;    /* Length of str1 */
    size_t  str2_len = 0;    /* Length of str2 */

    if (str1 == nullptr && str2 == nullptr)
        return nullptr;
    else if (str1 == nullptr && str2 != nullptr)
        return strdup (str2);
    else if (str1 != nullptr && str2 == nullptr)
        return strdup (str1);

    str1_len = strlen (str1);
    str2_len = strlen (str2) + 1;

    cres ((void**) &result, str1_len + str2_len, sizeof (char));

    memcpy (result, str1, str1_len);
    memcpy (result + str1_len, str2, str2_len);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append string src to dst.
 */
void
str_append (char       **dst,
            const char  *src)
{
    size_t dst_len = 0; /* length of dst string */
    size_t src_len = 0; /* Length of src string */

    if (src == nullptr)
        return;
    else
        src_len = strlen (src) + 1;

    if (*dst != nullptr)
        dst_len = strlen (*dst);

    cres ((void**) dst, src_len + dst_len, sizeof (char));

    memcpy ((*dst)+dst_len, src, src_len);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace newline 0A characters in str string to space.
 */
void
newline_to_space (char *str)
{
    while ((str = strchr (str, '\n')) != nullptr) {
        *str++ = ' ';
    }
}
/*----------------------------------------------------------------------------*/

