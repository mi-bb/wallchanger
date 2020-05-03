/**
 * @file  strfun.c
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
 * @brief  String operations 
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cres.h"
#include "strfun.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  String compare function for getting string position function.
 */
int
str_compare (const char *a,
             const char *b)
{
    int i_res = 0; /* Function result */

    if (a == NULL || b == NULL) {
        if (a == NULL && b == NULL) {
            i_res = 0;
        }
        else {
            if (a == NULL) {
                i_res = -1;
            }
            else {
                i_res = 1;
            }
        }
    }
    else {
        i_res = strcmp (a, b);
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates string with name made of s_name string and number
 *         ul_no at the end.
 */ 
char *
str_name_with_number (const char   *s_name,
                      const size_t  ul_no)
{
    char   *s_res  = NULL;   /* Result string */
    size_t  ul_tmp = ul_no;  /* Copy of number */
    size_t  ul_l   = 1;      /* Length of number string */

    while (ul_tmp /= 10)
        ul_l++;

    cres ((void**) &s_res, strlen (s_name) + ul_l + 1, sizeof (char));

    sprintf (s_res, "%s%ld", s_name, ul_no);

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace one string with another in a given src_dest string.
 */ 
char *
str_replace_in (const char *s_src,
                const char *s_fr,
                const char *s_to)
{
    const char *sp      = s_src; /* Copy s_src pointer */
    const char *pn      = NULL;  /* Find string pointer */
    char       *s_res   = NULL;  /* Result string */
    size_t      ul_len  = 0;     /* Length to allocate */
    size_t      ul_flen = 0;     /* Length of s_fr */
    size_t      ul_tlen = 0;     /* Length if s_to */

    ul_flen = strlen (s_fr);
    ul_tlen = strlen (s_to);
    ul_len  = strlen (s_src) + 1;

    cres ((void**) &s_res, ul_len, sizeof (char));
    s_res[0] = '\0';

    /* while there are "replace from" in source string */
    while ((pn = strstr (sp, s_fr)) != NULL) {

        ul_len += ul_tlen;
        ul_len -= ul_flen;

        cres ((void**) &s_res, ul_len, sizeof (char));
        /* append original text from last found up to new found */
        strncat (s_res, sp, (size_t) (pn - sp));
        /* append "replace to" */
        strcat (s_res, s_to);

        /* change source pointer to "after found" */
        sp = pn + ul_flen; 
    }
    strcat (s_res, sp);

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make command to set wallpaper.
 */ 
char *
str_set_up_wallpaper_command (const char *s_cmd,
                              const char *s_fname,
                              const char *s_sign)
{
    char *s_res = NULL; /* Result string */

    if (strstr (s_cmd, s_sign) == NULL) {
        s_res = str_comb (s_cmd, " ");
        str_append (&s_res, s_fname);
    }
    else {
        s_res = str_replace_in (s_cmd, s_sign, s_fname);
    }
    str_append (&s_res, " &");
    #ifdef DEBUG
    printf ("%s\n", s_res);
    #endif
    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Copy ui_num chars from s_src to s_dest and add a null sign.
 */
void
str_ncpy (char         * __restrict s_dest,
          const char   * __restrict s_src,
          const size_t  ui_num)
{
    size_t i = 0;
    while (i++ < ui_num && *s_src)
        *s_dest++ = *s_src++;
    *s_dest = '\0'; 
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Combine 2 strings into one.
 */
char *
str_comb (const char *s_str1,
          const char *s_str2)
{
    char   *s_ret   = NULL; /* Return string */
    size_t  ui_str1 = 0;    /* Length of str1 */
    size_t  ui_str2 = 0;    /* Length of str2 */

    if (s_str1 == NULL && s_str2 == NULL)
        return NULL;
    else if (s_str1 == NULL && s_str2 != NULL)
        return strdup (s_str2);
    else if (s_str1 != NULL && s_str2 == NULL)
        return strdup (s_str1);

    ui_str1 = strlen (s_str1);
    ui_str2 = strlen (s_str2);

    cres ((void**) &s_ret, ui_str1 + ui_str2 + 1, sizeof (char));

    memcpy (s_ret, s_str1, ui_str1);
    memcpy (s_ret + ui_str1, s_str2, ui_str2);
    s_ret[ui_str1 + ui_str2] = '\0';

    return s_ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append string s_src to s_dst.
 */
void
str_append (char       **s_dst,
            const char  *s_src)
{
    size_t ui_dst = 0; /* length of dst string */
    size_t ui_src = 0; /* Length of src string */
    int    i_zero = 0; /* Place 0 in new string */

    if (s_src == NULL)
        return;
    else
        ui_src = strlen (s_src);

    if (*s_dst == NULL)
        i_zero = 1;
    else
        ui_dst = strlen (*s_dst);

    cres ((void**) s_dst, ui_src + ui_dst + 1, sizeof (char));

    if (i_zero)
        *s_dst[0] = '\0';

    memcpy ((*s_dst)+ui_dst, s_src, ui_src);
    (*s_dst)[ui_src+ui_dst] = '\0';
}
/*----------------------------------------------------------------------------*/

