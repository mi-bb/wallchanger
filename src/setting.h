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
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting types 
 */
typedef enum
SetValType {
    SET_VAL_INT,    /**< Integer (64 bit) */
    SET_VAL_INT64,  /**< 64 bit integer */
    SET_VAL_INT32,  /**< 32 bit integer */
    SET_VAL_INT16,  /**< 16 bit integer */
    SET_VAL_INT8,   /**< 8 bit integer*/
    SET_VAL_UINT,   /**< Unsigned integer (64 bit) */
    SET_VAL_UINT64, /**< 64 bit unsigned integer */
    SET_VAL_UINT32, /**< 32 bit unsigned integer */
    SET_VAL_UINT16, /**< 16 bit unsigned integer */
    SET_VAL_UINT8,  /**< 8 bit unsigned integer */
    SET_VAL_DOUBLE, /**< double */
    SET_VAL_STRING, /**< string */
    SET_VAL_ARRAY   /**< array */
} SetValType;
/*----------------------------------------------------------------------------*/
typedef struct
SettingBase {
    SetValType     v_type;
    char          *s_name;
    uint_fast32_t  i_id;
    uint_fast32_t  i_owner_id;
} SettingBase;
/*----------------------------------------------------------------------------*/
/**
 * @struct Setting
 *
 * @brief  Setting structure 
 *
 * @var   Setting::v_type
 * @brief Type of setting
 *
 * @var   Setting::s_name
 * @brief Name of setting, it must be longer than 2 letters because of hash
 *        function (could make not unique results for smaller strings).
 *
 * @var   Setting::i_id
 * @brief Setting's id
 *
 * @var   Setting::i_owner_id
 * @brief Setting's owner id
 *
 * @var   Setting::data
 * @brief Struncture with setting value
 *
 * @union data_t
 * @brief Setting's data values
 *
 * @var   data_t::i_val
 * @brief Integer value
 * @var   data_t::i64_val
 * @brief 64 bit integer value
 * @var   data_t::i32_val
 * @brief 32 bit integer value
 * @var   data_t::i16_val
 * @brief 16 bit integer value
 * @var   data_t::i8_val
 * @brief 8 bit integer value
 * @var   data_t::ui_val
 * @brief Unsigned integer value
 * @var   data_t::ui64_val
 * @brief 64 bit unsigned integer value
 * @var   data_t::ui32_val
 * @brief 32 bit unsigned integer value
 * @var   data_t::ui16_val
 * @brief 16 bit unsigned integer value
 * @var   data_t::ui8_val
 * @brief 8 bit unsigned integer value
 * @var   data_t::d_val
 * @brief Double type number value
 * @var   data_t::s_val
 * @brief String value
 */
typedef struct 
Setting {
    SetValType     v_type;
    char          *s_name;
    uint_fast32_t  i_id;
    uint_fast32_t  i_owner_id;
    union data_t {
        int64_t   i_val;
        int64_t   i64_val;
        int32_t   i32_val;
        int16_t   i16_val;
        int8_t    i8_val;
        uint64_t  ui_val;
        uint64_t  ui64_val;
        uint32_t  ui32_val;
        uint16_t  ui16_val;
        uint8_t   ui8_val;
        double    d_val;
        char     *s_val;
    } data; 
} Setting;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free Setting object
 *
 * @param[in]  st_set  Setting object
 * @return     none
 */
void setting_free (Setting *st_set);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare 2 setting items.
 *
 * @param[in] st_sett1  Setting object
 * @param[in] st_sett2  Setting object
 * @return    0 if setting are equal, 1 if they differ
 */
int setting_compare (const Setting *st_sett1,
                     const Setting *st_sett2);
/*----------------------------------------------------------------------------*/
/**
 * @fn  const char * setting_get_name (const Setting *st_set)
 *
 * @brief  Get Setting object's name string
 *
 * @param[in]  st_set  Setting object
 * @return     Name string or null if wrong data passed to function
 *
 * @fn  SetValType setting_get_type (const Setting *st_set)
 *
 * @brief  Get the type of Setting
 *
 * @param[in]  st_set  Setting object
 * @return     Type of Setting
 *
 * @fn  uint32_t setting_get_id (const Setting *st_set)
 *
 * @brief  Get Setting id number
 *
 * @param[in]  st_set  Setting object
 * @return     Id number
 *
 * @fn  uint32_t setting_get_owner_id (const Setting *st_set)
 *
 * @brief  Get Setting object's owner id number
 *
 * @param[in]  st_set  Setting object
 * @return     Owner id number
 */
