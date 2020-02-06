/**
 * @file  setting.c
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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hashfun.h"
#include "setting.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting initialization
 *
 * @param[out] st_set  Setting object
 * @return     none
 */
static void setting_init (Setting *st_set);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free string data in Setting
 *
 * @param[out] st_set  Setting object
 * @return     none
 */
static void setting_free_string (Setting *st_set);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Copy data from one Setting object to another
 *
 * @param[out] st_dest  Destination Setting object
 * @param[in]  st_src   Source Setting object
 * @return     none
 */
static void setting_copy2 (Setting       *st_dest,
                           const Setting *st_src);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create default Setting
 *
 * @param[in]  s_name  Setting name
 * @return     New Setting or null
 */
static Setting * setting_create_default (const char *s_name)
                 __attribute__ ((nonnull (1), returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @fn static void setting_set_type (Setting *st_set, const SetValType i_type)
 * @brief  Set the type of setting
 * @param[out] st_set  Setting object
 * @param[in]  i_type  Type of setting
 * @return     none
 *
 * @fn static void setting_set_id (Setting *st_set, const uint32_t i_val)
 * @brief  Set Setting id value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Id value
 * @return     none
 *
 * @fn static void setting_set_owner_id (Setting *st_set, const uint32_t i_val)
 * @brief  Set Setting owner id value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Owner id value
 * @return     none
 *
 * @fn static void setting_set_name (Setting *st_set, const char *s_str)
 * @brief  Set Setting name
 * @param[out] st_set  Setting object
 * @param[in]  s_str   String name to set
 * @return     none
 *
 * @fn static void setting_set_array (Setting *st_set)
 * @brief  Set Setting type as array
 * @param[out] st_set  Setting object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static void setting_set_type     (Setting             *st_set,
                                  const SetValType     i_type);

static void setting_set_id       (Setting             *st_set,
                                  const uint_fast32_t  i_val);

static void setting_set_owner_id (Setting             *st_set,
                                  const uint_fast32_t  i_val);

static void setting_set_name     (Setting             *st_set,
                                  const char          *s_str);

static void setting_set_array    (Setting             *st_set);
/*----------------------------------------------------------------------------*/
/**
 * @fn    static void setting_set_int (Setting *st_set, const int64_t i_val)
 * @brief Set Setting int data value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Value of int data
 * @return     none
 *
 * @fn    static void setting_set_int64 (Setting *st_set, const int64_t i_val)
 * @brief Set Setting 64 bit int data value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Value of 64 bit int data
 * @return     none
 *
 * @fn    static void setting_set_int32 (Setting *st_set, const int32_t i_val)
 * @brief Set Setting 32 bit int data value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Value of 32 bit int data
 * @return     none
 *
 * @fn  static void setting_set_int16 (Setting *st_set, const int16_t i_val)
 * @brief  Set Setting 16 bit int data value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Value of 16 bit int data
 * @return     none
 *
 * @fn    static void setting_set_int8 (Setting *st_set, const int8_t i_val)
 * @brief Set Setting 8 bit int data value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Value of 8 bit int data
 * @return     none
 *
 * @fn    static void setting_set_uint (Setting *st_set, const uint64_t i_val)
 * @brief Set Setting unsigned int data value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Value of unsigned int data
 * @return     none
 *
 * @fn    static void setting_set_uint64 (Setting *st_set, const uint64_t i_val)
 * @brief Set Setting 64 bit unsigned int data value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Value of 64 bit unsigned int data
 * @return     none
 *
 * @fn    static void setting_set_uint32 (Setting *st_set, const uint32_t i_val)
 * @brief Set Setting 32 bit unsigned int data value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Value of 32 bit unsigned int data
 * @return     none
 *
 * @fn    static void setting_set_uint16 (Setting *st_set, const uint16_t i_val)
 * @brief Set Setting 16 bit unsigned int data value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Value of 16 bit unsigned int data
 * @return     none
 *
 * @fn    static void setting_set_uint8 (Setting *st_set, const uint8_t i_val)
 * @brief Set Setting 8 bit unsigned int data value
 * @param[out] st_set  Setting object
 * @param[in]  i_val   Value of 8 bit unsigned int data
 * @return     none
 *
 * @fn    static void setting_set_double (Setting *st_set, const double d_val)
 * @brief Set Setting double data value
 * @param[out] st_set  Setting object
 * @param[in]  d_val   Double value
 * @return     none
 *
 * @fn    static void setting_set_string (Setting *st_set, const char *s_str)
 * @brief Set Setting string data value
 * @param[out] st_set  Setting object
 * @param[in]  s_str   String data to set
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static void setting_set_int    (Setting        *st_set,
                                const int64_t   i_val);

static void setting_set_int64  (Setting        *st_set,
                                const int64_t   i_val);

static void setting_set_int32  (Setting        *st_set,
                                const int32_t   i_val);

static void setting_set_int16  (Setting        *st_set,
                                const int16_t   i_val);

static void setting_set_int8   (Setting        *st_set,
                                const int8_t    i_val);

static void setting_set_uint   (Setting        *st_set,
                                const uint64_t  i_val);

static void setting_set_uint64 (Setting        *st_set,
                                const uint64_t  i_val);

static void setting_set_uint32 (Setting        *st_set,
                                const uint32_t  i_val);

static void setting_set_uint16 (Setting        *st_set,
                                const uint16_t  i_val);

static void setting_set_uint8  (Setting        *st_set,
                                const uint8_t   i_val);

static void setting_set_double (Setting        *st_set,
                                const double    d_val);

static void setting_set_string (Setting        *st_set,
                                const char     *s_str)
                                __attribute__ ((nonnull (2)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the type of setting
 */
static void
setting_set_type (Setting          *st_set,
                  const SetValType  i_type)
{
    st_set->v_type = i_type;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the type of setting
 */
SetValType
setting_get_type (const Setting *st_set)
{
    return st_set->v_type;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting id value
 */
static void
setting_set_id (Setting             *st_set,
                const uint_fast32_t  i_val)
{
    st_set->i_id = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief      Get Setting id number
 */
uint_fast32_t
setting_get_id (const Setting *st_set)
{
    return st_set->i_id;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting owner id value
 */
static void
setting_set_owner_id (Setting             *st_set,
                      const uint_fast32_t  i_val)
{
    st_set->i_owner_id = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get Setting object's owner id number
 */
uint_fast32_t
setting_get_owner_id (const Setting *st_set)
{
    return st_set->i_owner_id;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting name
 */
static void
setting_set_name (Setting    *st_set,
                  const char *s_str)
{
    st_set->s_name = strdup (s_str);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get Setting object's name string
 */
const char *
setting_get_name (const Setting *st_set)
{
    return (const char *) st_set->s_name;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting type as array
 */
static void
setting_set_array (Setting *st_set)
{
    setting_set_type (st_set, SET_VAL_ARRAY);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting initialization
 */
static void
setting_init (Setting *st_set)
{
    st_set->v_type       = SET_VAL_INT64;
    st_set->s_name       = NULL;
    st_set->i_id         = 0;
    st_set->i_owner_id   = 0;
    st_set->data.i64_val = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free string data in Setting
 */
static void
setting_free_string (Setting *st_set)
{
    free (st_set->data.s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free Setting object
 */
void
setting_free (Setting *st_set)
{
    free (st_set->s_name);

    if (setting_get_type (st_set) == SET_VAL_STRING) {
        setting_free_string (st_set);
    }
    free (st_set);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting int data value
 */
static void
setting_set_int (Setting       *st_set,
                 const int64_t  i_val)
{
    setting_set_type (st_set, SET_VAL_INT);

    st_set->data.i_val = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get integer value stored in Setting
 */
int64_t
setting_get_int (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_INT)
        return st_set->data.i_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting 64 bit int data value
 */
static void
setting_set_int64 (Setting       *st_set,
                   const int64_t  i_val)
{
    setting_set_type (st_set, SET_VAL_INT64);

    st_set->data.i64_val = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get 64 bit integer value stored in Setting
 */
int64_t
setting_get_int64 (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_INT64)
        return st_set->data.i64_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting 32 bit int data value
 */
static void
setting_set_int32 (Setting       *st_set,
                   const int32_t  i_val)
{
    setting_set_type (st_set, SET_VAL_INT32);

    st_set->data.i32_val = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get 32 bit integer value stored in Setting
 */
int32_t
setting_get_int32 (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_INT32)
        return st_set->data.i32_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting 16 bit int data value
 */
static void
setting_set_int16 (Setting       *st_set,
                   const int16_t  i_val)
{
    setting_set_type (st_set, SET_VAL_INT16);

    st_set->data.i16_val = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get 16 bit integer value stored in Setting
 */
int16_t
setting_get_int16 (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_INT16)
        return st_set->data.i16_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting 8 bit int data value
 */
static void
setting_set_int8 (Setting      *st_set,
                  const int8_t  i_val)
{
    setting_set_type (st_set, SET_VAL_INT8);

    st_set->data.i8_val = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get 8 bit integer value stored in Setting
 */
int8_t
setting_get_int8 (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_INT8)
        return st_set->data.i8_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting unsigned int data value
 */
static void
setting_set_uint (Setting        *st_set,
                  const uint64_t  i_val)
{
    setting_set_type (st_set, SET_VAL_UINT);

    st_set->data.ui_val = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get unsigned integer (64 bit) value stored in Setting
 */
uint64_t
setting_get_uint (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_UINT)
        return st_set->data.ui_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting 64 bit unsigned int data value
 */
static void
setting_set_uint64 (Setting        *st_set,
                    const uint64_t  i_val)
{
    setting_set_type (st_set, SET_VAL_UINT64);

    st_set->data.ui64_val = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get 64 bit unsigned integer value stored in Setting
 */
uint64_t
setting_get_uint64 (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_UINT64)
        return st_set->data.ui64_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting 32 bit unsigned int data value
 */
static void
setting_set_uint32 (Setting        *st_set,
                    const uint32_t  i_val)
{
    setting_set_type (st_set, SET_VAL_UINT32);

    st_set->data.ui32_val = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get 32 bit unsigned integer value stored in Setting
 */
uint32_t
setting_get_uint32 (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_UINT32)
        return st_set->data.ui32_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting 16 bit unsigned int data value
 */
static void
setting_set_uint16 (Setting        *st_set,
                    const uint16_t  i_val)
{
    setting_set_type (st_set, SET_VAL_UINT16);

    st_set->data.ui16_val = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get 16 bit unsigned integer value stored in Setting
 */
uint16_t
setting_get_uint16 (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_UINT16)
        return st_set->data.ui16_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting 8 bit unsigned int data value
 */
static void
setting_set_uint8 (Setting       *st_set,
                   const uint8_t  i_val)
{
    setting_set_type (st_set, SET_VAL_UINT8);

    st_set->data.ui8_val = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get 8 bit unsigned integer value stored in Setting
 */
uint8_t
setting_get_uint8 (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_UINT8)
        return st_set->data.ui8_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting double data value
 *
 * @param[out] st_set  Setting object
 * @param[in]  d_val   Double value
 * @return     none
 */
static void
setting_set_double (Setting      *st_set,
                    const double  d_val)
{
    setting_set_type (st_set, SET_VAL_DOUBLE);

    st_set->data.d_val = d_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get double type value stored in Setting
 */
double
setting_get_double (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_DOUBLE)
        return st_set->data.d_val;
    else
        return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting string data value
 */
static void
setting_set_string (Setting    *st_set,
                    const char *s_str)
{
    setting_set_type (st_set, SET_VAL_STRING);

    st_set->data.s_val = strdup (s_str);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get text string stored in Setting
 */
const char *
setting_get_string (const Setting *st_set)
{
    if (setting_get_type (st_set) == SET_VAL_STRING)
        return (const char *) st_set->data.s_val;
    else
        return NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Assing Setting to an array with name s_name
 */
void
setting_assign_to_array (Setting    *st_set,
                         const char *s_name)
{
    if (strcmp (s_name, "") == 0)
        setting_set_owner_id (st_set, 0);
    else
        setting_set_owner_id (st_set, hash (s_name));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove setting from array, setting will stay a normal setting
 */
void
setting_reset_array (Setting *st_set)
{
    setting_set_owner_id (st_set, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Copy data from one Setting object to another
 */
static void
setting_copy2 (Setting       *st_dest,
               const Setting *st_src)
{
    setting_init (st_dest);
    st_dest->s_name     = strdup (st_src->s_name);
    st_dest->i_id       = st_src->i_id;
    st_dest->i_owner_id = st_src->i_owner_id;
    st_dest->v_type     = st_src->v_type;

    switch (st_src->v_type) {

        case SET_VAL_INT:
            st_dest->data.i_val = st_src->data.i_val;
            break;

        case SET_VAL_UINT:
            st_dest->data.ui_val = st_src->data.ui_val;
            break;

        case SET_VAL_INT64:
            st_dest->data.i64_val = st_src->data.i64_val;
            break;

        case SET_VAL_UINT64:
            st_dest->data.ui64_val = st_src->data.ui64_val;
            break;

        case SET_VAL_INT32:
            st_dest->data.i32_val = st_src->data.i32_val;
            break;

        case SET_VAL_UINT32:
            st_dest->data.ui32_val = st_src->data.ui32_val;
            break;

        case SET_VAL_INT16:
            st_dest->data.i16_val = st_src->data.i16_val;
            break;

        case SET_VAL_UINT16:
            st_dest->data.ui16_val = st_src->data.ui16_val;
            break;

        case SET_VAL_INT8:
            st_dest->data.i8_val = st_src->data.i8_val;
            break;

        case SET_VAL_UINT8:
            st_dest->data.ui8_val = st_src->data.ui8_val;
            break;

        case SET_VAL_DOUBLE:
            st_dest->data.d_val = st_src->data.d_val;
            break;

        case SET_VAL_STRING:
            st_dest->data.s_val = strdup (st_src->data.s_val);
            break;

        case SET_VAL_ARRAY:
            break;

        default:
            break;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicates a Setting
 */
Setting *
setting_copy (const Setting *st_src)
{
    Setting *st_ret;

    st_ret = malloc (sizeof (Setting));

    if (st_ret == NULL) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }

    setting_init (st_ret);
    setting_copy2 (st_ret, st_src);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create default Setting
 */
static Setting *
setting_create_default (const char *s_name)
{
    Setting *st_ret;

    st_ret = malloc (sizeof (Setting));

    if (st_ret == NULL) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }

    setting_init (st_ret);
    setting_set_name (st_ret, s_name);
    setting_set_id (st_ret, hash (s_name));

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_int (const int64_t  i_val,
                 const char    *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_int (st_ret, i_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_uint (const uint64_t  i_val,
                  const char     *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_uint (st_ret, i_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_int64 (const int64_t  i_val,
                   const char    *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_int64 (st_ret, i_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_uint64 (const uint64_t  i_val,
                    const char     *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_uint64 (st_ret, i_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_int32 (const int32_t  i_val,
                   const char    *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_int32 (st_ret, i_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_uint32 (const uint32_t  i_val,
                    const char     *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_uint32 (st_ret, i_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_int16 (const int16_t  i_val,
                   const char    *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_int16 (st_ret, i_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_uint16 (const uint16_t  i_val,
                    const char     *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_uint16 (st_ret, i_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_int8 (const int8_t  i_val,
                  const char   *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_int8 (st_ret, i_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_uint8 (const uint8_t  i_val,
                   const char    *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_uint8 (st_ret, i_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_double (const double  d_val,
                    const char   *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_double (st_ret, d_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_string (const char *s_val,
                    const char *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    if (s_val == NULL)
        setting_set_string (st_ret, "");
    else
        setting_set_string (st_ret, s_val);

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_array (const char *s_name)
{
    Setting *st_ret = setting_create_default (s_name);

    setting_set_array (st_ret);

    return st_ret;
}
#ifdef DEBUG
/*----------------------------------------------------------------------------*/
/*
 * @brief  Print setting info
 */
void
setting_print (const Setting *st_set)
{
    switch (st_set->v_type) {
        case SET_VAL_INT:
            printf ("int, val=%ld, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_int (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_UINT:
            printf ("uint, val=%ld, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_uint (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_INT64:
            printf ("int64, val=%ld, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_int64 (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_UINT64:
            printf ("uint64, val=%ld, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_uint64 (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_INT32:
            printf ("int32, val=%d, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_int32 (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_UINT32:
            printf ("uint32, val=%d, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_uint32 (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_INT16:
            printf ("int16, val=%d, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_int16 (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_UINT16:
            printf ("uint16, val=%d, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_uint16 (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_INT8:
            printf ("int8, val=%d, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_int8 (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_UINT8:
            printf ("uint8, val=%d, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_uint8 (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_DOUBLE:
            printf ("double, val=%f, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_double (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_STRING:
            printf ("str, val=%s, n=%s, oid=%" PRIdFAST32 "\n",
                    setting_get_string (st_set),
                    setting_get_name (st_set),
                    setting_get_owner_id (st_set));
            break;
        case SET_VAL_ARRAY:
            printf ("array, n=%s, id=%" PRIdFAST32 ", oid=%" PRIdFAST32 "\n",
                    setting_get_name (st_set),
                    setting_get_id (st_set),
                    setting_get_owner_id (st_set));
            break;
        default:
            break;
    }
}
/*----------------------------------------------------------------------------*/
#endif


