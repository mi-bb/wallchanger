/**
 * @file  proc_list.h
 * @copyright Copyright (C) 2020-2026 Michał Bąbik
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
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef PROC_LIST_H
#define PROC_LIST_H

#include <stdint.h>
#include <stddef.h>
#include "proc_item.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct ProcList
 *
 * @brief  List with process information stored in ProcItem items.
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
 * @fn  void proc_list_free (ProcList *pl_list)
 *
 * @brief  Free ProcList list.
 *
 * @param[in] pl_list  ProcList list to free
 * @return    none
 *
 * @fn  ProcList * proc_list_new (void)
 *
 * @brief  Create new ProcList list.
 *
 * @return New ProcList list, it should be freed using proc_list_free.
 */
/*----------------------------------------------------------------------------*/
void             proc_list_free     (ProcList       *pl_list);

[[nodiscard]]
ProcList       * proc_list_new      (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert ProcItem to proc_list.
 *
 * @param[out] pl_list  ProcList list 
 * @param[in]  pi_item  ProcItem to insert
 * @return     none
 */
void             proc_list_insert   (ProcList       *pl_list,
                                     ProcItem       *pi_item);
/*----------------------------------------------------------------------------*/
/**
 * @fn  const ProcItem * proc_list_get_item (const ProcList *pl_list,
 *                                           uint_fast32_t   pos)
 *
 * @brief  Get ProcItem from list.
 *
 * @param[in] pl_list  ProcList list 
 * @param[in] pos      Position of ProcItem item to get 
 * @return    Pointer to ProcItem item, it should not be freed
 *
 * @fn  uint_fast32_t proc_list_get_count (const ProcList *pl_list)
 *
 * @brief  Get number of items in proc_list.
 *
 * @param[in] pl_list  ProcList list 
 * @return    Number of items in the list
 */
/*----------------------------------------------------------------------------*/
static inline const ProcItem * proc_list_get_item  (const ProcList *pl_list,
                                                    uint_fast32_t   pos) {
    return pos < pl_list->cnt ? (const ProcItem *) pl_list->pi_list[pos] : nullptr;
}
static inline uint_fast32_t    proc_list_get_count (const ProcList *pl_list) {
    return pl_list->cnt;
}
/*----------------------------------------------------------------------------*/
#endif