/*----------------------------------------------------------------------------*/
const char *  setting_get_name     (const Setting *st_set)
                                    __attribute__ ((pure));

SetValType    setting_get_type     (const Setting *st_set)
                                    __attribute__ ((pure));

uint_fast32_t setting_get_id       (const Setting *st_set)
                                    __attribute__ ((pure));

uint_fast32_t setting_get_owner_id (const Setting *st_set)
                                    __attribute__ ((pure));
/*----------------------------------------------------------------------------*/
/**
 * @fn         int64_t setting_get_int (const Setting *st_set)
 * @brief      Get integer value stored in Setting
 *
 * Function gets standard integer value, without type.
 * It is set to hold and return 64 bit integer value.
 *
 * @param[in]  st_set  Setting object
 * @return     64 bit integer or 0 if incorrect type to get
 *
 * @fn         int64_t setting_get_int64 (const Setting *st_set)
 * @brief      Get 64 bit integer value stored in Setting
 * @param[in]  st_set  Setting object
 * @return     64 bit integer or 0 if incorrect type to get
 *
 * @fn         int32_t setting_get_int32 (const Setting *st_set)
 * @brief      Get 32 bit integer value stored in Setting
 * @param[in]  st_set  Setting object
 * @return     32 bit integer or 0 if incorrect type to get
 *
 * @fn  int16_t setting_get_int16 (const Setting *st_set)
 * @brief      Get 16 bit integer value stored in Setting
 * @param[in]  st_set  Setting object
 * @return     16 bit integer or 0 if incorrect type to get
 *
 * @fn  int8_t setting_get_int8 (const Setting *st_set)
 * @brief      Get 8 bit integer value stored in Setting
 * @param[in]  st_set  Setting object
 * @return     8 bit integer or 0 if incorrect type to get
 *
 * @fn         uint64_t setting_get_uint (const Setting *st_set)
 * @brief      Get unsigned integer (64 bit) value stored in Setting
 * @param[in]  st_set  Setting object
 * @return     Unsigned integer (64 bit) or 0 if incorrect type to get
 *
 * @fn         uint64_t setting_get_uint64 (const Setting *st_set)
 * @brief      Get 64 bit unsigned integer value stored in Setting
 * @param[in]  st_set  Setting object
 * @return     64 bit unsigned integer or 0 if incorrect type to get
 *
 * @fn         uint32_t setting_get_uint32 (const Setting *st_set)
 * @brief      Get 32 bit unsigned integer value stored in Setting
 * @param[in]  st_set  Setting object
 * @return     32 bit unsigned integer or 0 if incorrect type to get
 *
 * @fn         uint16_t setting_get_uint16 (const Setting *st_set)
 * @brief      Get 16 bit unsigned integer value stored in Setting
 * @param[in]  st_set  Setting object
 * @return     16 bit unsigned integer or 0 if incorrect type to get
 *
 * @fn         uint8_t setting_get_uint8 (const Setting *st_set)
 * @brief      Get 8 bit unsigned integer value stored in Setting
 * @param[in]  st_set  Setting object
 * @return     8 bit unsigned integer or 0 if incorrect type to get
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
int64_t      setting_get_int    (const Setting *st_set)
                                 __attribute__ ((pure));

int64_t      setting_get_int64  (const Setting *st_set)
                                 __attribute__ ((pure));

int32_t      setting_get_int32  (const Setting *st_set)
                                 __attribute__ ((pure));

int16_t      setting_get_int16  (const Setting *st_set)
                                 __attribute__ ((pure));

int8_t       setting_get_int8   (const Setting *st_set)
                                 __attribute__ ((pure));

uint64_t     setting_get_uint   (const Setting *st_set)
                                 __attribute__ ((pure));

uint64_t     setting_get_uint64 (const Setting *st_set)
                                 __attribute__ ((pure));

uint32_t     setting_get_uint32 (const Setting *st_set)
                                 __attribute__ ((pure));

uint16_t     setting_get_uint16 (const Setting *st_set)
                                 __attribute__ ((pure));

uint8_t      setting_get_uint8  (const Setting *st_set)
                                 __attribute__ ((pure));

double       setting_get_double (const Setting *st_set)
                                 __attribute__ ((pure));

const char * setting_get_string (const Setting *st_set)
                                 __attribute__ ((pure));
/*----------------------------------------------------------------------------*/
/**
 * @fn         void setting_assign_to_array (Setting    *st_set,
 *                                           const char *s_name)
 * @brief      Assing Setting to an array with name s_name
 * @param[out] st_set  Setting object
 * @param[in]  s_name  Name of array
 * @return     none
 *
 * @fn         void setting_reset_array (Setting *st_set)
 * @brief      Remove setting from array, setting will stay a normal setting
 *             not assigned to any array
 * @param[out] st_set  Setting object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void setting_assign_to_array (Setting    *st_set,
                              const char *s_name);
//                              __attribute__ ((nonnull (2)));

void setting_reset_array     (Setting    *st_set);
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_new_int (const int64_t i_val, const char *s_name)
 * @brief      Create new Setting object for an integer (64 bit), set its
 *             value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint (const uint64_t i_val, const char *s_name)
 * @brief      Create new Setting object for an unsigned integer (64 bit),
 *             set its value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_int64 (const int64_t i_val, const char *s_name)
 * @brief      Create new Setting object for 64 bit integer, set its value to
 *             i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint64 (const uint64_t i_val, const char *s_name)
 * @brief      Create new Setting object for 64 bit unsigned integer, set its
 *             value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_int32 (const int32_t i_val, const char *s_name)
 * @brief      Create new Setting object for 32 bit integer, set its value to
 *             i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint32 (const uint32_t i_val, const char *s_name)
 * @brief      Create new Setting object for 32 bit unsigned integer, set its
 *             value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_int16 (const int16_t i_val, const char *s_name)
 * @brief      Create new Setting object for 16 bit integer, set its value to
 *             i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint16 (const uint16_t i_val, const char *s_name)
 * @brief      Create new Setting object for 16 bit unsigned integer, set its
 *             value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_int8 (const int8_t i_val, const char *s_name)
 * @brief      Create new Setting object for 8 bit integer, set its value to
 *             i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint8 (const uint8_t i_val, const char *s_name)
 * @brief      Create new Setting object for 8 bit unsigned integer, set its
 *             value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_double (const double d_val, const char *s_name)
 * @brief      Create new Setting object for a double, set its value to d_val
 *             and name to s_name.
 * @param[in]  d_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_string (const char *s_val, const char *s_name)
 * @brief      Create new Setting object for a string, set its value to s_val
 *             and name to s_name.
 * @param[in]  s_val  Value to store in setting
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_array (const char *s_name)
 * @brief      Create new Setting object, set it to be an array type and its
 *             name to s_name.
 * @param[in]  s_name  Name of the setting (should be longer than 2 letters)
 * @return     New Setting object
 */
