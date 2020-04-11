/**
 * @file  proclist.h
 * @copyright Copyright (C) 2020 Michał Bąbik
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
 * @brief  ProcList structure and functions.
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef PROCLIST_H
#define PROCLIST_H

#include <stdint.h>
#include "procitem.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct ProcList
 *
 * @brief  List with process information strored in ProcItem items.
 *
 * @var   ProcList::cnt
 * @brief ProcItem items count
 *
 * @var   ProcList::pi_list
 * @brief List with ProcItem items
 */
typedef struct
ProcList {
    uint_fast32_t   cnt;
    ProcItem      **pi_list;
} ProcList;
/*----------------------------------------------------------------------------*/
/**
 * @fn  void proclist_free (ProcList *pl_list)
 *
 * @brief  Free ProcList list.
 *
 * @param[in] pl_list  ProcList list to free
 * @return    none
 *
 * @fn  ProcList * proclist_new (void)
 *
 * @brief  Create new ProcList list.
 *
 * @return New ProcList list, it should be freed using proclist_free.
 */
/*----------------------------------------------------------------------------*/
void             proclist_free     (ProcList       *pl_list);

ProcList       * proclist_new      (void);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void proclist_insert (ProcList *pl_list,
 *                            ProcItem *pi_item)
 *
 * @brief  Insert ProcItem to ProcList.
 *
 * @param[out] pl_list  ProcList list 
 * @param[in]  pi_item  ProcItem to insert
 * @return     none
 *
 * @fn  const ProcItem * proclist_get_item (const ProcList *pl_list,
 *                                          uint_fast32_t   pos)
 *
 * @brief  Get ProcItem from list.
 *
 * @param[in] pl_list  ProcList list 
 * @param[in] pos      Position of ProcItem item to get 
 * @return    Pointer to ProcItem item, it should not be freed
 *
 * @fn  uint_fast32_t proclist_get_cnt (const ProcList *pl_list)
 *
 * @brief  Get number of items in ProcList.
 *
 * @param[in] pl_list  ProcList list 
 * @return    Number of items in the list
 */
/*----------------------------------------------------------------------------*/
void             proclist_insert   (ProcList       *pl_list,
                                    ProcItem       *pi_item);

const ProcItem * proclist_get_item (const ProcList *pl_list,
                                    uint_fast32_t   pos)
                 __attribute__ ((pure));

uint_fast32_t    proclist_get_cnt  (const ProcList *pl_list)
                 __attribute__ ((pure));
/*----------------------------------------------------------------------------*/
#endif

