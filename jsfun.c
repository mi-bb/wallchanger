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
#include "jsfun.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set settings based on json data in buffer.
 *
 * @param[out]  ws_sett  Program settings
 * @param[in]   s_buff   Pointer to buffer with data
 * @return      Convert status
 * @retval      0  OK
 */
static int
js_json_buff_to_settings (WallSett   *ws_sett,
                          const char *s_buff)
{
    json_object *j_obj;       /* Json object of whole Json file */
    json_object *j_bgarray;   /* Json object for files array */
    json_object *j_val;       /* Json object for settings */
    int          i_arlen = 0; /* Number of items in array */

    if (s_buff == NULL)
        return 0;

    j_obj = json_tokener_parse (s_buff);

    if (json_object_object_get_ex (j_obj, "Backgrounds", &j_bgarray) &&
        json_object_get_type (j_bgarray) == json_type_array) {

        flist_clear (&ws_sett->fl_files);

        i_arlen = json_object_array_length (j_bgarray);

        for (int i = 0; i < i_arlen; ++i) {
            json_object *j_val;
            j_val = json_object_array_get_idx (j_bgarray, i);
            if (j_val != NULL) {
                flist_insert_data (&ws_sett->fl_files,
                                   json_object_get_string (j_val));
            }
        }
    }
    if (json_object_object_get_ex (j_obj, "Random wallpaper", &j_val) &&
        json_object_get_type(j_val) == json_type_int) {

        settings_set_random (ws_sett, json_object_get_int (j_val));
    }
    if (json_object_object_get_ex (j_obj, "Set last used wallpaper", &j_val) &&
        json_object_get_type(j_val) == json_type_int) {
        
        settings_set_last_used_setting (ws_sett, json_object_get_int (j_val));
    }
    if (json_object_object_get_ex (j_obj, "Last used wallpaper pos", &j_val) &&
        json_object_get_type(j_val) == json_type_int) {

        settings_set_last_used_pos (ws_sett, json_object_get_int (j_val));
    }
    if (json_object_object_get_ex (j_obj,
                                   "Wallpaper change interval", &j_val) &&
        json_object_get_type(j_val) == json_type_int) {

        settings_set_interval (ws_sett, json_object_get_int (j_val));
    }
    if (json_object_object_get_ex (j_obj, "Background set command", &j_val) &&
        json_object_get_type(j_val) == json_type_string) {

        settings_set_command (ws_sett, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, "Last used wallpaper file", &j_val) &&
        json_object_get_type(j_val) == json_type_string) {

        settings_set_last_used_fn (ws_sett, json_object_get_string (j_val));
    }
    json_object_put (j_obj);
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make a json format buffer of WallSet settings object.
 *
 * @param[in]   ws_sett  Program settings
 * @param[out]  s_buff   Pointer to output buffer
 * @return      Convert status
 * @retval      0  OK
 */
static int
js_settings_to_json_buff (WallSett  *ws_sett,
                          char     **s_buff)
{
    const char  *s_fn    = NULL; /* File name string */
    const char  *s_tmp   = NULL; /* Temp string for json buffer */
    uint32_t     ui_cnt  = 0;    /* Number of items in array */
    json_object *j_obj   = json_object_new_object();
    json_object *j_array = json_object_new_array();

    ui_cnt = flist_get_len (&ws_sett->fl_files);

    for (uint32_t i = 0; i < ui_cnt; ++i) {
        s_fn = flist_get_data (&ws_sett->fl_files, i);
        json_object_array_add (j_array, json_object_new_string (s_fn));
    }

    json_object_object_add (j_obj, "Backgrounds", j_array);

    json_object_object_add (j_obj,
                            "Random wallpaper",
                            json_object_new_int (settings_get_random (ws_sett)));

    json_object_object_add (j_obj,
                            "Set last used wallpaper",
                            json_object_new_int (
                                settings_get_last_used_setting (ws_sett)));

    json_object_object_add (j_obj,
                            "Last used wallpaper pos",
                            json_object_new_int(
                                settings_get_last_used_pos (ws_sett)));

    if (settings_get_last_used_fn (ws_sett) != NULL) {
        json_object_object_add (j_obj,
                                "Last used wallpaper file",
                                json_object_new_string (
                                    settings_get_last_used_fn (ws_sett)));
    }
    if (ws_sett->s_bgcmd != NULL) {
        json_object_object_add (j_obj,
                                "Background set command",
                                json_object_new_string (
                                    settings_get_command (ws_sett)));
    }
    json_object_object_add(j_obj,
                           "Wallpaper change interval",
                           json_object_new_int (settings_get_interval (ws_sett)));

    s_tmp = json_object_to_json_string (j_obj);

    create_resize ((void**) s_buff, strlen (s_tmp) + 1, sizeof (char));

    strcpy (*s_buff, s_tmp);

    json_object_put (j_obj);

    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used wallpaper string in json buffer. 
 *
 * @param[in,out]  s_buff   Json buffer
 * @param[in]      s_lu     Last used wallpaper file path
 * @param[in]      i_lu     Last used wallpaper position on list
 * @return         Updating data status
 */
static int
js_json_buffer_update_last_used (char       **s_buff,
                                 const char  *s_lu,
                                 const int    i_lu)
{
    json_object *j_obj;
    const char  *s_buff2 = NULL; /* Temp buffer for json data */

    if (*s_buff == NULL) {
        j_obj = json_object_new_object();
    }
    else {
        j_obj = json_tokener_parse (*s_buff);
    }
    json_object_object_add(j_obj,"Last used wallpaper file", 
                           json_object_new_string (s_lu));
    json_object_object_add(j_obj,"Last used wallpaper pos",
                           json_object_new_int(i_lu));

    s_buff2 = json_object_to_json_string (j_obj);

    if (strlen (s_buff2) != strlen (*s_buff)) {
        create_resize ((void**) s_buff, strlen (s_buff2) + 1, sizeof (char));
    }
    strcpy (*s_buff, s_buff2);

    json_object_put (j_obj);

    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get settings data from file.
 */
int
js_settings_read (WallSett   *ws_sett,
                  const char *s_fname)
{
    char *s_buff = NULL; /* File data buffer */
    int   i_res  = 0;    /* Function result */

    /* Read config data to buffer and check its hash */
    i_res = read_file_data_hash (s_fname, &s_buff, &ws_sett->i_hash);
    if (i_res)
        return i_res;

    /* Make WallSett settings data of raw json buffer */
    i_res =  js_json_buff_to_settings (ws_sett, s_buff);
    free (s_buff);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Saving program settings data in file. 
 */
int
js_settings_write (WallSett   *ws_sett,
                   const char *s_fname)
{
    char   *s_buff = NULL; /* File data buffer */
    int     i_res  = 0;    /* Function result */
    int32_t i_pos  = 0;    /* Last used wallpaper position on list */

    /* Checking last used wallpaper position correctness */
    if (settings_get_last_used_fn (ws_sett) != NULL) {
        i_pos = flist_get_pos (&ws_sett->fl_files,
                               settings_get_last_used_fn (ws_sett));
        if (i_pos >= 0) {
            settings_set_last_used_pos (ws_sett, (uint32_t) i_pos);
        }
    }

    /* Read config data to buffer and check its hash */
    i_res = read_file_data_hash (s_fname, &s_buff, &ws_sett->i_hash);
    if (i_res)
        return i_res;

    /* Make json raw buffer of current settings */
    i_res = js_settings_to_json_buff (ws_sett, &s_buff);
    if (i_res)
        return i_res;

    /* Compare saved file buffer hash and new one,
     * if they are different save settings */
    if (hash (s_buff) != ws_sett->i_hash) {
        i_res = save_file_data (s_fname, s_buff);
    }

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used wallpaper position in program settings. 
 */
int
js_settings_update_last_used (WallSett   *ws_sett,
                              const char *s_fname)
{
    char *s_buff = NULL;   /* File data buffer */
    int   i_res  = ERR_OK; /* Function result */

    if (settings_get_last_used_fn(ws_sett) == NULL) 
        return ERR_OK;

    /* Read config data to buffer and check its hash */
    i_res = read_file_data_hash (s_fname, &s_buff, &ws_sett->i_hash);
    if (i_res) {
        free (s_buff);
        return i_res;
    }
    /* If nothing read make buffer of current settings */
    if (s_buff == NULL) {
        js_settings_to_json_buff (ws_sett, &s_buff);
    }
    /* Update only last used wallpaper info in buffer */
    i_res = js_json_buffer_update_last_used (&s_buff,
            settings_get_last_used_fn (ws_sett),
            settings_get_last_used_pos (ws_sett));
    if (i_res) {
        free (s_buff);
        return i_res;
    }
    /* Compare saved file buffer hash and new with changed last used wallpaper
     * info, if they are different save settings */
    if (hash (s_buff) != ws_sett->i_hash) {
        i_res = save_file_data (s_fname, s_buff);
        if (i_res) {
            free (s_buff);
            return i_res;
        }
    }
    free (s_buff);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are different that saved ones.
 */
int
js_settings_check_changed (WallSett *ws_sett,
                           uint8_t  *i_changed)
{
    char *s_buff = NULL;   /* File data buffer */
    int   i_res  = ERR_OK; /* Function result */

    *i_changed = 0;

    /* Read config data to buffer and check its hash */
    i_res = read_file_data_hash (settings_get_cfg_fn (ws_sett),
                                 &s_buff, &ws_sett->i_hash);
    if (i_res) {
        free (s_buff);
        return i_res;
    }
    /* Get json raw buffer of current settings */
    i_res = js_settings_to_json_buff (ws_sett, &s_buff);
    if (i_res) {
        free (s_buff);
        return i_res;
    }
    /* Compare saved file buffer hash and new one,
     * if they are different save settings */
    if (hash (s_buff) != ws_sett->i_hash) {
        /* settings changed */
        *i_changed = 1;
    }
    free (s_buff);

    return ERR_OK;
}
/*----------------------------------------------------------------------------*/

