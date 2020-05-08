/**
 * @file  strfun.h
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
#ifndef STRFUN_H
#define STRFUN_H

#include <stddef.h>
/*----------------------------------------------------------------------------*/
/**
 * @fn  int str_compare (const char *a, const char *b)
 *
 * @brief  String compare function for getting string position function.
 *
 * @param[in] a  String a 
 * @param[in] b  String b
 * @return    Compare result
 *
 * @fn  char * str_name_with_number (const char   *s_name,
 *                                          const size_t  ul_no)
 *
 * @brief  Creates string with name made of s_name string and number
 *         ul_no at the end.
 *
 * @param[in]  s_name  Input string
 * @param[out] ul_no   Number to append
 * @return     String with name and number to null if could not alloc memory
 *
 * @fn  char * str_replace_in (const char *s_src,
 *                             const char *s_fr, const char *s_to)
 *
 * @brief  Replace one string with another in a given src_dest string.
 *
 * Function searches in src_dest string for s_fr and replaces it with s_to
 * string.
 * Return value is the final result.
 *
 * @param[in] s_src  Pointer to source string to examine
 * @param[in] s_fr   Pointer to string with "change from" text
 * @param[in] s_to   Pointer to string with "change to" text
 * @return    New string
 *
 * @fn  char * str_set_up_wallpaper_command (const char *s_cmd,
 *                                           const char *s_fname,
 *                                           const char *s_sign)
 *
 * @brief  Make wallpapet set command.
 *
 * @param[in] s_cmd    Wallpaper set command 
 * @param[in] s_fname  Wallpaper file name 
 * @param[in] s_sign   String to swap with wallpapet name
 * @return    String with command
 */
/*----------------------------------------------------------------------------*/
int    str_compare                  (const char   *a,
                                     const char   *b)
                                     __attribute__ ((pure));

char * str_replace_in               (const char   *s_src,
                                     const char   *s_fr,
                                     const char   *s_to)
       __attribute__ ((nonnull (1,2,3), returns_nonnull));


char * str_set_up_wallpaper_command (const char   *s_cmd,
                                     const char   *s_fname,
                                     const char   *s_sign)
       __attribute__ ((nonnull (1,2,3), returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Copy ui_num chars from s_src to s_dest and add a null sign.
 *
 *         Copy ui_num chars from s_src to s_dest and add a null sign.
 *         If a null sign is found in s_src before ui_num chars, copy
 *         process will stop and null sign will be added.
 *
 * @param[out] s_dest  Destination string buffer
 * @param[in]  s_src   Source string
 * @param[in]  ui_num  Number of chars to copy
 * @return     none
 */
void   str_ncpy (char         * __restrict s_dest,
                 const char   * __restrict s_src,
                 const size_t  ui_num)
       __attribute__ ((nonnull (1,2)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Combine 2 strings into one.
 *
 * @param[in] s_str1  First string
 * @param[in] s_str2  Second string
 * @return    String made of s_str1 and s_str2
 */
char * str_comb (const char *s_str1,
                 const char *s_str2);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append string s_src to s_dst.
 *
 * @param[out] s_dst Pointer to result append string process
 * @param[in]  s_src Source string which should be appended to s_dst
 * @return     none
 */
void str_append (char       **s_dst,
                 const char  *s_src);
/*----------------------------------------------------------------------------*/
#endif

