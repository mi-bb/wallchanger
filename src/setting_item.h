/**
 * @file  setting_item.h
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
 * @brief  Setting structure and functions
 *
 * Structure and functions I made to manage settings in applications.
 * This file contains a Setting structure and functions to manage it.
 * It is a single setting object that can contain one value of selected type.
 * Value can be a 64, 32, 16, 8 bit signed or unsigned integer, a double
 * value or a string.
 * Setting object may be also set as an array type. Array type do not hold any
 * value in it. It can be a parent to other settings assigned to it.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef SETTING_ITEM_H
#define SETTING_ITEM_H

#include <inttypes.h>
#include <stddef.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting types 
 */
typedef enum
SetValType : int {
    SET_VAL_INT,     /**< Integer (64 bit) */
    SET_VAL_UINT,    /**< Unsigned integer (64 bit) */
    SET_VAL_DOUBLE,  /**< double */
    SET_VAL_STRING,  /**< string */
    SET_VAL_SETTING, /**< setting type */
    SET_VAL_ARRAY,   /**< array of values */
    SET_VAL_NULL     /**< null item */
} SetValType;
/*----------------------------------------------------------------------------*/
/**
 * @struct Setting
 *
 * @brief  Setting structure 
 *
 * @var   Setting::prev
 * @brief Pointer to previous setting
 * @var   Setting::next
 * @brief Pointer to next setting
 * @var   Setting::parent
 * @brief Pointer to parent setting
 * @var   Setting::v_type
 * @brief Type of setting
 * @var   Setting::s_name
 * @brief Name of setting, it must be longer than 2 letters because of hash
 *        function (could make not unique results for smaller strings).
 * @var   Setting::hash
 * @brief Setting's hash based on setting name
 * @var   Setting::data
 * @brief Struncture with setting value
 *
 * @union data_t
 * @brief Setting's data values
 *
 * @var   data_t::i_val
 * @brief Integer value
 * @var   data_t::ui_val
 * @brief Unsigned integer value
 * @var   data_t::d_val
 * @brief Double type number value
 * @var   data_t::s_val
 * @brief String value
 * @var   data_t::st_child
 * @brief Pointer to child setting
 */
typedef struct 
Setting {
    struct Setting *prev;
    struct Setting *next;
    struct Setting *parent;
    SetValType      v_type;
    char           *s_name;
    uint_fast32_t   hash;
    union data_t {
        int64_t         i_val;
        uint64_t        ui_val;
        double          d_val;
        char           *s_val;
        struct Setting *st_child;
    } data; 
} Setting;
/*----------------------------------------------------------------------------*/
/**
 * @fn  SetValType setting_get_type (const Setting *setting)
 *
 * @brief  Get Setting type value
 *
 * @param[in] setting  Setting to get type
 * @return    Setting type value
 *
 * @fn  const char * setting_get_name (const Setting *setting)
 *
 * @brief  Get Setting name string
 *
 * @param[in] setting  Setting to get name
 * @return    Name string
 *
 * @fn  uint_fast32_t setting_get_hash (const Setting *setting)
 *
 * @brief  Get Setting hash value
 *
 * @param[in] setting  Setting to get hash
 * @return    Hash value
 */
