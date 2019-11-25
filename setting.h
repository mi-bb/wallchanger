/**
 * @file  setting.h
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
/**
 * @brief  Setting structure 
 */
typedef struct 
Setting {
    SetValType v_type;     /**< Type of setting */
    char *s_name;          /**< Setting name */
    uint64_t i_id;         /**< Setting id */
    uint64_t i_owner_id;   /**< Setting owner id */
    union {
        int64_t  i_val;    /**< Value for integer (64 bit) */
        int64_t  i64_val;  /**< Value for 64 bit integer */
        int32_t  i32_val;  /**< Value for 32 bit integer */
        int16_t  i16_val;  /**< Value for 16 bit integer */
        int8_t   i8_val;   /**< Value for 8 bit integer */
        uint64_t ui_val;   /**< Value for unsigned integer (64 bit) */
        uint64_t ui64_val; /**< Value for 64 bit unsigned integer */
        uint32_t ui32_val; /**< Value for 32 bit unsigned integer */
        uint16_t ui16_val; /**< Value for 16 bit unsigned integer */
        uint8_t  ui8_val;  /**< Value for 8 bit unsigned integer */
        double   d_val;    /**< Value for double */
        char    *s_val;    /**< Value for string */
    } data; 
} Setting;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Calls hash function
 *
 * @param[in]  s_str  String to count hash
 * @return     Calculated hash value
 */
uint64_t setting_hashfun (const char *s_str);
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
 * @fn  const char * setting_get_name (Setting *st_set)
 * @brief      Get Setting object's name string
 * @param[in]  st_set  Setting object
 * @return     Name string or null if wrong data passed to function
 *
 * @fn  SetValType setting_get_type (Setting *st_set)
 * @brief      Get type of Setting object value
 * @param[in]  st_set  Setting object
 * @return     Type of Setting value
 *
 * @fn  uint64_t setting_get_id (Setting *st_set)
 * @brief      Get Setting object's id number
 * @param[in]  st_set  Setting object
 * @return     Id number
 *
 * @fn  uint64_t setting_get_owner_id (Setting *st_set)
 * @brief      Get Setting object's owner id number
 * @param[in]  st_set  Setting object
 * @return     Owner id number
 */
/*----------------------------------------------------------------------------*/
const char * setting_get_name     (Setting *st_set);
/*----------------------------------------------------------------------------*/
SetValType   setting_get_type     (Setting *st_set);
/*----------------------------------------------------------------------------*/
uint64_t     setting_get_id       (Setting *st_set);
/*----------------------------------------------------------------------------*/
uint64_t     setting_get_owner_id (Setting *st_set);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int64_t setting_get_int (Setting *st_set)
 * @brief      Get integer value stored in Setting object
 *
 * Function gets standard integer value, without type.
 * It is set to hold and return 64 bit integer value.
 *
 * @param[in]  st_set  Setting object
 * @return     64 bit integer or 0 if incorrect type to get
 *
 * @fn  int64_t setting_get_int64 (Setting *st_set)
 * @brief      Get 64 bit integer value stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     64 bit integer or 0 if incorrect type to get
 *
 * @fn  int32_t setting_get_int32 (Setting *st_set)
 * @brief      Get 32 bit integer value stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     32 bit integer or 0 if incorrect type to get
 *
 * @fn  int16_t setting_get_int16 (Setting *st_set)
 * @brief      Get 16 bit integer value stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     16 bit integer or 0 if incorrect type to get
 *
 * @fn  int8_t setting_get_int8 (Setting *st_set)
 * @brief      Get 8 bit integer value stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     8 bit integer or 0 if incorrect type to get
 *
 * @fn  uint64_t setting_get_uint (Setting *st_set)
 * @brief      Get unsigned integer (64 bit) value stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     Unsigned integer (64 bit) or 0 if incorrect type to get
 *
 * @fn  uint64_t setting_get_uint64 (Setting *st_set)
 * @brief      Get 64 bit unsigned integer value stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     64 bit unsigned integer or 0 if incorrect type to get
 *
 * @fn  uint32_t setting_get_uint32 (Setting *st_set)
 * @brief      Get 32 bit unsigned integer value stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     32 bit unsigned integer or 0 if incorrect type to get
 *
 * @fn  uint16_t setting_get_uint16 (Setting *st_set)
 * @brief      Get 16 bit unsigned integer value stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     16 bit unsigned integer or 0 if incorrect type to get
 *
 * @fn  uint8_t setting_get_uint8 (Setting *st_set)
 * @brief      Get 8 bit unsigned integer value stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     8 bit unsigned integer or 0 if incorrect type to get
 *
 * @fn  double setting_get_double (Setting *st_set)
 * @brief      Get double type value stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     Double value or 0 if incorrect type to get
 *
 * @fn  const char * setting_get_string (Setting *st_set)
 * @brief      Get text string stored in Setting object
 * @param[in]  st_set  Setting object
 * @return     String or null if incorrect type to get
 */
