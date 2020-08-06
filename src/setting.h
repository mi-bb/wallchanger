/**
 * @file  setting.h
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
 * @brief  Setting structure and functions
 *
 * Structure and functions I made to manage settings in applications.
 * This file contains a Setting structure and functions to manage it.
 * It is a sigle setting object that can contain one value of selected type.
 * Value can be a 64, 32, 16, 8 bit signed or usigned integer, a double
 * value or a string.
 * Setting object may be also set as an array type. Array type do not hold any
 * value in it. It can be a parent to other settings assigned to it.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef SETTING_H
#define SETTING_H

#include <inttypes.h>
#include <stddef.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting types 
 */
typedef enum
SetValType {
    SET_VAL_INT,     /**< Integer (64 bit) */
    SET_VAL_UINT,    /**< Unsigned integer (64 bit) */
    SET_VAL_DOUBLE,  /**< double */
    SET_VAL_STRING,  /**< string */
    SET_VAL_SETTING, /**< setting type */
    SET_VAL_ARRAY,   /**< array of values */
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
 * @fn  SetValType setting_get_type (const Setting *st_setting)
 *
 * @brief  Get Setting type value
 *
 * @param[in] st_setting  Setting to get type
 * @return    Setting type value
 *
 * @fn  const char * setting_get_name (const Setting *st_setting)
 *
 * @brief  Get Setting name string
 *
 * @param[in] st_setting  Setting to get name
 * @return    Name string
 *
 * @fn  uint_fast32_t setting_get_hash (const Setting *st_setting)
 *
 * @brief  Get Setting hash value
 *
 * @param[in] st_setting  Setting to get hash
 * @return    Hash value
 */
/*----------------------------------------------------------------------------*/
static inline SetValType    setting_get_type (const Setting *st_setting) {
    return st_setting->v_type;
}
static inline const char  * setting_get_name (const Setting *st_setting) {
    return st_setting->s_name;
}
static inline uint_fast32_t setting_get_hash (const Setting *st_setting) {
    return st_setting->hash;
}
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_get_parent (Setting *st_setting)
 *
 * @brief  Get Setting's parent Setting
 *
 * @param[in] st_setting  Setting item
 * @return    Parent Setting
 *
 * @fn  Setting * setting_get_child (Setting *st_setting)
 *
 * @brief  Get Setting's child Setting
 *
 * @param[in] st_setting  Setting item
 * @return    Child Setting
 */
/*----------------------------------------------------------------------------*/
static inline Setting * setting_get_parent (Setting *st_setting) {
    return st_setting->parent;
}

Setting * setting_get_child  (Setting *st_setting);
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_next (Setting *st_setting)
 *
 * @brief  Get next Setting in list
 *
 * @param[in] st_setting  Setting item
 * @return    Next Setting in list or null
 *
 * @fn  Setting * setting_prev (Setting *st_setting)
 *
 * @brief  Get previous Setting in list
 *
 * @param[in] st_setting  Setting item
 * @return    Previous Setting in list or null
 */
/*----------------------------------------------------------------------------*/
static inline Setting * setting_next (Setting *st_setting) {
    return st_setting->next;
}
static inline Setting * setting_prev (Setting *st_setting) {
    return st_setting->prev;
}
/*----------------------------------------------------------------------------*/
/**
 * @fn  void setting_free (Setting *st_set)
 *
 * @brief  Free Setting data
 *
 * @param[in]  st_set  Setting object
 * @return     none
 *
 * @fn  void settings_free_all (Setting *st_set)
 *
 * @brief  Free all settings in given Setting
 *
 * @param[in]  st_set  Setting object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void setting_free      (Setting *st_set);

void settings_free_all (Setting *st_set);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int64_t setting_get_int (const Setting *st_set)
 *
 * @brief  Get integer value stored in Setting
 *
 * It is set to hold and return 64 bit integer value.
 *
 * @param[in]  st_set  Setting object
 * @return     64 bit integer or 0 if incorrect type to get
 *
 * @fn  uint64_t setting_get_uint (const Setting *st_set)
 *
 * @brief  Get unsigned integer value stored in Setting
 *
 * It is set to hold and return 64 bit unsigned integer value.
 *
 * @param[in]  st_set  Setting object
 * @return     64 bit unsigned integer or 0 if incorrect type to get
 *
 * @fn  double setting_get_double (const Setting *st_set)
 *
 * @brief  Get double type value stored in Setting
 *
 * @param[in]  st_set  Setting object
 * @return     Double value or 0 if incorrect type to get
 *
 * @fn  const char * setting_get_string (const Setting *st_set)
 *
 * @brief  Get text string stored in Setting
 *
 * @param[in]  st_set  Setting object
 * @return     String or null if incorrect type to get
 */
/*----------------------------------------------------------------------------*/
static inline int64_t      setting_get_int    (const Setting *st_set) {
    return setting_get_type (st_set) == SET_VAL_INT ? st_set->data.i_val : 0;
}
static inline uint64_t     setting_get_uint   (const Setting *st_set) {
    return setting_get_type (st_set) == SET_VAL_UINT ? st_set->data.ui_val : 0;
}
static inline double       setting_get_double (const Setting *st_set) {
    return setting_get_type (st_set) == SET_VAL_DOUBLE ? st_set->data.d_val : 0;
}
static inline const char * setting_get_string (const Setting *st_set) {
    return setting_get_type (st_set) == SET_VAL_STRING ? 
        (const char *) st_set->data.s_val : NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set setting to string type and its content to val.
 *
 * @param[in,out] st_set  Setting object
 * @param[in]     val     String value to set
 * @return        none
 */
void         setting_set_string (Setting       *st_set,
                                 const char    *val);
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_new_setting (const char *s_name)
 * @brief      Create new Setting object, set it to be a setting type and its
 *             name to s_name.
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_array (const char *s_name)
 * @brief      Create new Setting object, set it to be an array type and its
 *             name to s_name.
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_int (const char *s_name, const int64_t i_val)
 * @brief      Create new Setting object for an integer (64 bit), set its
 *             value to val and name to s_name.
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @param[in]  val     Value to store in setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint (const char *s_name, const uint64_t i_val)
 * @brief      Create new Setting object for an unsigned integer (64 bit),
 *             set its value to i_val and name to s_name.
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @param[in]  val     Value to store in setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_double (const char *s_name, const double d_val)
 * @brief      Create new Setting object for a double, set its value to val
 *             and name to s_name.
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @param[in]  val     Value to store in setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_string (const char *s_name, const char *val)
 * @brief      Create new Setting object for a string, set its value to val
 *             and name to s_name.
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @param[in]  val     Value to store in setting
 * @return     New Setting object
 */
/*----------------------------------------------------------------------------*/
Setting * setting_new_setting (const char *s_name)
          __attribute__ ((returns_nonnull));

Setting * setting_new_array   (const char *s_name)
          __attribute__ ((returns_nonnull));

Setting * setting_new_int     (const char *s_name,
                               int64_t     val)
          __attribute__ ((returns_nonnull));

Setting * setting_new_uint    (const char *s_name,
                               uint64_t    val)
          __attribute__ ((returns_nonnull));

Setting * setting_new_double  (const char *s_name,
                               double      val)
          __attribute__ ((returns_nonnull));

Setting * setting_new_string  (const char *s_name,
                               const char *val)
          __attribute__ ((returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicates a Setting
 *
 * @param[in]  st_src  Source Setting object
 * @return     New copy of Setting or null
 */
Setting * setting_copy (const Setting *st_src)
          __attribute__ ((returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_first (Setting *st_settings)
 *
 * @brief  Find and return first item in a Setting list.
 *
 * @param[in] st_settings  List of Setting items
 * @return    Pointer to first element on list or null if list is empty
 *
 * @fn  Setting * setting_last (Setting *st_settings)
 *
 * @brief  Find and return last item in a Setting list.
 *
 * @param[in] st_settings  List of Setting items
 * @return    Pointer to last element on list or null if list is empty
 *
 * @fn  Setting * setting_get_top_parent (Setting *st_setting)
 *
 * @brief  Get top parent Setting of given Setting item.
 *
 * @param[in] st_setting  Setting to get its top parent
 * @return    Pointer to top parent Setting item
 *
 * @fn  Setting * settings_find (Setting *st_settings, const char *s_name)
 *
 * @brief  Find Setting with given name in list starting at position of input
 *         Setting item.
 *
 * @param[in] st_settings  List of Setting items
 * @param[in] s_name       Name of Setting to be found
 * @return    Pointer to found Setting or null if not found
 *
 * @fn  Setting * setting_get_at_pos (Setting      *st_settings,
 *                                    const size_t  pos)
 *
 * @brief  Find and return Setting item at given posision in st_settings
 *         list.
 *
 * @param[in] st_settings  List of Setting items
 * @param[in] pos          Position of setting to get
 * @return    Pointer to Setting item or null if not found
 */
/*----------------------------------------------------------------------------*/
Setting * setting_first          (Setting       *st_settings);

Setting * setting_last           (Setting       *st_settings);

Setting * setting_get_top_parent (Setting       *st_setting);

Setting * settings_find          (Setting       *st_settings,
                                  const char    *s_name);

Setting * setting_get_at_pos     (Setting       *st_settings,
                                  const size_t   pos);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count number of Setting items present in given st_settings list.
 *
 * @param[in] st_settings  List of Setting items
 * @return    Number of Setting items in st_settings list
 */
size_t    settings_count         (const Setting *st_settings);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove Setting from list.
 *
 * @param[in] st_setting  Setting to remove
 * @return    none
 */
void      setting_remove         (Setting *st_setting);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace Setting st_old in list with st_new.
 *
 * @param[in,out] st_old  Old Setting item to be replaced
 * @param[in,out] st_new  New Setting item to replace old one
 * @return        none
 */
void      setting_replace        (Setting       *st_old,
                                  Setting       *st_new);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find setting in list and replace.
 *
 * Function is looking in Setting list st_list for settings with name like
 * name of st_setting Setting. After finding such setting it will be replaced
 * with st_seting. If i_multi is set to 0, fuction stops after first replace,
 * if i_multi is set to 1, funciton replaces all found settings. Function
 * returns number of replaces performed.
 *
 * @param[in,out] st_list     List of Setting items
 * @param[in,out] st_setting  Destination Setting
 * @param[in]     i_multi     Multiple replaces value
 * @return        Number of replaces performed.
 */
int       settings_find_replace  (Setting       *st_list,
                                  Setting       *st_setting,
                                  const int      i_multi);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void setting_add_child (Setting *st_parent,
 *                              Setting *st_child)
 *
 * @brief  Append Setting st_child to child list of Setting st_parent
 *
 * @param[in,out] st_parent  Parent Setting to add child Setting
 * @param[in,out] st_child   Child Setting to add to parent's list
 * @return        none
 *
 * @fn  void settings_prepend (Setting *st_list,
 *                             Setting *st_setting)
 *
 * @brief  Prepend st_setting item to st_list list.
 *
 * @param[in]  st_list     List of Setting items
 * @param[out] st_setting  Setting item to prepend
 * @return     none
 *
 * @fn  void settings_append (Setting *st_list,
 *                            Setting *st_setting)
 *
 * @brief  Append st_setting item to st_list list.
 *
 * @param[in]  st_list     List of Setting items
 * @param[out] st_setting  Setting item to append
 * @return     none
 *
 * @fn  void settings_append_or_ignore (Setting *st_list,
 *                                      Setting *st_setting)
 *
 * @brief  Append Setting item to st_list list or ignore append if
 *         setting with same name exists on it.
 *
 * @param[in]  st_list     List of Setting items
 * @param[out] st_setting  Setting item to append
 * @return     none
 *
 * @fn  void settings_append_or_replace (Setting *st_list,
 *                                       Setting *st_setting)
 *
 * @brief  Append Setting item to st_list list and replace if
 *         setting with same name exists on it.
 *
 * @param[in]  st_list     List of Setting items
 * @param[out] st_setting  Setting item to append
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void setting_add_child          (Setting *st_parent,
                                 Setting *st_child);

void settings_prepend           (Setting *st_list,
                                 Setting *st_setting);

void settings_append            (Setting *st_list,
                                 Setting *st_setting);

void settings_append_or_ignore  (Setting *st_list,
                                 Setting *st_setting);

void settings_append_or_replace (Setting *st_list,
                                 Setting *st_setting);
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_get_top_level (Setting *st_setting)
 *
 * @brief  Get top level Setting on list
 *
 * @param[in]  st_setting  Setting from list
 * @return     Top level setting in list
 *
 * @fn  size_t setting_count_children (Setting *st_setting)
 *
 * @brief  Count children in Setting object or array
 *
 * @param[in]  st_setting  Setting from list
 * @return     Number of items in List
 *
 * @fn  Setting * setting_find_child (Setting *st_setting, const char *s_name)
 *
 * @brief  Find child with given name in Setting object or array
 *
 * @param[in]  st_setting  Setting from list
 * @param[in]  s_name      Name of Setting to find
 * @return     Found Setting item or null of not found
 */
/*----------------------------------------------------------------------------*/
static inline Setting * setting_get_top_level  (Setting    *st_setting) {
    return setting_first (setting_get_top_parent (st_setting));
}
static inline size_t    setting_count_children (Setting    *st_setting) {
    return settings_count (setting_get_child (st_setting));
}
static inline Setting * setting_find_child     (Setting    *st_setting,
                                                const char *s_name) {
    return settings_find (setting_get_child (st_setting), s_name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings have equal names.
 *
 * @param[in]  st_set1  First setting
 * @param[in]  st_set2  Second setting
 * @return     Compare result. 1 if they are qual 0 if they are not
 */
static inline int settings_equal_names (Setting *st_set1,
                                        Setting *st_set2) {
    return st_set1->hash == st_set2->hash;
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print setting info
 *
 * @param[out]  st_set  Setting object
 * @return      none
 */
void setting_print  (Setting *st_set);

void settings_print (Setting *st_set);
/*----------------------------------------------------------------------------*/
#endif
#endif

