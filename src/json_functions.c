/**
 * @file  json_functions.c
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
#include "config.h"

#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif

#include "json_file.h"
#include "errors.h"
#include "setting_item.h"
#include "file_io.h"
#include "string_functions.h"
#include "hash_djb2.h"
#include "json_functions.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert json object to Setting item.
 *
 * @param[in] val     Json object to convert
 * @param[in] name    Name of Setting item
 * @return     New setting item
 */
static Setting * json_json_obj_to_setting (json_object *val,
                                          const char  *name);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get items from Setting array or object and save them in Json array 
 *         or object item.
 *
 * @param[in]  setting  Setting item
 * @return     New json object
 */
static json_object * json_settings_array_to_json (const Setting *setting);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Setting object to Json object.
 *
 * @param[in] setting   Setting item to convert
 * @return    Json object
 */
static json_object * json_setting_to_json_obj  (const Setting *setting);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Setting items from SettList and put them in Json object.
 *
 * @param[in]  settings  List of Setting items
 * @param[out] obj       Json object to insert data
 * @return     none
 */
static void json_settings_add_to_json_obj (const Setting *settings,
                                           json_object   *obj);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert json object to Setting item.
 */
static Setting *
json_json_obj_to_setting (json_object *val,
                          const char  *name)
{
    json_object *item;         /* Json object read from Json array */
    Setting     *set = nullptr; /* Setting item to return */
    size_t       count = 0;    /* Length of Json array */
    size_t       i = 0;        /* i */

    int val_type = json_object_get_type (val);

    switch (val_type) {

        case json_type_null:
            break;
        case json_type_boolean:
            set = setting_new_int (name,
                                   (int64_t) json_object_get_int64 (val));
            break;
        case json_type_double:
            set = setting_new_double (name, json_object_get_double (val));
            break;
        case json_type_int:
            set = setting_new_int (name,
                                   (int64_t) json_object_get_int64 (val));
            break;
        case json_type_string:
            set = setting_new_string (name, json_object_get_string (val));
            break;
        case json_type_object:
            set = setting_new_setting (name);
            json_object_object_foreach (val, key, child) {
                setting_add_child (set, json_json_obj_to_setting (child, key));
            }
            break;
        case json_type_array:
            set = setting_new_array (name);
            count = json_object_array_length (val);
            for (i = 0; i < count; ++i) {
                if ((item = json_object_array_get_idx (val, i)) != nullptr) {
                    setting_add_child (set,
                                       json_json_obj_to_setting (item, nullptr));
                }
            }
            break;
        default:
            break;
    }
    return set;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get items from Setting array or object and save them in Json array 
 *         or object item.
 */
static json_object *
json_settings_array_to_json (const Setting *setting)
{
    json_object *array;  /* Json array to return */
    json_object *obj;    /* Json object made from Setting */
    Setting     *item;   /* For setting iteration */

    array = (setting_get_type (setting) == SET_VAL_SETTING) ?
              json_object_new_object() :
              json_object_new_array();

    item = setting_get_child (setting);

    while (item != nullptr) {

        obj = json_setting_to_json_obj (item);

        if (setting_get_type (setting) == SET_VAL_SETTING) { /* Json obj */
            json_object_object_add (array, setting_get_name (item), obj);
        }
        else { /* Json array */
            json_object_array_add (array, obj);
        }
        item = item->next;
    }
    return array;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Setting object to Json object.
 */
static json_object *
json_setting_to_json_obj (const Setting *setting)
{
    json_object *obj;
    SetValType   type = setting_get_type (setting);

    switch (type) {

        case SET_VAL_DOUBLE:
            obj = json_object_new_double (setting_get_double (setting));
            return obj;

        case SET_VAL_INT:
            obj = json_object_new_int64 ((int64_t) setting_get_int (setting));
            return obj;

        case SET_VAL_UINT:
            obj = json_object_new_int64 ((int64_t) setting_get_uint (setting));
            return obj;

        case SET_VAL_STRING:
            obj = json_object_new_string (setting_get_string (setting));
            return obj;

        case SET_VAL_SETTING:
            obj = json_settings_array_to_json (setting);
            return obj;

        case SET_VAL_ARRAY:
            obj = json_settings_array_to_json (setting);
            return obj;

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
json_settings_add_to_json_obj (const Setting *settings,
                               json_object   *obj)
{
    json_object *val;   /* Json object made from Setting */

    while (settings != nullptr) {
        val = json_setting_to_json_obj (settings);
        json_object_object_add (obj, setting_get_name (settings), val);
        settings = settings->next;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get settings data from file.
 */
Setting *
json_settings_read (const char *fname,
                    int        *err)
{
    Setting     *settings = nullptr; /* Settings to return */
    json_object *obj;                /* Json object made from file data */

    *err = ERR_OK;
    obj  = json_open_file (fname, nullptr, err);

    if (*err == ERR_OK && obj != nullptr) {
        json_object_object_foreach (obj, key, val) {
            settings = settings_append (settings,
                        json_json_obj_to_setting (val, key));
        }
#ifdef DEBUG
        printf ("-> Settings read start\n");
        settings_print (settings);
        printf ("-> Settings read end\n");
#endif
    }
    if (obj != nullptr)
        json_object_put (obj);

    return settings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings in list are an update to settings stored in
 *         settings file.
 */
char *
json_settings_check_for_update (const Setting *settings,
                                const char    *fname,
                                int           *err)
{
    json_object   *obj;          /* Json object made from file data */
    const char    *json_buff = nullptr; /* Json object as string */
    char          *res_buff  = nullptr; /* Result data buffer */
    uint_fast32_t  file_hash = 0;       /* Data read hash */

    *err = ERR_OK;

    if ((obj = json_open_file (fname, &file_hash, err)) == nullptr)
        return nullptr;

    json_settings_add_to_json_obj (settings, obj);

    json_buff = json_object_to_json_string (obj);

    /* Compare saved file buffer hash and new one,
     * if they are different update output buffer */
    res_buff = (hash (json_buff) != file_hash) ? strdup (json_buff) : nullptr;

    json_object_put (obj);

    return res_buff;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update file with new data.
 */
int
json_settings_update_file (const char *buff,
                           const char *fname)
{
    return save_file_data (fname, buff);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are an update to saved ones and update file with
 *         new data if they are.
 */
int
json_settings_check_update_file (const Setting *settings,
                                 const char    *fname)
{
    int   err  = ERR_OK; /* Possible error to return */
    char *buff = nullptr;   /* Result of update check, if it is not null there
                              is a change in configuration and returned buffer
                              is the new data to save */
    buff = json_settings_check_for_update (settings, fname, &err);
    if (buff != nullptr) {
        err = json_settings_update_file (buff, fname);
        free (buff);
    }
    return err;
}
/*----------------------------------------------------------------------------*/

