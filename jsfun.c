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
    json_object *j_obj;
    json_object *j_bgarray;
    json_object *j_val;

    if (s_buff == NULL)
        return 0;

    j_obj = json_tokener_parse (s_buff);
    if (json_object_object_get_ex (j_obj, "Backgrounds", &j_bgarray) &&
        json_object_get_type(j_bgarray) == json_type_array) {
        int i_arlen = json_object_array_length (j_bgarray);
        for (int i = 0; i < i_arlen; ++i) {
            json_object *j_val;
            j_val = json_object_array_get_idx (j_bgarray, i);
            if (j_val != NULL) {
                const char *s_str1 = json_object_get_string (j_val);
                char *s_fn = g_strdup (s_str1);
                ws_sett->gsl_files = g_slist_append (ws_sett->gsl_files, s_fn);
                json_object_put (j_val);
            }
        }
    }
    if (json_object_object_get_ex (j_obj, "Random wallpaper", &j_val) &&
        json_object_get_type(j_val) == json_type_int) {
        ws_sett->i_random = json_object_get_int (j_val);
        json_object_put (j_val);
    }
    if (json_object_object_get_ex (j_obj, "Set last used wallpaper", &j_val) &&
        json_object_get_type(j_val) == json_type_int) {
        ws_sett->i_lastsett = json_object_get_int (j_val);
        json_object_put (j_val);
    }
    if (json_object_object_get_ex (j_obj, "Last used wallpaper pos", &j_val) &&
        json_object_get_type(j_val) == json_type_int) {
        ws_sett->i_lastused = json_object_get_int (j_val);
        json_object_put (j_val);
    }
    if (json_object_object_get_ex (j_obj,
                                   "Wallpaper change interval", &j_val) &&
        json_object_get_type(j_val) == json_type_int) {
        ws_sett->i_chinterval = json_object_get_int (j_val);
        json_object_put (j_val);
    }
    if (json_object_object_get_ex (j_obj, "Background set command", &j_val) &&
        json_object_get_type(j_val) == json_type_string) {
        const char *s_str1 = json_object_get_string (j_val);
        ws_sett->s_bgcmd = g_strdup (s_str1);
        json_object_put (j_val);
    }
    if (json_object_object_get_ex (j_obj, "Last used wallpaper file", &j_val) &&
        json_object_get_type(j_val) == json_type_string) {
        const char *s_str1 = json_object_get_string (j_val);
        ws_sett->s_lastused = g_strdup (s_str1);
        json_object_put (j_val);
    }
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
    GSList      *gsl_files1 = NULL;
    json_object *j_obj      = json_object_new_object();
    json_object *j_array    = json_object_new_array();

    gsl_files1 = g_slist_copy (ws_sett->gsl_files);

    while (gsl_files1 != NULL) {
        char *s_fn = gsl_files1->data;
        json_object *j_string = json_object_new_string (s_fn);
        json_object_array_add (j_array, j_string);
        gsl_files1 = gsl_files1->next;
    }
    g_slist_free (gsl_files1);

    json_object_object_add(j_obj, "Backgrounds", j_array);

    json_object *j_int = json_object_new_int(ws_sett->i_random);
    json_object_object_add(j_obj,"Random wallpaper", j_int);
    j_int = json_object_new_int(ws_sett->i_lastsett);
    json_object_object_add(j_obj,"Set last used wallpaper", j_int);
    j_int = json_object_new_int(ws_sett->i_lastused);
    json_object_object_add(j_obj,"Last used wallpaper pos", j_int);
    if (ws_sett->s_lastused != NULL) {
        json_object *j_string = json_object_new_string (ws_sett->s_lastused);
        json_object_object_add(j_obj,"Last used wallpaper file", j_string);
    }
    if (ws_sett->s_bgcmd != NULL) {
        json_object *j_string = json_object_new_string (ws_sett->s_bgcmd);
        json_object_object_add(j_obj,"Background set command", j_string);
    }
    j_int = json_object_new_int(ws_sett->i_chinterval);
    json_object_object_add(j_obj,"Wallpaper change interval", j_int);
    const char *s_tmp = json_object_to_json_string (j_obj);
    *s_buff = g_realloc (*s_buff, (strlen (s_tmp) + 1) * sizeof (char));
    strcpy (*s_buff, s_tmp);
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
 * @retval         0  OK
 */
