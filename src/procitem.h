/**
 * @file  procitem.h
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
 * @brief  ProcItem structure and functions.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef PROCITEM_H
#define PROCITEM_H
/*----------------------------------------------------------------------------*/
/**
 * @struct ProcItem
 *
 * @brief  Single process info item with its PID and name
 *
 * @var   ProcItem::s_pid
 * @brief Process PID
 *
 * @var   ProcItem::s_name
 * @brief Process name
 */
typedef struct
ProcItem {
    char *s_pid;
    char *s_name;
} ProcItem;
/*----------------------------------------------------------------------------*/
/**
 * @fn  const char * procitem_get_pid (const ProcItem *pi_item)
 *
 * @brief  Get PID string from ProcItem item.
 *
 * @param[in] pi_item  ProcItem with PID to get
 * @return    String with PID
 *
 * @fn void procitem_set_pid (ProcItem   *pi_item,
 *                            const char *s_pid)
 *
 * @brief  Set PID in ProcItem item.
 *
 * @param[out] pi_item  ProcItem to set PID
 * @param[in]  s_pid    String with PID to set
 * @return     none
 *
 * @fn  const char * procitem_get_name (const ProcItem *pi_item)
 *
 * @brief  Get name string from ProcItem item.
 *
 * @param[in] pi_item  ProcItem with name to get
 * @return    String with name
 *
 * @fn  void procitem_set_name (ProcItem   *pi_item,
 *                              const char *s_name)
 *
 * @brief  Set name in ProcItem item.
 *
 * @param[out] pi_item  ProcItem to set name
 * @param[in]  s_name   String with name to set
 * @return     none
 */
/*----------------------------------------------------------------------------*/
const char * procitem_get_pid       (const ProcItem   *pi_item)
             __attribute__ ((pure));

void         procitem_set_pid       (ProcItem         *pi_item,
                                     const char       *s_pid);

const char * procitem_get_name      (const ProcItem   *pi_item)
             __attribute__ ((pure));

void         procitem_set_name      (ProcItem         *pi_item,
                                     const char       *s_name);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void procitem_free (ProcItem *pi_item)
 *
 * @brief  Free ProcItem item.
 *
 * @param[in] pi_item  ProcItem item to free
 * @return    none
 *
 * @fn  ProcItem * procitem_new (void)
 *
 * @brief  Create new ProcItem item.
 *
 * @return New ProcIem item, it should be freed using procitem_free.
 *
 * @fn  ProcItem * procitem_new_from_data (const char *s_pid,
 *                                        const char *s_name)
 *
 * @brief  Create new ProcItem item from given pid and name string.
 *
 * @param[in] s_pid   String with PID
 * @param[in] s_name  String with name
 * @return    New ProcIem item, it should be freed using procitem_free.
 *
 * @fn  ProcItem * procitem_copy (const ProcItem *pi_item)
 *
 * @brief  Copy ProcItem item and return it.
 *
 * @param[in] pi_item  ProcItem item to copy
 * @return    New ProcIem item, it should be freed using procitem_free.
 */
/*----------------------------------------------------------------------------*/
void         procitem_free          (ProcItem         *pi_item);

ProcItem   * procitem_new           (void)
             __attribute__ ((returns_nonnull));

ProcItem   * procitem_new_from_data (const char       *s_pid,
                                     const char       *s_name)
             __attribute__ ((returns_nonnull));

ProcItem   * procitem_copy          (const ProcItem   *pi_item);
/*----------------------------------------------------------------------------*/
#endif

