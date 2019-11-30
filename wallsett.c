/**
* @file  wallsett.c
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
#include <string.h>
#include <stdlib.h>
#include "miscfun.h"
#include "errs.h"
#include "wallsett.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets default program settings. 
 */
int
wallset_init (WallSett *ws_sett)
{
    int   i_err            = 0;    /* Return result value */
    ws_sett->i_chinterval  = 30;   /* Wallpaper change interval */ 
    ws_sett->i_random      = 0;    /* Random wallpaper change */
    ws_sett->i_lastsett    = 0;    /* Last used wallpaper setting */
    ws_sett->s_lastused    = NULL; /* Last used wallpaper file name */
    ws_sett->s_bgcmd       = NULL; /* Background set command */

    flist_init (&ws_sett->fl_files);
    randomm_init (&ws_sett->rm_mem);
    
    ws_sett->s_cfgfile = check_config_path_file (&i_err);

    return i_err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
wallset_free (WallSett *ws_sett)
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
wallset_get_cfg_fn (WallSett *ws_sett)
{
    return (const char*) ws_sett->s_cfgfile;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set using last used wallpaper on start.
 */
void
wallset_set_last_used_setting (WallSett *ws_sett,
                               uint8_t   ui_val)
{
    ws_sett->i_lastsett = ui_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get using last used wallpaper on start.
 */
uint8_t
wallset_get_last_used_setting (WallSett *ws_sett)
{
    return ws_sett->i_lastsett;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random wallpaper select value.
 */
void
wallset_set_random (WallSett *ws_sett,
                    uint8_t   ui_val)
{
    ws_sett->i_random = ui_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get random wallpaper select value.
 */
uint8_t
wallset_get_random (WallSett *ws_sett)
{
    return ws_sett->i_random;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set wallpaper change interval value.
 */
void
wallset_set_interval (WallSett *ws_sett,
                      uint32_t  ui_val)
{
    ws_sett->i_chinterval = ui_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper change interval value.
 */
uint32_t
wallset_get_interval (WallSett *ws_sett)
{
    return ws_sett->i_chinterval;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Change last used wallpaper file name in program settings. 
 */
int
wallset_set_last_used_fn (WallSett   *ws_sett,
                          const char *s_fn)
{
    //int i_pos = 0; /* Last used data position in list */

    if (s_fn == NULL)
        return ERR_OK;

    if (compare_strings (s_fn, ws_sett->s_lastused) == 0)
        return ERR_OK;

    create_resize ((void**) &ws_sett->s_lastused,
                   strlen (s_fn) + 1,
                   sizeof (char));

    if (ws_sett->s_lastused != NULL) {

        strcpy (ws_sett->s_lastused, s_fn);
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get last used wallpaper file name.
 */
const char *
wallset_get_last_used_fn (WallSett *ws_sett)
{
    return (const char*) ws_sett->s_lastused;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets command for setting wallpaper. 
 */
int
wallset_set_command (WallSett   *ws_sett,
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
wallset_get_command (WallSett *ws_sett)
{
    return (const char*) ws_sett->s_bgcmd;
}
/*----------------------------------------------------------------------------*/

