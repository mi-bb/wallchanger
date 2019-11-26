/**
 * @file  jsfun.c
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
 * @brief  Json functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <json-c/json.h>
#include "errs.h"
#include "setting.h"
#include "miscfun.h"
#include "jsfun.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static void js_json_array_to_stlist (json_object *j_array,
                                             SettList    *st_list,
                                             const char  *s_array_name)
 * @brief      Get items from Json array and add them to SettList object.
 * @param[in]  j_array       Array of Json objects
 * @param[in]  st_list       SettList list to add elements
 * @param[in]  s_array_name  Name of array for the elements
 * @return     none
 *
 * @fn  static void js_stlist_array_to_json (SettList    *st_list,
                                             Setting     *st_sett,
                                             json_object *j_array)
 * @brief      Get items from Setting array and save them in Json array object.
 * @param[in]  st_list  List of all settings
 * @param[in]  st_sett  Array Setting to process
 * @param[out] j_array  Json aray object to insert data
 * @return     None
 */
/*----------------------------------------------------------------------------*/
static void js_json_array_to_stlist (json_object *j_array,
                                     SettList    *st_list,
                                     const char  *s_array_name);
/*----------------------------------------------------------------------------*/
static void js_stlist_array_to_json (SettList    *st_list,
                                     Setting     *st_sett,
                                     json_object *j_array);
/*----------------------------------------------------------------------------*/
/**
 * @fn  static Setting * js_json_object_to_setting (json_object *val,
 *                                                  const char  *s_name,
 *                                                  SettList    *st_list)
 * @brief      Convert Json object to Setting object
 * @param[in]  val      Json object to process
 * @param[in]  s_name   Destination Setting name
 * @param[out] st_list  SettList to insert settings
 * @return     Setting object
 *
 * @fn  static json_object * js_setting_to_json_object (SettList *st_list,
 *                                                      Setting  *st_sett)
 * @brief      Convert Setting object to Json object
 * @param[in]  st_list   List of all settings
 * @param[in]  st_sett   Setting to examine
 * @return     Json object
 */
/*----------------------------------------------------------------------------*/
static Setting     * js_json_object_to_setting (json_object *val,
                                                const char  *s_name,
                                                SettList    *st_list);
