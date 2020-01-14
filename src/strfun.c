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

    create_resize ((void**) &s_res, strlen (s_name) + ul_l + 1, sizeof (char));

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
    const char *sp      = s_src; /* copy s_src pointer */
    const char *pn      = NULL;  /* find string pointer */
    char       *s_res   = NULL;  /* result string */
    size_t      ul_len  = 0;     /* length to allocate */
    size_t      ul_flen = 0;     /* length of s_fr */
    size_t      ul_tlen = 0;     /* length if s_to */

    ul_flen = strlen (s_fr);
    ul_tlen = strlen (s_to);
    ul_len  = strlen (s_src) + 1;

    create_resize ((void**) &s_res, ul_len, sizeof (char));

    /* find the first occurence of "replace from" */
    pn = strstr (sp, s_fr); 

    /* while there are "replace from" in source string */
    while (pn != NULL) {

        ul_len += ul_tlen;
        ul_len -= ul_flen;

        create_resize ((void**) &s_res, ul_len, sizeof (char));

        /* append original text from last found up to new found */
        strncat (s_res, sp, (size_t) (pn - sp));
        /* append "replace to" */
        strcat (s_res, s_to);

        /* change source pointer to "after found" */
        sp = pn + ul_flen; 
        /* find another "replace from" str in src */
        pn = strstr (sp, s_fr);  
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
    char   *s_res  = NULL;
    size_t  ul_siz = 0;

    if (strstr (s_cmd, s_sign) == NULL) {

        const char * __restrict cp = s_cmd;
        const char * __restrict fp = s_fname;

        ul_siz = strlen (cp) + strlen (fp) + 4;

        create_resize ((void**) &s_res, ul_siz, sizeof (char));

        char * __restrict rp = s_res;

        while (*cp) {
            *rp++ = *cp++;
        }

        *rp++ = ' ';

        while (*fp) {
            *rp++ = *fp++;
        }

        *rp++ = ' ';
        *rp = '&';
    }
    else {
        s_res = str_replace_in (s_cmd, s_sign, s_fname);
        ul_siz = strlen (s_res) + 3;
        create_resize ((void**) &s_res, ul_siz, sizeof (char));
        strcat (s_res, " &");
    }
    return s_res;
}
/*----------------------------------------------------------------------------*/

