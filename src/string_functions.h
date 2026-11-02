/**
 * @file  string_functions.h
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
#ifndef STRING_FUNCTIONS_H
#define STRING_FUNCTIONS_H

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
 * @fn  char * str_replace_in (const char *src,
 *                             const char *from, const char *to)
 *
 * @brief  Replace one string with another in a given src string.
 *
 * Function searches in src string for from and replaces it with to
 * string.
 * Return value is the final result.
 *
 * @param[in] src  Pointer to source string to examine
 * @param[in] from Pointer to string with "change from" text
 * @param[in] to   Pointer to string with "change to" text
 * @return    New string
 *
 * @fn  char * str_set_up_wallpaper_command (const char *cmd,
 *                                           const char *fname,
 *                                           const char *sign)
 *
 * @brief  Make wallpapet set command.
 *
 * @param[in] cmd    Wallpaper set command 
 * @param[in] fname  Wallpaper file name 
 * @param[in] sign   String to swap with wallpapet name
 * @return    String with command
 */
/*----------------------------------------------------------------------------*/
int    str_compare                  (const char   *a,
                                     const char   *b)
                                     __attribute__ ((pure));

char * str_replace_in               (const char   *src,
                                     const char   *from,
                                     const char   *to)
       __attribute__ ((nonnull (1,2,3), returns_nonnull));

char * str_set_up_wallpaper_command (const char   *cmd,
                                     const char   *fname,
                                     const char   *sign)
       __attribute__ ((nonnull (1,2,3), returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Copy num chars from src to dest and add a null sign.
 *
 *         Copy num chars from src to dest and add a null sign.
 *         If a null sign is found in src before num chars, copy
 *         process will stop and null sign will be added.
 *
 * @param[out] dest  Destination string buffer
 * @param[in]  src   Source string
 * @param[in]  num   Number of chars to copy
 * @return     none
 */
void   str_ncpy (char         * __restrict dest,
                 const char   * __restrict src,
                 const size_t  num)
       __attribute__ ((nonnull (1,2)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Combine 2 strings into one.
 *
 * @param[in] str1  First string
 * @param[in] str2  Second string
 * @return    String made of str1 and str2
 */
char * str_comb (const char *str1,
                 const char *str2);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append string src to dst.
 *
 * @param[out] dst Pointer to result append string process
 * @param[in]  src Source string which should be appended to dst
 * @return     none
 */
void str_append (char       **dst,
                 const char  *src);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace newline 0A characters in str string to space.
 *
 * @param[in,out] str  String to process
 * @return        none
 */
void newline_to_space (char *str);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if string is empty.
 *
 * @param[in] str  Input string
 * @return    1 if string is empty, 0 if not
 */
static inline int str_is_empty (const char *str) {
    return (str == nullptr || str[0] == '\0');
}
/*----------------------------------------------------------------------------*/
#endif