/*----------------------------------------------------------------------------*/
Setting * setting_new_int    (const int64_t    i_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2)));

Setting * setting_new_uint   (const uint64_t   i_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2)));

Setting * setting_new_int64  (const int64_t    i_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2), returns_nonnull));

Setting * setting_new_uint64 (const uint64_t   i_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2), returns_nonnull));

Setting * setting_new_int32  (const int32_t    i_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2), returns_nonnull));

Setting * setting_new_uint32 (const uint32_t   i_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2), returns_nonnull));

Setting * setting_new_int16  (const int16_t    i_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2), returns_nonnull));

Setting * setting_new_uint16 (const uint16_t   i_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2), returns_nonnull));

Setting * setting_new_int8   (const int8_t     i_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2), returns_nonnull));

Setting * setting_new_uint8  (const uint8_t    i_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2), returns_nonnull));

Setting * setting_new_double (const double     d_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2), returns_nonnull));

Setting * setting_new_string (const char      *s_val,
                              const char      *s_name)
          __attribute__ ((returns_nonnull));
//          __attribute__ ((nonnull (2), returns_nonnull));

Setting * setting_new_array  (const char      *s_name)
          __attribute__ ((nonnull (1), returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicates a Setting
 *
 * @param[in]  st_src  Source Setting object
 * @return     New copy of Setting or null
 */
Setting * setting_copy (const Setting *st_src)
          __attribute__ ((returns_nonnull));
#ifdef DEBUG
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print setting info
 *
 * @param[out]  st_set  Setting object
 * @return      none
 */
void setting_print (const Setting *st_set);
/*----------------------------------------------------------------------------*/
#endif
#endif