static int
js_json_buffer_update_last_used (char       **s_buff,
                                 const char  *s_lu,
                                 const int    i_lu)
{
    json_object *j_obj;
    if (*s_buff == NULL) {
        j_obj = json_object_new_object();
    }
    else {
        j_obj = json_tokener_parse (*s_buff);
    }
    json_object *j_string = json_object_new_string (s_lu);
    json_object_object_add(j_obj,"Last used wallpaper file", j_string);
    json_object *j_int = json_object_new_int(i_lu);
    json_object_object_add(j_obj,"Last used wallpaper pos", j_int);
    const char *s_buff2 = json_object_to_json_string (j_obj);
    if (strlen (s_buff2) != strlen (*s_buff)) {
        *s_buff = g_realloc (*s_buff,
                (strlen (s_buff2) + 1) * sizeof (char));
    }
    strcpy (*s_buff, s_buff2);
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get settings data from file.
 *
 * @param[out] ws_sett  Settings data to write to
 * @param[in]  s_fname  Name of file with settings
 * @return     Writting status
 * @retval     0   OK
 * @retval     1   File error
 * @retval     2   Reading error
 */
int
js_settings_read (WallSett   *ws_sett,
                  const char *s_fname)
{
    char *s_buff = NULL;
    int   i_res  = 0;

    i_res = read_file_data_hash (s_fname, &s_buff, &ws_sett->i_hash);
    if (i_res)
        return i_res;
    i_res =  js_json_buff_to_settings (ws_sett, s_buff);
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Saving program settings data in file. 
 *
 * @param[in]  ws_sett   Program settings
 * @param[in]  s_fname  Settings file name
 * @return     Saving data status
 * @retval     0  OK
 * @retval     1  File open error
 * @retval     2  Read error / Wrong size of written data
 * @retval    -1  No need to save
 */
int
js_settings_write (WallSett   *ws_sett,
                   const char *s_fname)
{
    char     *s_buff = NULL;
    int       i_res  = 0;

    i_res = read_file_data_hash (s_fname, &s_buff, &ws_sett->i_hash);
    if (i_res)
        return i_res;

    i_res = js_settings_to_json_buff (ws_sett, &s_buff);
    if (i_res)
        return i_res;
    if (hash (s_buff) != ws_sett->i_hash) {
        i_res = save_file_data (s_fname, s_buff);
    }
    else {
        i_res = -1;
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used wallpaper position in program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @param[in]      s_fname  Settings file name
 * @return         Updating data status
 * @retval         0  OK
 * @retval         1  File error
 * @retval         2  Reading/writting error
 * @retval         3  Last used null
 * @retval        -1  No need to save
 */
int
js_settings_update_last_used (WallSett   *ws_sett,
                              const char *s_fname)
{
    char     *s_buff = NULL;
    int       i_res  = 0;

    if (ws_sett->s_lastused == NULL) 
        return 3;
    i_res = read_file_data_hash (s_fname, &s_buff, &ws_sett->i_hash);
    if (i_res) {
        g_free (s_buff);
        return i_res;
    }
    if (s_buff == NULL) {
        js_settings_to_json_buff (ws_sett, &s_buff);
    }
    i_res = js_json_buffer_update_last_used (&s_buff, ws_sett->s_lastused,
                                             ws_sett->i_lastused);
    if (i_res) {
        g_free (s_buff);
        return i_res;
    }
    if (hash (s_buff) != ws_sett->i_hash) {
        i_res = save_file_data (s_fname, s_buff);
        if (i_res) {
            g_free (s_buff);
            return i_res;
        }
    }
    else {
        i_res = 0;
    }
    g_free (s_buff);
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are different that saved ones.
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Checking settings status
 * @retval         0  No change
 * @retval        -1  Settings changed 
 * @retval         1  File error
 * @retval         2  Reading error
 */
int
js_settings_check_changed (WallSett *ws_sett)
{
    char     *s_buff = NULL;
    int       i_res  = 0;
    uint64_t  i_hash = 0;

    i_res = read_file_data_hash (ws_sett->s_cfgfile, &s_buff, &i_hash);
    if (i_res) {
        g_free (s_buff);
        return i_res;
    }
    js_settings_to_json_buff (ws_sett, &s_buff);
    if (i_res) {
        g_free (s_buff);
        return i_res;
    }
    if (hash (s_buff) != i_hash) {
        // settings changed
        return -1;
    }
    g_free (s_buff);
    return 0;
}
/*----------------------------------------------------------------------------*/

