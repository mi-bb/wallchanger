/**
 * @file  flist.h
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
 * @brief  File list FList structure and functions
 *
 * Simple structure and functions to manipulate list of files.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef FLIST_H
#define FLIST_H

#include <stdint.h>
#include <stddef.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  File list structure
 */
typedef struct
FList {
    uint32_t   i_cnt;  /**< Number of items in list */
    char     **s_file; /**< List with names */
} FList;
/*----------------------------------------------------------------------------*/
/**
 * @fn         void flist_init (FList *fl_list)
 * @brief      FList init
 * @param[out] fl_list  FList object
 * @return     none
 *
 * @fn         int flist_reserve (FList *fl_list, const size_t ul_size)
 * @brief      Reserve space for items in FList object
 * @param[out] fl_list  FList object
 * @param[in]  i_size   Size to reserve
 * @return     Reserve result 0 if OK, 1 if error occurred
 *
 * @fn         void flist_clear (FList *fl_list)
 * @brief      Clear items in FList object
 * @param[out] fl_list  FList object
 * @return     none
 *
 * @fn         void flist_free (FList *fl_list)
 * @brief      Free FList data
 * @param[out] fl_list  FList object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void flist_init    (FList        *fl_list);

int  flist_reserve (FList        *fl_list,
                    const size_t  ul_size);

void flist_clear   (FList        *fl_list);

void flist_free    (FList        *fl_list);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int flist_insert_data (FList      *fl_list,
 *                             const char *s_fn)
 * @brief      Insert data to FList
 * @param[out] fl_list  FList object
 * @param[in]  s_fn     String to insert
 * @return     Clear result 0 if OK, 1 if error occurred
 *
 * @fn  void flist_set_data (FList          *fl_list,
 *                           const uint32_t  i_pos, 
 *                           const char     *s_fn)
 * @brief      Set data on i_pos position in FList
 * @param[out] fl_list  FList object
 * @param[in]  i_pos    Position of data to set
 * @param[in]  s_fn     Data to set
 * @return     none
 */
/*----------------------------------------------------------------------------*/
int  flist_insert_data (FList          *fl_list,
                        const char     *s_fn);

void flist_set_data    (FList          *fl_list,
                        const uint32_t  i_pos, 
                        const char     *s_fn);
/*----------------------------------------------------------------------------*/
/**
 *
 * @fn  const char * flist_get_data (FList          *fl_list,
 *                                   const uint32_t  i_pos)
 * @brief     Gat data from FList
 * @param[in] fl_list  FList object
 * @param[in] i_pos    Position of name to get
 * @return    Name string or null pointer if position is not in list
 *
 * @fn  char * flist_get_data_new (FList          *fl_list,
 *                                 const uint32_t  i_pos)
 * @brief     Gat data from FList as a new string
 * @param[in] fl_list  FList object
 * @param[in] i_pos    Position of name to get
 * @return    Name string or null pointer if position is not in list,
 *            this name shoule be later freed
 *
 * @fn  uint32_t flist_get_len (const FList *fl_list)
 * @brief     Gat number of items in FList
 * @param[in] fl_list  FList object
 * @return    FList length
 *
 * @fn  int32_t flist_get_pos (const FList *fl_list,
                               const char  *s_fn)
 * @brief     Get position of given name in FList
 * @param[in] fl_list  FList object
 * @param[in] s_fn     Name of item to get position
 * @return    Position of item in list or -1 if not found, empty list or
 *            item to find if null
 */
/*----------------------------------------------------------------------------*/
const char * flist_get_data     (const FList    *fl_list,
                                 const uint32_t  i_pos);

char       * flist_get_data_new (const FList    *fl_list,
                                 const uint32_t  i_pos);

uint32_t     flist_get_len      (const FList    *fl_list);

int32_t      flist_get_pos      (const FList    *fl_list,
                                 const char     *s_fn);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void flist_remove_duplicates (FList *fl_list)
 * @brief         Remove duplicates from list.
 * @param[in,out] fl_list  FList object
 * @return        none
 *
 * @fn  void flist_filter_by_extensions_list (FList *fl_files,
 *                                            FList *fl_exts)
 * @brief         Remove from file list files that are not on provided
 *                extensions list.
 * @param[in,out] fl_files  List with files to proccess
 * @param[in]     fl_exts   List of extensions
 * @return        none
 */
/*----------------------------------------------------------------------------*/
void flist_remove_duplicates         (FList *fl_list);

void flist_filter_by_extensions_list (FList *fl_files,
                                      FList *fl_exts);
/*----------------------------------------------------------------------------*/
/**
* @brief  Print FList data
*
* @param[in] fl_list  FList object
* @return    none
*/
void flist_print_data (const FList *fl_list);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory content, add it to FList 
 *
 * @param[in]   s_path1   Path to directory
 * @param[out]  fl_files  List of files in directory
 * @return      none
 */
void get_directory_content_add_to_flist (const char *s_path1,
                                         FList      *fl_files);
/*----------------------------------------------------------------------------*/
#endif

