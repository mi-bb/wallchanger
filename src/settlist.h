/**
 * @file  settlist.h
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
 * @brief  SettList structure and functions
 * 
 * Structure and functions I made to manage settings in applications.
 * File contains SettList structure which is made to be a list for
 * Setting objects.
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef SETTLIST_H
#define SETTLIST_H

#include <stdint.h>
#include <stddef.h>
#include "setting.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct SettList
 *
 * @brief  SettList structure for list of Setting objects
 *
 * @var   SettList::st_setting
 * @brief List of settings 
 *
 * @var   SettList::i_cnt
 * @brief Number of elements in list
 */
typedef struct
SettList {
    Setting  **st_setting;  /* List of settings */
    size_t     i_cnt;       /* Number of elements in list */
} SettList;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Errors retuned in SettList functions
 */
enum {
    SET_ER_OK,      /**< Everything OK */
    SET_ER_ALLOC,   /**< Alloc error */
    SET_ER_NO_ARR,  /**< No such array in list */
    SET_ER_NO_SET,  /**< No such setting in list */
    SET_ER_NOT_ARR, /**< Item is not an array */
    SET_ER_WRONG    /**< Wrong input value */
};
/*----------------------------------------------------------------------------*/
/**
 * @fn  SettList * stlist_new_list (void)
 * @brief  Create new empty list for settings
 *
 * Function returns new empty list for settings, settings can be added using
 * stlist_insert_setting () function or stlist_insert_setting_to_array ().
 * Function should be freed using stlist_free () function.
 *
 * @return  New empty list
 *
 * @fn  void stlist_free (SettList *st_list)
 * @brief      Free SettList list
 * @param[out] st_list  SettList list of settings
 * @return     None
 *
 * @fn  void stlist_free_p (SettList *st_list)
 * @brief      Free SettList pointer list
 * @param[out] st_list  SettList list of pointers to settings
 * @return     None
 *
 * @fn  void stlist_clear (SettList *st_list)
 * @brief      Clear SettList list of Setting items
 * @param[out] st_list  SettList list of settings
 * @return     None
 */
/*----------------------------------------------------------------------------*/
SettList * stlist_new_list (void);

void       stlist_free     (SettList *st_list);

void       stlist_free_p   (SettList *st_list);

void       stlist_clear    (SettList *st_list);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void stlist_insert_setting (SettList *st_list,
 *                                  Setting  *st_val)
 *
 * @brief  Inserts Settings object to settings list
 *
 * Function inserts Setting object to list. If object with the same name 
 * as the new setting name exists in list, it will be updated with the new one.
 * If there are no settings with same name as the new one, setting will be
 * appended to the list.
 *
 * @param[out] st_list  SettList list of settings
 * @param[in]  st_val   Settings object
 * @return     none
 *
 * @fn  int stlist_insert_setting_to_array (SettList   *st_list,
 *                                          Setting    *st_val,
 *                                          const char *s_array_name)
 *
 * @brief  Inserts Settings object to array in settings list
 *
 * @param[out] st_list       SettList list of settings
 * @param[in]  st_val        Settings object
 * @param[in]  s_array_name  Name of array
 * @return     Result of operation
 */
/*----------------------------------------------------------------------------*/
void stlist_insert_setting          (SettList   *st_list,
                                     Setting    *st_val);

int  stlist_insert_setting_to_array (SettList   *st_list,
                                     Setting    *st_val,
                                     const char *s_array_name);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void stlist_remove_setting_at_pos (SettList   *st_list,
 *                                         uint32_t    ui_pos)
 * @brief      Remove Settings from list at specified position
 * @param[out] st_list       SettList list of settings
 * @param[in]  ui_pos        Position of setting to remove
 * @return     none
 *
 * @fn  void stlist_remove_setting (SettList   *st_list,
 *                                  Setting    *st_val)
 * @brief      Remove Settings from list 
 * @param[out] st_list       SettList list of settings
 * @param[in]  st_val        Settings object to remove
 * @return     none
 *
 * @fn  void stlist_remove_setting_with_name (SettList   *st_list,
 *                                            const char *s_name)
 * @brief      Remove Settings with specified name from list 
 * @param[out] st_list       SettList list of settings
 * @param[in]  s_name        Settings name
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void stlist_remove_setting_at_pos    (SettList     *st_list,
                                      const size_t  ui_pos);

void stlist_remove_setting           (SettList     *st_list,
                                      Setting      *st_val);

void stlist_remove_setting_with_name (SettList     *st_list,
                                      const char   *s_name);
                                      //__attribute__ ((nonnull (2)));
/*----------------------------------------------------------------------------*/
/**
 * @fn  uint32_t stlist_get_length (const SettList *st_list)
 * @brief     Get number of settings in list
 * @param[in] st_list  SettList list of settings
 * @return    Number of settings in list
 *
 * @fn  Setting * stlist_get_setting_at_pos (const SettList *st_list,
 *                                           const uint32_t  ui_pos)
 * @brief     Get pointer to Setting object at given position in SettList
 *
 * Function returns pointer to Setting item. It should not be freed,
 * it will be freed at SettList free operation.
 *
 * @param[in] st_list  SettList list of settings
 * @param[in] ui_pos   Position of setting to get
 * @return    Setting object or null
 *
 * @fn  Setting * stlist_get_setting_with_name (const SettList *st_list,
 *                                              const char     *s_name)
 * @brief     Get pointer to Setting object with name provided in s_name
 *
 * Function returns pointer to Setting item. It should not be freed,
 * it will be freed at SettList free operation.
 *
 * @param[in] st_list  SettList list of settings
 * @param[in] s_name   Name of setting to get
 * @return    Setting object or null
 *
 * @fn  int32_t stlist_get_setting_pos (const SettList *st_list,
 *                                      const char     *s_name)
 * @brief     Get position of setting with name s_name on list
 * @param[in] st_list  SettList list of settings
 * @param[in] s_name   Name of setting
 * @return    Position in list or -1 if not found
 *
 * @fn  int32_t stlist_get_setting_val_str_pos (const SettList *st_list,
 *                                              const char     *s_val)
 * @brief     Get position on list of setting with string type and value
 *            s_val
 * @param[in] st_list  SettList list of settings
 * @param[in] s_val    String value
 * @return    Position in list or -1 if not found
 */
