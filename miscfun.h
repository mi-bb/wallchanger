/**
 * @file  miscfun.h
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
 * @brief  Other functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef MISCFUN_H
#define MISCFUN_H

#include "flist.h"
#define FN_LEN 1000
/*----------------------------------------------------------------------------*/
/**
 * @brief  Hash function.
 *
 * @param[in]  str  String to count hash
 * @return     Hash value
 */
uint64_t hash (const char *str);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create or resize dynamic array
 *
 * @param[out] v_ptr Pointer to data to create or resize
 * @param[in]  num   Number of elements to allocate
 * @param[in]  size  Size of each element
 * @return     Result of operation
 */
int create_resize (void         **v_ptr,
                   const size_t   ul_num, 
                   const size_t   ul_size);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if config file and path exists, check read/write permissions. 
 *
 * Checks config path and file existence, creates them if needed. Function
 * returns checking/creating status and writes config file path to s_file if
 * process completed succefully.
 *
 * @param[out]  i_err  Error output
 * @return      Config file path
 */
/*----------------------------------------------------------------------------*/
char * check_config_path_file (int *i_err);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int read_file_data (const char *s_fname, char **s_buff)
 * @brief      Read some data from file.
 * @param[in]  s_fname File name
 * @param[out] s_buff  Pointer to buffer to write data
 * @return     Reading status
 *
 * @fn  int read_file_data_hash (const char  *s_fname,
 *                               char       **s_buff,
 *                               uint64_t    *i_hash)
 * @brief      Read some data from file and count hash.
 * @param[in]  s_fname File name
 * @param[out] s_buff  Pointer to destination buffer
 * @param[out] i_hash  Pointer to hash
 * @return     Reading status
 *
 * @fn  int save_file_data (const char *s_fname, const char *s_buff)
 * @brief      Save a bunch of data to file.
 * @param[in]  s_fname File name
 * @param[in]  s_buff  Buffer with data
 * @return     Writting status
 */
/*----------------------------------------------------------------------------*/
int read_file_data      (const char   *s_fname,
                         char        **s_buff);

int read_file_data_hash (const char   *s_fname,
                         char        **s_buff,
                         uint64_t     *i_hash);

int save_file_data      (const char   *s_fname,
                         const char   *s_buff);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int compare_strings (const char *a, const char *b)
 * @brief     String compare function for getting string position function.
 * @param[in] a  String a 
 * @param[in] b  String b
 * @return    Compare result
 *
 * @fn  char * string_name_with_number (const char   *s_name,
 *                                      const size_t  i_no)
 * @brief      Creates string with name made of s_name string and number
 *             i_no at the end.
 * @param[in]  s_name  Input string
 * @param[out] i_no    Number to append
 * @return     String with name and number to null if could not alloc memory
 *
 * @fn  char * string_replace_in (const char *s_src,
 *                                const char *s_fr, const char *s_to)
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
 * @fn  char * set_up_wallpaper_command (const char *s_cmd,
 *                                       const char *s_fname,
 *                                       const char *s_sign)
 * @brief     Make wallpapet set command.
 * @param[in] s_cmd    Wallpaper set command 
 * @param[in] s_fname  Wallpaper file name 
 * @param[in] s_sign   String to swap with wallpapet name
 * @return    String with command
 */
/*----------------------------------------------------------------------------*/
int    compare_strings          (const char   *a,
                                 const char   *b);

char * string_name_with_number  (const char   *s_name,
                                 const size_t  ul_no);

char * string_replace_in        (const char   *s_src,
                                 const char   *s_fr,
                                 const char   *s_to);

char * set_up_wallpaper_command (const char   *s_cmd,
                                 const char   *s_fname,
                                 const char   *s_sign);
/*----------------------------------------------------------------------------*/
#endif

