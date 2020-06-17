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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef SETTING_H
#define SETTING_H

#include <inttypes.h>
#include <stddef.h>
/*----------------------------------------------------------------------------*/
/**
 * @def    setting_get_top_level(setting)
 * @brief  Get top level Setting on list
 *
 * @def    setting_count_children(setting)
 * @brief  Count children in Setting object or array
 *
 * @def    setting_find_child(setting)
 * @brief  Find child with given name in Setting object or array
 */
/*----------------------------------------------------------------------------*/
#define setting_get_top_level(setting) \
    (setting_first (setting_get_top_parent (setting)))

#define setting_count_children(setting) \
    (settings_count (setting_get_child (setting)))

#define setting_find_child(setting, name) \
    (settings_find (setting_get_child (setting), name))
/*----------------------------------------------------------------------------*/
/**
 * @def    setting_get_type(setting)
 * @brief  Get Setting type value
 *
 * @def    setting_get_name(setting)
 * @brief  Get Setting name string
 *
 * @def    setting_get_hash(setting)
 * @brief  Get Setting hash value
 */
/*----------------------------------------------------------------------------*/
#define setting_get_type(setting)     (setting->v_type)

#define setting_get_name(setting)     (setting->s_name)

#define setting_get_hash(setting)     (setting->hash)
/*----------------------------------------------------------------------------*/
/**
 * @def    setting_get_parent(setting)
 * @brief  Get Setting parent Setting
 *
 * @def    setting_get_child(setting)
 * @brief  Get Setting child Setting
 */
/*----------------------------------------------------------------------------*/
#define setting_get_parent(setting)   (setting->parent)

#define setting_get_child(setting)    (setting->data.st_child)
/*----------------------------------------------------------------------------*/
/**
 * @def    setting_next(setting)
 * @brief  Get next Setting in list
 *
 * @def    setting_prev(setting)
 * @brief  Get previous Setting in list
 */
/*----------------------------------------------------------------------------*/
#define setting_next(setting)         (setting->next)

#define setting_prev(setting)         (setting->prev)
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
 * @brief  Compare 2 setting items.
 *
 * @param[in] st_sett1  Setting object
 * @param[in] st_sett2  Setting object
 * @return    0 if setting are equal, non 0 if they differ
 */
int setting_compare (const Setting *st_sett1,
                     const Setting *st_sett2) __attribute__ ((pure));
/*----------------------------------------------------------------------------*/
/**
 * @fn         int64_t setting_get_int (const Setting *st_set)
 * @brief      Get integer value stored in Setting
 *
 * It is set to hold and return 64 bit integer value.
 *
 * @param[in]  st_set  Setting object
 * @return     64 bit integer or 0 if incorrect type to get
 *
 * @fn         uint64_t setting_get_uint (const Setting *st_set)
 * @brief      Get unsigned integer value stored in Setting
 *
 * It is set to hold and return 64 bit unsigned integer value.
 *
 * @param[in]  st_set  Setting object
 * @return     64 bit unsigned integer or 0 if incorrect type to get
 *
 * @fn         double setting_get_double (const Setting *st_set)
 * @brief      Get double type value stored in Setting
 * @param[in]  st_set  Setting object
 * @return     Double value or 0 if incorrect type to get
 *
 * @fn         const char * setting_get_string (const Setting *st_set)
 * @brief      Get text string stored in Setting
 * @param[in]  st_set  Setting object
 * @return     String or null if incorrect type to get
 */
/*----------------------------------------------------------------------------*/
int64_t      setting_get_int    (const Setting *st_set) __attribute__ ((pure));

uint64_t     setting_get_uint   (const Setting *st_set) __attribute__ ((pure));

double       setting_get_double (const Setting *st_set) __attribute__ ((pure));

const char * setting_get_string (const Setting *st_set) __attribute__ ((pure));
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
 * @brief  Finds and return first item in a Setting list.
 *
 * @param[in] st_settings  List of Setting items
 * @return    Pointer to first element on list or null if list is empty
 *
 * @fn  Setting * setting_last (Setting *st_settings)
 *
 * @brief  Finds and return last item in a Setting list.
 *
 * @param[in] st_settings  List of Setting items
 * @return    Pointer to last element on list or null if list is empty
 *
 * @fn  Setting * setting_get_top_parent (Setting *st_setting)
 *
 * @brief  Get top parent Setting of given Setting item.
 *
 * @param[in] st_setting  Setting to get it's top parent
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
 *
 * @fn  size_t settings_count (const Setting *st_settings)
 *
 * @brief  Count number of Setting items present in given st_settings list.
 *
 * @param[in] st_settings  List of Setting items
 * @return    Number of Setting items in st_settings list
 */
/*----------------------------------------------------------------------------*/
Setting * setting_first          (Setting       *st_settings);

Setting * setting_last           (Setting       *st_settings);

Setting * setting_get_top_parent (Setting       *st_setting);

Setting * settings_find          (Setting       *st_settings,
                                  const char    *s_name);

Setting * setting_get_at_pos     (Setting       *st_settings,
                                  const size_t   pos);

size_t    settings_count         (const Setting *st_settings);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void setting_replace (Setting *st_old,
 *                            Setting *st_new)
 *
 * @brief  Replace Setting st_old in list with st_new.
 *
 * @param[in,out] st_old  Old Setting item to be replaced
 * @param[in,out] st_new  New Setting item to replace old one
 * @return        none
 *
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
void setting_replace            (Setting *st_old,
                                 Setting *st_new);

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
#ifdef DEBUG
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print setting info
 *
 * @param[out]  st_set  Setting object
 * @return      none
 */
void setting_print (const Setting *st_set);

void settings_print (const Setting *st_set);
/*----------------------------------------------------------------------------*/
#endif
#endif