/*----------------------------------------------------------------------------*/
size_t       stlist_get_length              (const SettList *st_list)
             __attribute__ ((pure));

Setting    * stlist_get_setting_at_pos      (const SettList *st_list,
                                             const size_t    ui_pos)
             __attribute__ ((pure));

Setting    * stlist_get_setting_with_name   (const SettList *st_list,
                                             const char     *s_name)
             __attribute__ ((pure));
             //__attribute__ ((pure, nonnull (2)));

int_fast32_t stlist_get_setting_pos         (const SettList *st_list,
                                             const char     *s_name)
             __attribute__ ((pure));
             //__attribute__ ((pure, nonnull (2)));

int_fast32_t stlist_get_setting_val_str_pos (const SettList *st_list,
                                             const char     *s_val)
             __attribute__ ((pure, nonnull (2)));
/*----------------------------------------------------------------------------*/
/**
 * @fn  SettList * stlist_get_settings_in_array_name (const SettList *st_list,
 *                                                    const char     *s_name)
 * @brief  Get SettList list of Setting objects stored in array.
 *
 * Function returns list of Setting objects assigned to an array with
 * s_name name.
 * If there is no setting with name s_name on st_list or Setting object is not
 * an array, null pointer will be returned.
 * The new returned list should be later freed using stlist_free () function.
 *
 * @param[in]  st_list  SettList list of settings
 * @param[in]  s_name   Name of array to get objects
 * @return     SettList list or null
 *
 * @fn SettList * stlist_get_settings_in_array_name_p (const SettList *st_list,
 *                                                     const char     *s_name)
 * @brief  Get SettList list of pointers to Setting objects stored in array.
 *
 * Function is similar to stlist_get_settings_in_array_name. The difference is
 * that it returns list of pointers to settings instead of list with copies.
 * The returned list shluld be later freed with stlist_free_p () function.
 *
 * @param[in]  st_list  SettList list of settings
 * @param[in]  s_name   Name of array to get objects
 * @return     SettList list of pointers or null
 *
 * @fn  SettList * stlist_get_settings_in_array_obj (const SettList *st_list,
 *                                                   const Setting  *st_array)
 * @brief  Get SettList list of Setting objects stored in array.
 *
 * Function works like stlist_get_settings_in_array_name, only difference is
 * that an array is passed as an object not a name.
 * The new returned list should be later freed using stlist_free () function.
 *
 * @param[in]  st_list  SettList list of settings
 * @param[in]  st_array Array type Setting to get objects
 * @return     SettList list or null
 *
 * @fn  SettList * stlist_get_settings_in_array_obj_p (const SettList *st_list,
 *                                                     const Setting  *st_array)
 * @brief  Get SettList list of pointers to Setting objects stored in array.
 *
 * Function is similar to stlist_get_settings_in_array_obj. The difference is
 * that it returns list of pointers to settings instead of list with copies.
 * The returned list shluld be later freed with stlist_free_p () function.
 *
 * @param[in]  st_list  SettList list of settings
 * @param[in]  st_array Array type Setting to get objects
 * @return     SettList list of pointers or null
 *
 * @fn  SettList * stlist_get_settings_main (const SettList *st_list)
 * @brief  Get Setting objects (vals and arrays) stord in SettList top level.
 *
 * Function returns list of settings (values and arrays) from st_list
 * setting list.
 * Returned list will have only top level settings, without settings assigned
 * to arrays.
 * The new returned list should be later freed using stlist_free () function.
 *
 * @param[in]  st_list  SettList list of Setting objects
 * @return     SettList list or null
 *
 * @fn  SettList * stlist_get_settings_main_p (const SettList *st_list)
 * @brief  Get pointers to Setting objects (vals and arrays) stord in
 *         SettList top level.
 *
 * Function is similar to stlist_get_settings_main. The difference is
 * that it returns list of pointers to settings instead of list with copies.
 * The returned list shluld be later freed with stlist_free_p () function.
 *
 * @param[in]  st_list  SettList list of Setting objects
 * @return     SettList list of pointers or null
 */
/*----------------------------------------------------------------------------*/
SettList * stlist_get_settings_in_array_name   (const SettList *st_list,
                                                const char     *s_name);
           //__attribute__ ((nonnull (2)));

SettList * stlist_get_settings_in_array_name_p (const SettList *st_list,
                                                const char     *s_name);
//           __attribute__ ((nonnull (2)));

SettList * stlist_get_settings_in_array_obj    (const SettList *st_list,
                                                const Setting  *st_array);

SettList * stlist_get_settings_in_array_obj_p  (const SettList *st_list,
                                                const Setting  *st_array);

SettList * stlist_get_settings_main            (const SettList *st_list);

SettList * stlist_get_settings_main_p          (const SettList *st_list);
#ifdef DEBUG
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print content of SettList
 *
 * @param[in]  st_list  SettList list of Setting objects
 * @return     None
 */
void stlist_print_content (const SettList *st_list);
/*----------------------------------------------------------------------------*/
#endif
#endif

