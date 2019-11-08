/**
 * @file  setts.c
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
 * @brief  Settings manage functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include "setts.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get config file path
 *
 * @return  Config full path
 */
static char *
get_config_path (void)
{
    char *s_path = NULL;            /* Result full config file name */
    char *s_home = NULL;            /* Home path */
    char *s_sett = "wchanger.json"; /* Settings file name */
    char *s_cfg  = "/.config/";     /* Settings file path */

    if ((s_home = getenv ("HOME")) == NULL) {
        s_home = getpwuid (getuid ())->pw_dir;
    }
    int i_len = strlen (s_home) + strlen (s_cfg) + strlen (s_sett);
    s_path = g_malloc0 ((i_len + 1) * sizeof (char));
    sprintf (s_path, "%s%s%s", s_home, s_cfg, s_sett);
    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if config file exists, have read/write permissions. 
 *
 * @param[in]  s_fname  Config file name
 * @return     Checking status
 * @retval     0  OK
 * @retval     1  Bad permissions
 * @retval     2  Can not be created
 */
static int
check_config_file (const char *s_fname)
{
    // check if file exists
    if (access (s_fname, F_OK) == 0) {
        // check permissions
        if (access (s_fname, W_OK | R_OK) != 0) {
            fputs ("Bad config file permissions\n", stderr);
            return 1;
        }
        else {
            return 0;
        }
    }
    else {
        // file does not exist, try to create it
        FILE *f_file = fopen(s_fname, "a+");
        if (f_file == NULL) {
            fputs ("File can not be created\n", stderr);
            return 2;
        }
        else {
            // file created
            fclose (f_file);
        }
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Change last used wallpaper info in program settings. 
 *
 * @param[in,out]  ws_sett    Program settings
 * @param[in]      s_wallnew  Wallpaper file path
 * @return         Wallpaper set status
 * @retval         0  OK
 */
int
settings_set_last_used_data (WallSett   *ws_sett,
                             const char *s_wallnew)
{
    if (ws_sett->s_lastused == NULL) {
        ws_sett->s_lastused = g_malloc0 (
                (strlen (s_wallnew) + 1) * sizeof (gchar));
    }
    else {
        ws_sett->s_lastused = g_realloc (ws_sett->s_lastused,
                (strlen (s_wallnew) + 1) * sizeof (gchar));
    }
    if (ws_sett->s_lastused != NULL) {
        strcpy (ws_sett->s_lastused, s_wallnew);
        int i_pos = my_gslist_get_position (ws_sett->gsl_files,
                                            ws_sett->s_lastused);
        ws_sett->i_lastused = i_pos;
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets command for setting wallpaper. 
 *
 * @param[out]  ws_sett  Program settings
 * @param[in]   s_cmd    New set wallpaper command
 * @return      Change status
 * @retval      0  OK
 */
int
settings_set_command (WallSett   *ws_sett,
                      const char *s_cmd)
{
    if (compare_strings (s_cmd, ws_sett->s_bgcmd) == 0)
        return 0;
    if (s_cmd == NULL)
        return 0;
    if (ws_sett->s_bgcmd == NULL)
        ws_sett->s_bgcmd = g_malloc0 ((strlen (s_cmd) + 1) * sizeof (char));
    else {
        ws_sett->s_bgcmd = g_realloc (ws_sett->s_bgcmd,
              (strlen (s_cmd) + 1) * sizeof (char));
    }
    strcpy (ws_sett->s_bgcmd, s_cmd);
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets default program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Setting status
 * @retval         0  OK
 * @retval         1  Bad permissions
 * @retval         2  Can not be created
 */
int
settings_init (WallSett *ws_sett)
{
    int i_res = 0;
    ws_sett->gsl_files    = NULL; /* Image file list */
    ws_sett->i_chinterval = 30;   /* Wallpaper change interval */ 
    ws_sett->i_random     = 0;    /* Random wallpaper change */
    ws_sett->i_lastsett   = 0;    /* Last used wallpaper setting */
    ws_sett->s_lastused   = NULL; /* Last used wallpaper file name */
    ws_sett->i_lastused   = -1;   /* Index of last used wallpaper */
    ws_sett->s_bgcmd      = NULL; /* Background set command */
    ws_sett->i_hash       = 0;    /* Data hash variable */

    ws_sett->s_cfgfile = get_config_path ();

    i_res = check_config_file (ws_sett->s_cfgfile);
    if (i_res != 0) {
        return i_res;
    }
    settings_set_command (ws_sett, "feh --bg-fill");
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Reading settings status
 * @retval         0  OK
 * @retval         1  File error
 * @retval         2  Reading error
 */
int
settings_read (WallSett *ws_sett) 
{
    return js_settings_read (ws_sett, ws_sett->s_cfgfile);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Write program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Writting settings status
 * @retval         0  OK
 * @retval         1  File open error
 * @retval         2  Read error / Wrong size of written data
 * @retval        -1  No need to save
 */
int
settings_write (WallSett *ws_sett)
{
    return js_settings_write (ws_sett, ws_sett->s_cfgfile);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used wallpaper position in program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Updating settings status
 * @retval         0  OK
 * @retval         1  File error
 * @retval         2  Reading/writting error
 * @retval         3  Last used null
 * @retval        -1  No need to save
 */
int
settings_update_last_used (WallSett *ws_sett)
{
    return js_settings_update_last_used (ws_sett, ws_sett->s_cfgfile);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are different that saved ones.
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Checking settings status
 * @retval         0  No change
 * @retval        -1  Settings changed 
 * @retval         1  Error
 */
int
settings_check_changed (WallSett *ws_sett)
{
    return js_settings_check_changed (ws_sett);
}
/*----------------------------------------------------------------------------*/