/*----------------------------------------------------------------------------*/
static json_object * js_setting_to_json_object (SettList    *st_list,
                                                Setting     *st_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert raw Json data string to SettList list of Setting objects.
 *
 * @param[in]  s_buff  String with Json data
 * @return     SettList list
 */
static SettList * js_json_string_to_settlist (const char *s_buff);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Setting items from SettList and put them in Json object
 *
 * @param[in]  st_list  List of Setting items
 * @param[out] j_obj    Json object to insert data
 * @return     none
 */
static void js_settlist_append_to_json_object (SettList    *st_list,
                                               json_object *j_obj);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get items from Json array and add them to SettList object.
 */
static void
js_json_array_to_stlist (json_object *j_array,
                         SettList    *st_list,
                         const char  *s_array_name)
{
    json_object *j_val;
    Setting     *st_set;
    char        *s_name = NULL;
    size_t       ui_cnt = 0;
    size_t       i      = 0;

    ui_cnt = json_object_array_length (j_array);

    for (i = 0; i < ui_cnt; ++i) {

        j_val = json_object_array_get_idx (j_array, i);

        if (j_val != NULL) {
            s_name = string_name_with_number (s_array_name, i);
            st_set = js_json_object_to_setting (j_val, s_name, st_list);
            free (s_name);
            if (st_set != NULL)
                stlist_insert_setting_to_array (st_list, st_set, s_array_name);
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Json object to Setting object
 */
static Setting *
js_json_object_to_setting (json_object *val,
                           const char  *s_name,
                           SettList    *st_list)
{
    Setting *st_set;
    int      i_val_type = 0;

    i_val_type = json_object_get_type (val);

    switch (i_val_type) {
        case json_type_null:
            return NULL;
        case json_type_boolean:
            st_set = setting_new_int8 (json_object_get_int (val), s_name);
            return st_set;
        case json_type_double:
            st_set = setting_new_double (json_object_get_double (val), s_name);
            return st_set;
        case json_type_int:
            st_set = setting_new_uint32 (json_object_get_int (val), s_name);
            return st_set;
        case json_type_string:
            st_set = setting_new_string (json_object_get_string (val), s_name);
            return st_set;
        case json_type_object:
            return NULL;
        case json_type_array:
            st_set = setting_new_array (s_name);
            stlist_insert_setting (st_list, st_set);
            js_json_array_to_stlist (val, st_list, s_name);
            return NULL;
        default:
            break;
    }
    return NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert raw Json data string to SettList list of Setting objects.
 */
static SettList *
js_json_string_to_settlist (const char *s_buff)
{
    json_object *j_obj;
    SettList    *st_list;
    Setting     *st_set;

    if (s_buff == NULL)
        return NULL;

    st_list = stlist_new_list ();

    j_obj = json_tokener_parse (s_buff);

    json_object_object_foreach(j_obj, key, val) {
        st_set = js_json_object_to_setting (val, key, st_list);
        if (st_set != NULL)
            stlist_insert_setting (st_list, st_set);
    }
    json_object_put (j_obj);
    return st_list;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get items from Setting array and save them in Json array object.
 */
static void
js_stlist_array_to_json (SettList    *st_list,
                               Setting     *st_sett,
                               json_object *j_array)
{
    Setting     *st_val;
    json_object *j_obj;
    SettList    *st_array_list = stlist_get_settings_in_array_obj (st_list,
                                                                   st_sett);
    uint32_t     ui_cnt = 0;
    uint32_t     i      = 0;

    ui_cnt = stlist_get_length (st_array_list);

    for (i = 0; i < ui_cnt; ++i) {
        st_val = stlist_get_setting_at_pos (st_array_list, i);

        j_obj = js_setting_to_json_object (st_list, st_val);

        json_object_array_add (j_array, j_obj);
    }
    stlist_free (st_array_list);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Setting object to Json object
 */
static json_object *
js_setting_to_json_object (SettList *st_list,
                           Setting  *st_sett)
{
    json_object *j_obj;
    SetValType   i_type = 0;

    i_type = setting_get_type (st_sett);

    switch (i_type) {
        case SET_VAL_DOUBLE:
            j_obj = json_object_new_double (setting_get_double (st_sett));
            return j_obj;
        case SET_VAL_INT:
            j_obj = json_object_new_int (setting_get_int (st_sett));
            return j_obj;
        case SET_VAL_UINT32:
            j_obj = json_object_new_int (setting_get_uint32 (st_sett));
            return j_obj;
        case SET_VAL_STRING:
            j_obj = json_object_new_string (setting_get_string (st_sett));
            return j_obj;
        case SET_VAL_ARRAY:
            j_obj = json_object_new_array();
            js_stlist_array_to_json (st_list, st_sett, j_obj);
            return j_obj;
        default:
            break;
    }
    return NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Setting items from SettList and put them in Json object
 */
static void
js_settlist_append_to_json_object (SettList    *st_list,
                                   json_object *j_obj)
{
    json_object *j_val;
    SettList    *st_main;
    Setting     *st_sett;
    uint32_t     ui_cnt   = 0;
    uint32_t     i        = 0;

    if (j_obj == NULL || st_list == NULL)
        return;

    st_main  = stlist_get_settings_main (st_list);

    ui_cnt = stlist_get_length (st_main);

    for (i = 0; i < ui_cnt; ++i) {
        st_sett = stlist_get_setting_at_pos (st_main, i);
        j_val = js_setting_to_json_object (st_list, st_sett);
        json_object_object_add (j_obj, setting_get_name (st_sett), j_val);
    }
    stlist_free (st_main);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get settings data from file.
 */
SettList *
js_settings_read (const char *s_fname,
                  int        *i_err)
{
    SettList *st_list;
    char     *s_buff = NULL; /* File data buffer */

    *i_err = read_file_data (s_fname, &s_buff);
    if (*i_err)
        return NULL;

    if (s_buff == NULL)
        return NULL;

    st_list = js_json_string_to_settlist (s_buff);

    free (s_buff);

    return st_list;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings in SettList are an update to settings
 *         stored in settings file.
 */
int
js_settings_check_for_update (SettList    *st_list,
                              const char  *s_fname,
                              char       **s_out_buff)
{
    json_object *j_obj;
    const char  *s_jbuff = NULL;   /* Json object as string */
    char        *s_buff  = NULL;   /* File data buffer */
    uint64_t     i_hash  = 0;      /* File read hash */
    int          i_res   = ERR_OK; /* Function result */

    i_res = read_file_data_hash (s_fname, &s_buff, &i_hash);
    if (i_res) {
        free (s_buff);
        return i_res;
    }

    if (s_buff == NULL) {
        j_obj = json_object_new_object();
    }
    else {
        j_obj = json_tokener_parse (s_buff);
    }

    free (s_buff);

    js_settlist_append_to_json_object (st_list, j_obj);

    s_jbuff = json_object_to_json_string (j_obj);

    /* Compare saved file buffer hash and new one,
     * if they are different update output buffer */
    if (hash (s_jbuff) != i_hash) {
        *s_out_buff = str_dup (s_jbuff);
    }
    else {
        *s_out_buff = NULL;
    }

    json_object_put (j_obj);
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update file with new data
 */
int
js_settings_update_file (const char *s_buff,
                         const char *s_fname)
{
    return save_file_data (s_fname, s_buff);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are an update and update file with new data
 *         if they are.
 */
int
js_settings_check_update_file (SettList   *st_list,
                               const char *s_fname)
{
    char *s_buff = NULL;
    int   i_res  = 0;
    
    i_res = js_settings_check_for_update (st_list, s_fname, &s_buff);

    if (s_buff != NULL) {
        js_settings_update_file (s_buff, s_fname);
        free (s_buff);
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/