/*----------------------------------------------------------------------------*/
int64_t      setting_get_int    (Setting *st_set);
/*----------------------------------------------------------------------------*/
int64_t      setting_get_int64  (Setting *st_set);
/*----------------------------------------------------------------------------*/
int32_t      setting_get_int32  (Setting *st_set);
/*----------------------------------------------------------------------------*/
int16_t      setting_get_int16  (Setting *st_set);
/*----------------------------------------------------------------------------*/
int8_t       setting_get_int8   (Setting *st_set);
/*----------------------------------------------------------------------------*/
uint64_t     setting_get_uint   (Setting *st_set);
/*----------------------------------------------------------------------------*/
uint64_t     setting_get_uint64 (Setting *st_set);
/*----------------------------------------------------------------------------*/
uint32_t     setting_get_uint32 (Setting *st_set);
/*----------------------------------------------------------------------------*/
uint16_t     setting_get_uint16 (Setting *st_set);
/*----------------------------------------------------------------------------*/
uint8_t      setting_get_uint8  (Setting *st_set);
/*----------------------------------------------------------------------------*/
double       setting_get_double (Setting *st_set);
/*----------------------------------------------------------------------------*/
const char * setting_get_string (Setting *st_set);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void setting_assign_to_array (Setting *st_set, const char *s_name)
 * @brief       Assing Setting to an array with name s_name
 * @param[out]  st_set  Setting object
 * @param[in]   s_name  Name of array
 * @return      none
 *
 * @fn  void setting_remove_from_array (Setting *st_set)
 * @brief       Remove setting from array, setting will stay a normal setting
 *              not assigned to any array
 * @param[out]  st_set  Setting object
 * @return      none
 */
/*----------------------------------------------------------------------------*/
void setting_assign_to_array   (Setting    *st_set,
                                const char *s_name);
/*----------------------------------------------------------------------------*/
void setting_remove_from_array (Setting    *st_set);
/*----------------------------------------------------------------------------*/
/**
 * @fn  Setting * setting_new_int (const int64_t i_val, const char *s_name)
 * @brief      Create new Setting object for an integer (64 bit), set its
 *             value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint (const uint64_t i_val, const char *s_name)
 * @brief      Create new Setting object for an unsigned integer (64 bit),
 *             set its value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
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
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_int32 (const int32_t i_val, const char *s_name)
 * @brief      Create new Setting object for 32 bit integer, set its value to
 *             i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint32 (const uint32_t i_val, const char *s_name)
 * @brief      Create new Setting object for 32 bit unsigned integer, set its
 *             value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_int16 (const int16_t i_val, const char *s_name)
 * @brief      Create new Setting object for 16 bit integer, set its value to
 *             i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint16 (const uint16_t i_val, const char *s_name)
 * @brief      Create new Setting object for 16 bit unsigned integer, set its
 *             value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_int8 (const int8_t i_val, const char *s_name)
 * @brief      Create new Setting object for 8 bit integer, set its value to
 *             i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_uint8 (const uint8_t i_val, const char *s_name)
 * @brief      Create new Setting object for 8 bit unsigned integer, set its
 *             value to i_val and name to s_name.
 * @param[in]  i_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_double (const double d_val, const char *s_name)
 * @brief      Create new Setting object for a double, set its value to d_val
 *             and name to s_name.
 * @param[in]  d_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_string (const char *s_val, const char *s_name)
 * @brief      Create new Setting object for a string, set its value to s_val
 *             and name to s_name.
 * @param[in]  s_val  Value to store in setting
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 *
 * @fn  Setting * setting_new_array (const char *s_name)
 * @brief      Create new Setting object, set it to be an array type and its
 *             name to s_name.
 * @param[in]  s_name  Name of the setting
 * @return     New Setting object
 */
/*----------------------------------------------------------------------------*/
Setting * setting_new_int    (const int64_t    i_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_uint   (const uint64_t   i_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_int64  (const int64_t    i_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_uint64 (const uint64_t   i_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_int32  (const int32_t    i_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_uint32 (const uint32_t   i_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_int16  (const int16_t    i_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_uint16 (const uint16_t   i_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_int8   (const int8_t     i_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_uint8  (const uint8_t    i_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_double (const double     d_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_string (const char      *s_val,
                              const char      *s_name);
/*----------------------------------------------------------------------------*/
Setting * setting_new_array  (const char      *s_name);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get a copy of setting
 *
 * @param[out]  st_src  Setting object to copy
 * @return      Duplicated setting or null if could not allocate or wrong
 *              data passed
 */
Setting * setting_copy (Setting *st_src);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print setting info
 *
 * @param[out]  st_set  Setting object
 * @return      none
 */
void setting_print (Setting *st_set);
/*----------------------------------------------------------------------------*/
#endif

