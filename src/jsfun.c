/**
 * @file  jsfun.c
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
 * @brief  Json functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../config.h"

#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif

#include "jsof.h"
#include "errs.h"
#include "setting.h"
#include "rwdt.h"
#include "strfun.h"
#include "hashfun.h"
#include "jsfun.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert json object to Setting item.
 *
 * @param[in] val     Json object to convert
 * @param[in] s_name  Name of Setting item
 * @retrn     New setting item
 */
static Setting * js_json_obj_to_setting (json_object *val,
                                         const char  *s_name);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get items from Setting array or object and save them in Json array 
 *         or object item.
 *
 * @param[in]  st_setting  Setting item
 * @return     New json object
 */
static json_object * js_settings_array_to_json (const Setting *st_setting);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Setting object to Json object.
 *
 * @param[in] st_sett   Setting item to convert
 * @return    Json object
 */
static json_object * js_setting_to_json_obj  (const Setting *st_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Setting items from SettList and put them in Json object.
 *
 * @param[in]  st_settings  List of Setting items
 * @param[out] j_obj        Json object to insert data
 * @return     none
 */
static void js_settings_add_to_json_obj (const Setting *st_settings,
                                         json_object   *j_obj);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert json object to Setting item.
 */
static Setting *
js_json_obj_to_setting (json_object *val,
                        const char  *s_name)
{
    json_object *j_idx;         /* Json object read from Json array */
    Setting     *st_set = nullptr; /* Setting item to return */
    size_t       ui_cnt = 0;    /* Length of Json array */
    size_t            i = 0;    /* i */

    int i_val_type = json_object_get_type (val);

    switch (i_val_type) {

        case json_type_null:
            break;
        case json_type_boolean:
            st_set = setting_new_int (s_name,
                                      (int64_t) json_object_get_int64 (val));
            break;
        case json_type_double:
            st_set = setting_new_double (s_name, json_object_get_double (val));
            break;
        case json_type_int:
            st_set = setting_new_int (s_name,
                                      (int64_t) json_object_get_int64 (val));
            break;
        case json_type_string:
            st_set = setting_new_string (s_name, json_object_get_string (val));
            break;
        case json_type_object:
            st_set = setting_new_setting (s_name);
            json_object_object_foreach (val, key, val1) {
                setting_add_child (st_set, js_json_obj_to_setting (val1, key));
            }
            break;
        case json_type_array:
            st_set = setting_new_array (s_name);
            ui_cnt = json_object_array_length (val);
            for (i = 0; i < ui_cnt; ++i) {
                if ((j_idx = json_object_array_get_idx (val, i)) != nullptr) {
                    setting_add_child (st_set, 
                                       js_json_obj_to_setting (j_idx, nullptr));
                }
            }
            break;
        default:
            break;
    }
    return st_set;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get items from Setting array or object and save them in Json array 
 *         or object item.
 */
static json_object *
js_settings_array_to_json (const Setting *st_setting)
{
    json_object *j_array;  /* Json array to return */
    json_object *j_obj;    /* Json object made from Setting */
    Setting     *st_item;  /* For setting iteration */

    j_array = (setting_get_type (st_setting) == SET_VAL_SETTING) ?
              json_object_new_object() :
              json_object_new_array();

    st_item = setting_get_child (st_setting);

    while (st_item != nullptr) {

        j_obj = js_setting_to_json_obj (st_item);

        if (setting_get_type (st_setting) == SET_VAL_SETTING) { /* Json obj */
            json_object_object_add (j_array, setting_get_name (st_item), j_obj);
        }
        else { /* Json array */
            json_object_array_add (j_array, j_obj);
        }
        st_item = st_item->next;
    }
    return j_array;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Setting object to Json object.
 */
static json_object *
js_setting_to_json_obj (const Setting *st_sett)
{
    json_object *j_obj;
    SetValType   i_type = setting_get_type (st_sett);

    switch (i_type) {

        case SET_VAL_DOUBLE:
            j_obj = json_object_new_double (setting_get_double (st_sett));
            return j_obj;

        case SET_VAL_INT:
            j_obj = json_object_new_int64 ((int64_t) setting_get_int (st_sett));
            return j_obj;

        case SET_VAL_UINT:
            j_obj = json_object_new_int64 ((int64_t) setting_get_uint (st_sett));
            return j_obj;

        case SET_VAL_STRING:
            j_obj = json_object_new_string (setting_get_string (st_sett));
            return j_obj;

        case SET_VAL_SETTING:
            j_obj = js_settings_array_to_json (st_sett);
            return j_obj;

        case SET_VAL_ARRAY:
            j_obj = js_settings_array_to_json (st_sett);
            return j_obj;

        case SET_VAL_NULL:
            break;

        default:
            break;
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Setting items from SettList and put them in Json object.
 */
static void
js_settings_add_to_json_obj (const Setting *st_settings,
                             json_object   *j_obj)
{
    json_object *j_val;   /* Json object made from Setting */

    while (st_settings != nullptr) {
        j_val = js_setting_to_json_obj (st_settings);
        json_object_object_add (j_obj, setting_get_name (st_settings), j_val);
        st_settings = st_settings->next;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get settings data from file.
 */
Setting *
js_settings_read (const char *s_fname,
                  int        *i_err)
{
    Setting     *st_settings = nullptr; /* Settings to return */
    json_object *j_obj;              /* Json object made from file data */

    *i_err = ERR_OK;
    j_obj  = js_open_file (s_fname, nullptr, i_err);

    if (*i_err == ERR_OK && j_obj != nullptr) {
        json_object_object_foreach (j_obj, key, val) {
            st_settings = settings_append (st_settings,
                        js_json_obj_to_setting (val, key));
        }
#ifdef DEBUG
        printf ("-> Settings read start\n");
        settings_print (st_settings);
        printf ("-> Settings read end\n");
#endif
    }
    if (j_obj != nullptr)
        json_object_put (j_obj);

    return st_settings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings in list are an update to settings stored in
 *         settings file.
 */
char *
js_settings_check_for_update (const Setting *st_settings,
                              const char    *s_fname,
                              int           *i_err)
{
    json_object   *j_obj;             /* Json object made from file data */
    const char    *s_jbuff    = nullptr; /* Json object as string */
    char          *s_res_buff = nullptr; /* Result data buffer */
    uint_fast32_t  ui_hash    = 0;    /* Data read hash */

    *i_err = ERR_OK;

    if ((j_obj = js_open_file (s_fname, &ui_hash, i_err)) == nullptr)
        return nullptr;

    js_settings_add_to_json_obj (st_settings, j_obj);

    s_jbuff = json_object_to_json_string (j_obj);

    /* Compare saved file buffer hash and new one,
     * if they are different update output buffer */
    s_res_buff = (hash (s_jbuff) != ui_hash) ? strdup (s_jbuff) : nullptr;

    json_object_put (j_obj);

    return s_res_buff;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update file with new data.
 */
int
js_settings_update_file (const char *s_buff,
                         const char *s_fname)
{
    return save_file_data (s_fname, s_buff);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are an update to saved ones and update file with
 *         new data if they are.
 */
int
js_settings_check_update_file (const Setting *st_settings,
                               const char    *s_fname)
{
    int   i_err  = ERR_OK; /* Possible error to return */
    char *s_buff = nullptr;   /* Result of update check, if it is not null there
                              is a change in configuration and returned buffer
                              is the new data to save */
    s_buff = js_settings_check_for_update (st_settings, s_fname, &i_err);
    if (s_buff != nullptr) {
        i_err = js_settings_update_file (s_buff, s_fname);
        free (s_buff);
    }
    return i_err;
}
/*----------------------------------------------------------------------------*/

