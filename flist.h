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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef FLIST_H
#define FLIST_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "errs.h"
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
 * @brief  FList init
 *
 * @param[out] fl_list  FList object
 * @return     none
 */
void flist_init (FList *fl_list);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reserve space for items in FList object
 *
 * @param[out] fl_list  FList object
 * @param[in]  i_size   Size to reserve
 * @return     Reserve result 0 if OK, 1 if error occurred
 */
int flist_reserve (FList *fl_list, const size_t i_size);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clear items in FList object
 *
 * @param[out] fl_list  FList object
 * @return     none
 */
void flist_clear (FList *fl_list);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free FList data
 *
 * @param[out] fl_list  FList object
 * @return     none
 */
void flist_free (FList *fl_list);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert data to FList
 *
 * @param[out] fl_list  FList object
 * @param[in]  s_fn     String to insert
 * @return     Clear result 0 if OK, 1 if error occurred
 */
int flist_insert_data (FList *fl_list, const char *s_fn);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set data on i_pos position in FList
 *
 * @param[out] fl_list  FList object
 * @param[in]  i_pos    Position of data to set
 * @param[in]  s_fn     Data to set
 * @return     none
 */
void flist_set_data (FList *fl_list, const uint32_t i_pos, const char *s_fn);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Gat data from FList
 *
 * @param[in] fl_list  FList object
 * @param[in] i_pos    Position of name to get
 * @return    Name string or null pointer if position is not in list
 */
const char *flist_get_data (FList *fl_list, const uint32_t i_pos);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Gat data from FList as a new string
 *
 * @param[in] fl_list  FList object
 * @param[in] i_pos    Position of name to get
 * @return    Name string or null pointer if position is not in list,
 *            this name shoule be later freed
 */
char *flist_get_data2 (FList *fl_list, const uint32_t i_pos);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Gat number of items in FList
 *
 * @param[in] fl_list  FList object
 * @return    FList length
 */
uint32_t flist_get_len (FList *fl_list);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get position of given name in FList
 *
 * @param[in] fl_list  FList object
 * @param[in] s_fn     Name of item to get position
 * @return    Position of item in list or -1 if not found, empty list or
 *            item to find if null
 */
int32_t flist_get_pos (FList *fl_list, const char *s_fn);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print FList data
 *
 * @param[in] fl_list  FList object
 * @return    none
 */
void flist_print_data (FList *fl_list);
/*----------------------------------------------------------------------------*/
#endif
