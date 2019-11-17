/**
* @file  settstr.c
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
* @brief  Wallpaper settings WallSett structure and functions
* 
* @author Michał Bąbik <michalb1981@o2.pl>
*/
#include "settstr.h"

/*----------------------------------------------------------------------------*/
/**
 * @brief  Check permissions, existence of file or directory. 
 *
 * @param[in]  s_name  Name of file / directory to check
 * @param[in]  i_mode  Permissions to check
 * @return     Checking status
 */
static int
check_permissions (const char *s_name,
                   int         i_mode)
{
    /* check if file/dir exists */
    if (access (s_name, F_OK) == 0) {
        /* check permissions */
        if (access (s_name, i_mode) != 0) {
            fputs ("Bad permissions\n", stderr);
            return ERR_FILE_RW;
        }
        else {
            /* Permissions OK */
            return ERR_OK;
        }
    }
    else {
        /* File/dir does not exist */
        return ERR_FILE_EX;
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check file permissions (read write), existence. 
 *
 * @param[in]  s_file  File name to check
 * @return     Checking status
 */
static int
check_file_permissions (const char *s_file)
{
    return check_permissions (s_file, W_OK | R_OK);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check directory permissions (read, write, execute), existence. 
 *
 * @param[in]  s_dir  Directory name to check
 * @return     Checking status
 */
static int
check_dir_permissions (const char *s_dir)
{
    return check_permissions (s_dir, W_OK | R_OK | X_OK);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check directory permissions, existence and create if needed. 
 *
 * @param[in]  s_dir  Directory name to check / create
 * @return     Checking / creating status
 */
static int
check_dir_premissions_create (const char *s_dir)
{
    int i_res = ERR_OK;

    i_res = check_dir_permissions (s_dir);

    /* Everything OK */
    if (i_res == ERR_OK) {
        return i_res;
    }
    /* If directory does not exist */
    else if (i_res == ERR_FILE_EX) {
        /* try to create it */
        int i_res2 = mkdir (s_dir, 0700);
        if (i_res2 == 0) {
            return ERR_OK;
        }
        else {
            fputs ("Directory can not be created\n", stderr);
            return ERR_FILE_CR;
        }
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check file permissions, existence and maybe create it. 
 *
 * @param[in]  s_file  Name of file to check / create
 * @return     Checking / creating status
 */
int
check_file_premissions_create (const char *s_file)
{
    int i_res = ERR_OK;

    i_res = check_file_permissions (s_file);

    /* Everything OK */
    if (i_res == ERR_OK) {
        return i_res;
    }
    /* If file does not exist */
    else if (i_res == ERR_FILE_EX) {
        /* try to create it */
        FILE *f_file = fopen(s_file, "a+");
        if (f_file == NULL) {
            fputs ("File can not be created\n", stderr);
            return ERR_FILE_CR;
        }
        else {
            /* file created */
            fclose (f_file);
            return ERR_OK;
        }
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if config file and path exists, check read/write permissions. 
 *
 * Checks config path and file existence, creates them if needed. Function
 * returns checking/creating status and writes config file path to s_file if
 * process completed succefully.
 *
 * @param[out]  s_file  Config file name
 * @return      Checking / creating status
 */
/*----------------------------------------------------------------------------*/
static int
check_config_path_file (char **s_file)
{
    const char *s_sett = "wchanger.json"; /* Settings file name */
    const char *s_cfg  = "/.config/";     /* Settings file path */
    char       *s_path = NULL;            /* Result full config file name */
    char       *s_home = NULL;            /* Home path */
    size_t      i_len  = 0;               /* Config path length */
    int         i_res  = 0;               /* Function result */

    /* Getting user's HOME path */
    if ((s_home = getenv ("HOME")) == NULL) {
        s_home = getpwuid (getuid ())->pw_dir;
    }

    /* Total config file path and name length */
    i_len = strlen (s_home) + strlen (s_cfg) + strlen (s_sett);

    /* Create string for config file path and name */
    create_resize ((void**) &s_path, (i_len + 1), sizeof (char));

    /* Copy config file path */
    strcpy (s_path, s_home);
    strcat (s_path, s_cfg);

    /* Check config path existence and permissions,
     * create directory if needed */
    i_res = check_dir_premissions_create (s_path);
    if (i_res != ERR_OK) {
        free (s_path);
        return i_res;
    }

    /* Append file name to config path */
    strcat (s_path, s_sett);

    /* Check config file existence and permissions,
     * create file if needed */
    i_res = check_file_premissions_create (s_path);
    if (i_res != ERR_OK) {
        free (s_path);
        return i_res;
    }

    /* Everything went ok I suppose, assign config file string pointer */
    *s_file = s_path;

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets default program settings. 
 */
int
settings_init (WallSett *ws_sett)
{
    int   i_res            = 0;    /* Return result value */
    char *s_cfg_file       = NULL; /* Config file temp string pointer */

    ws_sett->i_chinterval  = 30;   /* Wallpaper change interval */ 
    ws_sett->i_random      = 0;    /* Random wallpaper change */
    ws_sett->i_lastsett    = 0;    /* Last used wallpaper setting */
    ws_sett->s_lastused    = NULL; /* Last used wallpaper file name */
    ws_sett->i_lastused    = -1;   /* Index of last used wallpaper */
    ws_sett->s_bgcmd       = NULL; /* Background set command */
    ws_sett->i_hash        = 0;    /* Data hash variable */
    ws_sett->ui_win_width  = 1024; /* Application window width */
    ws_sett->ui_win_height = 768;  /* Application window height */

    flist_init (&ws_sett->fl_files);
    randomm_init (&ws_sett->rm_mem);
    
    i_res = check_config_path_file (&s_cfg_file);

    ws_sett->s_cfgfile = s_cfg_file;

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
settings_free (WallSett *ws_sett)
{
    flist_free (&ws_sett->fl_files);
    free (ws_sett->s_bgcmd);
    free (ws_sett->s_cfgfile);
    free (ws_sett->s_lastused);
    ws_sett->s_bgcmd = NULL;
    ws_sett->s_cfgfile = NULL;
    ws_sett->s_lastused = NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get config file name.
 */
const char *
settings_get_cfg_fn (WallSett *ws_sett)
{
    return (const char*) ws_sett->s_cfgfile;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set last used wallpaper position on list.
 */
void
settings_set_last_used_pos (WallSett *ws_sett,
                            int32_t   ui_val)
{
    ws_sett->i_lastused = ui_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get last used position value.
 */
int32_t
settings_get_last_used_pos (WallSett *ws_sett)
{
    return ws_sett->i_lastused;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set using last used wallpaper on start.
 */
void
settings_set_last_used_setting (WallSett *ws_sett,
                                uint8_t   ui_val)
{
    ws_sett->i_lastsett = ui_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get using last used wallpaper on start.
 */
uint8_t
settings_get_last_used_setting (WallSett *ws_sett)
{
    return ws_sett->i_lastsett;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random wallpaper select value.
 */
void
settings_set_random (WallSett *ws_sett,
                     uint8_t   ui_val)
{
    ws_sett->i_random = ui_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get random wallpaper select value.
 */
uint8_t
settings_get_random (WallSett *ws_sett)
{
    return ws_sett->i_random;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set wallpaper change interval value.
 */
void
settings_set_interval (WallSett *ws_sett,
                       uint32_t  ui_val)
{
    ws_sett->i_chinterval = ui_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper change interval value.
 */
uint32_t
settings_get_interval (WallSett *ws_sett)
{
    return ws_sett->i_chinterval;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Change last used wallpaper file name in program settings. 
 */
int
settings_set_last_used_fn (WallSett   *ws_sett,
                           const char *s_fn)
{
    int i_pos = 0; /* Last used data position in list */

    if (s_fn == NULL)
        return ERR_OK;

    if (compare_strings (s_fn, ws_sett->s_lastused) == 0)
        return ERR_OK;

    create_resize ((void**) &ws_sett->s_lastused,
                   strlen (s_fn) + 1,
                   sizeof (char));

    if (ws_sett->s_lastused != NULL) {
        strcpy (ws_sett->s_lastused, s_fn);

        i_pos = flist_get_pos (&ws_sett->fl_files, ws_sett->s_lastused);
        settings_set_last_used_pos (ws_sett, i_pos);
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get last used wallpaper file name.
 */
const char *
settings_get_last_used_fn (WallSett *ws_sett)
{
    return (const char*) ws_sett->s_lastused;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets command for setting wallpaper. 
 */
int
settings_set_command (WallSett   *ws_sett,
                      const char *s_cmd)
{
    if (s_cmd == NULL)
        return ERR_OK;

    if (compare_strings (s_cmd, ws_sett->s_bgcmd) == 0)
        return ERR_OK;

    create_resize ((void**) &ws_sett->s_bgcmd,
                   strlen (s_cmd) + 1,
                   sizeof (char));
    strcpy (ws_sett->s_bgcmd, s_cmd);

    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper set command.
 */
const char *
settings_get_command (WallSett *ws_sett)
{
    return (const char*) ws_sett->s_bgcmd;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set application window width and height value.
 */
void
settings_set_window_width (WallSett      *ws_sett,
                           const uint16_t ui_width)
{
    ws_sett->ui_win_width  = ui_width;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get application window width and height value.
 */
int
settings_get_window_width (WallSett *ws_sett)
{
    return ws_sett->ui_win_width;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set application window width and height value.
 */
void
settings_set_window_height (WallSett     *ws_sett,
                           const uint16_t ui_height)
{
    ws_sett->ui_win_height  = ui_height;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get application window width and height value.
 */
int
settings_get_window_height (WallSett *ws_sett)
{
    return ws_sett->ui_win_height;
}
/*----------------------------------------------------------------------------*/

