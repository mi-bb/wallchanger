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

#include <stdio.h>
#include <dirent.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "errs.h"
#include "flist.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Hash function.
 *
 * @param[in]  str  String to count hash
 * @return     Hash value
 */
uint64_t hash(const char *str);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get file extenstion.
 *
 * @param[in] s_fn String with file path
 * @return    String with file extension
 */
char *get_file_ext (const char *s_fn);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create or resize dynamic array
 *
 * @param[out] v_ptr Pointer to data to create or resize
 * @param[in]  num   Number of elements to allocate
 * @param[in]  size  Size of each element
 * @return     Result of operation
 */
int create_resize (void **v_ptr, const size_t num, const size_t size);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate n bytes of string.
 *
 * @param[in]  s_str   String to duplicate
 * @param[in]  st_len  Number of bytes to duplicate
 * @return     Duplicated string or null pointer
 */
char *str_ndup (const char *s_str, size_t st_len);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate string.
 *
 * @param[in]  s_str  String to duplicate
 * @return     Duplicated string or null pointer
 */
char *str_dup (const char *s_str);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file.
 *
 * @param[in]  s_fname File name
 * @param[out] s_buff  Pointer to destination buffer
 * @return     Reading status
 */
int read_file_data (const char *s_fname, char **s_buff);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file and count hash.
 *
 * @param[in]  s_fname File name
 * @param[out] s_buff  Pointer to destination buffer
 * @param[out] i_hash  Pointer to hash
 * @return     Reading status
 */
int read_file_data_hash (const char *s_fname, char **s_buff,
                         uint64_t *i_hash);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save a bunch of data to file.
 *
 * @param[in]  s_fname File name
 * @param[in]  s_buff  Buffer with data
 * @return     Writting status
 */
int save_file_data (const char *s_fname, const char *s_buff);
/*----------------------------------------------------------------------------*/
/**
 * @brief  String compare function for getting string position function.
 *
 * @param[in]  a  String a 
 * @param[in]  b  String b
 * @return     Compare result
 */
int compare_strings (const char *a, const char *b);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory content in FList 
 *
 * @param[in]   s_path1   Path to directory
 * @param[out]  fl_files  List of files in directory
 * @return      none
 */
void get_directory_content_append_to_flist (const char *s_path1,
                                            FList *fl_files);
/*----------------------------------------------------------------------------*/
#endif