/*----------------------------------------------------------------------------*/
static inline SetValType    setting_get_type (const Setting *setting) {
    return setting != nullptr ? setting->v_type : SET_VAL_NULL;
}
static inline const char  * setting_get_name (const Setting *setting) {
    return setting != nullptr ? setting->s_name : nullptr;
}
static inline uint_fast32_t setting_get_hash (const Setting *setting) {
    return setting != nullptr ? setting->hash : 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_get_parent (const Setting *setting)
 *
 * @brief  Get Setting's parent Setting
 *
 * @param[in] setting  Setting item
 * @return    Parent Setting
 *
 * @fn  Setting * setting_get_child (const Setting *setting)
 *
 * @brief  Get Setting's child Setting
 *
 * @param[in] setting  Setting item
 * @return    Child Setting
 */
/*----------------------------------------------------------------------------*/
static inline Setting * setting_get_parent (const Setting *setting) {
    return setting != nullptr ? setting->parent : nullptr;
}

Setting * setting_get_child  (const Setting *setting);
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_next (const Setting *setting)
 *
 * @brief  Get next Setting in list
 *
 * @param[in] setting  Setting item
 * @return    Next Setting in list or null
 *
 * @fn  Setting * setting_prev (const Setting *setting)
 *
 * @brief  Get previous Setting in list
 *
 * @param[in] setting  Setting item
 * @return    Previous Setting in list or null
 */
/*----------------------------------------------------------------------------*/
static inline Setting * setting_next (const Setting *setting) {
    return setting != nullptr ? setting->next : nullptr;
}
static inline Setting * setting_prev (const Setting *setting) {
    return setting != nullptr ? setting->prev : nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @fn  void setting_free (Setting *setting)
 *
 * @brief  Free Setting data
 *
 * @param[in]  setting  Setting object
 * @return     none
 *
 * @fn  void settings_free_all (Setting *setting)
 *
 * @brief  Free all settings in given Setting
 *
 * @param[in]  setting  Setting object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void setting_free      (Setting *setting);

void settings_free_all (Setting *setting);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int64_t setting_get_int (const Setting *setting)
 *
 * @brief  Get integer value stored in Setting
 *
 * It is set to hold and return 64 bit integer value.
 *
 * @param[in]  setting  Setting object
 * @return     64 bit integer or 0 if incorrect type to get
 *
 * @fn  uint64_t setting_get_uint (const Setting *setting)
 *
 * @brief  Get unsigned integer value stored in Setting
 *
 * It is set to hold and return 64 bit unsigned integer value.
 *
 * @param[in]  setting  Setting object
 * @return     64 bit unsigned integer or 0 if incorrect type to get
 *
 * @fn  double setting_get_double (const Setting *setting)
 *
 * @brief  Get double type value stored in Setting
 *
 * @param[in]  setting  Setting object
 * @return     Double value or 0 if incorrect type to get
 *
 * @fn  const char * setting_get_string (const Setting *setting)
 *
 * @brief  Get text string stored in Setting
 *
 * @param[in]  setting  Setting object
 * @return     String or null if incorrect type to get
 */
/*----------------------------------------------------------------------------*/
static inline int64_t      setting_get_int    (const Setting *setting) {
    return setting_get_type (setting) == SET_VAL_INT ? setting->data.i_val : 0;
}
static inline uint64_t     setting_get_uint   (const Setting *setting) {
    return setting_get_type (setting) == SET_VAL_UINT ?
        setting->data.ui_val : 0;
}
static inline double       setting_get_double (const Setting *setting) {
    return setting_get_type (setting) == SET_VAL_DOUBLE ?
        setting->data.d_val : 0;
}
static inline const char * setting_get_string (const Setting *setting) {
    return setting_get_type (setting) == SET_VAL_STRING ? 
        (const char *) setting->data.s_val : nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set setting to string type and its content to val.
 *
 * @param[in,out] setting  Setting object
 * @param[in]     val      String value to set
 * @return        none
 */
void         setting_set_string (Setting       *setting,
                                 const char    *val);
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_new_setting (const char *name)
 * @brief      Create new Setting object, set it to be a setting type and its
 *             name to the given name.
 * @param[in]  name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_array (const char *name)
 * @brief      Create new Setting object, set it to be an array type and its
 *             name to the given name.
 * @param[in]  name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_int (const char *name, const int64_t val)
 * @brief      Create new Setting object for an integer (64 bit), set its
 *             value to val and name to the given name.
 * @param[in]  name  Name of the setting (should be longer than 2 letters)
 * @param[in]  val   Value to store in setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint (const char *name, const uint64_t val)
 * @brief      Create new Setting object for an unsigned integer (64 bit),
 *             set its value to val and name to the given name.
 * @param[in]  name  Name of the setting (should be longer than 2 letters)
 * @param[in]  val   Value to store in setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_double (const char *name, const double val)
 * @brief      Create new Setting object for a double, set its value to val
 *             and name to the given name.
 * @param[in]  name  Name of the setting (should be longer than 2 letters)
 * @param[in]  val   Value to store in setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_string (const char *name, const char *val)
 * @brief      Create new Setting object for a string, set its value to val
 *             and name to the given name.
 * @param[in]  name  Name of the setting (should be longer than 2 letters)
 * @param[in]  val   Value to store in setting
 * @return     New Setting object
 */
/*----------------------------------------------------------------------------*/
[[nodiscard]]
Setting * setting_new_setting (const char     *name)
          __attribute__ ((returns_nonnull));

[[nodiscard]]
Setting * setting_new_array   (const char     *name)
          __attribute__ ((returns_nonnull));

[[nodiscard]]
Setting * setting_new_int     (const char     *name,
                               const int64_t   val)
          __attribute__ ((returns_nonnull));

[[nodiscard]]
Setting * setting_new_uint    (const char     *name,
                               const uint64_t  val)
          __attribute__ ((returns_nonnull));

[[nodiscard]]
Setting * setting_new_double  (const char     *name,
                               const double    val)
          __attribute__ ((returns_nonnull));

[[nodiscard]]
Setting * setting_new_string  (const char     *name,
                               const char     *val)
          __attribute__ ((returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_first (Setting *settings)
 *
 * @brief  Find and return first item in a Setting list.
 *
 * @param[in] settings  List of Setting items
 * @return    Pointer to first element on list or null if list is empty
 *
 * @fn  Setting * setting_last (Setting *settings)
 *
 * @brief  Find and return last item in a Setting list.
 *
 * @param[in] settings  List of Setting items
 * @return    Pointer to last element on list or null if list is empty
 *
 * @fn  Setting * setting_get_top_parent (Setting *setting)
 *
 * @brief  Get top parent Setting of given Setting item.
 *
 * @param[in] setting  Setting to get its top parent
 * @return    Pointer to top parent Setting item
 *
 * @fn  Setting * settings_find (Setting *settings, const char *name)
 *
 * @brief  Find Setting with given name in list starting at position of input
 *         Setting item.
 *
 * @param[in] settings  List of Setting items
 * @param[in] name      Name of Setting to be found
 * @return    Pointer to found Setting or null if not found
 *
 * @fn  Setting * setting_get_at_pos (Setting      *settings,
 *                                    const size_t  pos)
 *
 * @brief  Find and return Setting item at given posision in settings
 *         list.
 *
 * @param[in] settings  List of Setting items
 * @param[in] pos       Position of setting to get
 * @return    Pointer to Setting item or null if not found
 */
/*----------------------------------------------------------------------------*/
Setting * setting_first          (Setting       *settings);

Setting * setting_last           (Setting       *settings);

Setting * setting_get_top_parent (Setting       *setting);

Setting * settings_find          (Setting       *settings,
                                  const char    *name);

Setting * setting_get_at_pos     (Setting       *settings,
                                  const size_t   pos);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count number of Setting items present in given settings list.
 *
 * @param[in] settings  List of Setting items
 * @return    Number of Setting items in settings list
 */
size_t    settings_count         (const Setting *settings);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove Setting from list.
 *
 * @param[in] setting  Setting to remove
 * @return    Pointer to beginning of list
 */
Setting * setting_remove         (Setting *setting);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace Setting old_item in list with new_item.
 *
 * @param[in,out] old_item  Old Setting item to be replaced
 * @param[in,out] new_item  New Setting item to replace old one
 * @return        Pointer to first element in Setting list
 */
Setting * setting_replace        (Setting       *old_item,
                                  Setting       *new_item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find setting in list and replace.
 *
 * Function is looking in Setting list for a setting with the same
 * name as setting. After finding such setting it will be replaced
 * with setting and the function stops (only the first match is
 * replaced). Function returns beginning of Setting list.
 *
 * @param[in,out] list     List of Setting items
 * @param[in,out] setting  Destination Setting
 * @return        Beginning of Setting list
 */
Setting * settings_find_replace  (Setting       *list,
                                  Setting       *setting);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void setting_add_child (Setting *parent,
 *                              Setting *child)
 *
 * @brief  Append Setting child to child list of Setting parent
 *
 * @param[in,out] parent  Parent Setting to add child Setting
 * @param[in,out] child   Child Setting to add to parent's list
 * @return        none
 *
 * @fn  void settings_prepend (Setting *list,
 *                             Setting *setting)
 *
 * @brief  Prepend setting item to the list.
 *
 * @param[in]  list     List of Setting items
 * @param[out] setting  Setting item to prepend
 * @return     none
 *
 * @fn  void settings_append (Setting *list,
 *                            Setting *setting)
 *
 * @brief  Append setting item to the list.
 *
 * @param[in]  list     List of Setting items
 * @param[out] setting  Setting item to append
 * @return     none
 *
 * @fn  void settings_append_or_ignore (Setting *list,
 *                                      Setting *setting)
 *
 * @brief  Append Setting item to the list or ignore append if
 *         setting with same name exists on it.
 *
 * @param[in]  list     List of Setting items
 * @param[out] setting  Setting item to append
 * @return     none
 *
 * @fn  void settings_append_or_replace (Setting *list,
 *                                       Setting *setting)
 *
 * @brief  Append Setting item to the list and replace if
 *         setting with same name exists on it.
 *
 * @param[in]  list     List of Setting items
 * @param[out] setting  Setting item to append
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void      setting_add_child          (Setting *parent,
                                      Setting *child);

Setting * settings_prepend           (Setting *list,
                                      Setting *setting);

Setting * settings_append            (Setting *list,
                                      Setting *setting);

void      settings_append_or_ignore  (Setting *list,
                                      Setting *setting);

Setting * settings_append_or_replace (Setting *list,
                                      Setting *setting);
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_get_top_level (Setting *setting)
 *
 * @brief  Get top level Setting on list
 *
 * @param[in]  setting  Setting from list
 * @return     Top level setting in list
 *
 * @fn  size_t setting_count_children (const Setting *setting)
 *
 * @brief  Count children in Setting object or array
 *
 * @param[in]  setting  Setting from list
 * @return     Number of items in List
 *
 * @fn  Setting * setting_find_child (const Setting *setting,
 *                                    const char *name)
 *
 * @brief  Find child with given name in Setting object or array
 *
 * @param[in]  setting  Setting from list
 * @param[in]  name     Name of Setting to find
 * @return     Found Setting item or null of not found
 */
/*----------------------------------------------------------------------------*/
static inline Setting * setting_get_top_level  (Setting       *setting) {
    return setting_first (setting_get_top_parent (setting));
}
static inline size_t    setting_count_children (const Setting *setting) {
    return settings_count (setting_get_child (setting));
}
static inline Setting * setting_find_child     (const Setting *setting,
                                                const char    *name) {
    return settings_find (setting_get_child (setting), name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings have equal names.
 *
 * @param[in]  first   First setting
 * @param[in]  second  Second setting
 * @return     Compare result. 1 if they are qual 0 if they are not
 */
static inline int settings_equal_names (const Setting *first,
                                        const Setting *second) {
    return first->hash == second->hash;
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print setting info
 *
 * @param[out]  setting  Setting object
 * @return      none
 */
void setting_print  (const Setting *setting);

void settings_print (const Setting *setting);
/*----------------------------------------------------------------------------*/
#endif
#endif

