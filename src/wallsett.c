/**
* @file  wallsett.c
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
* @brief  Wallpaper settings WallSett structure and functions
* 
* @author Michał Bąbik <michalb1981@o2.pl>
*/
#include <string.h>
#include <stdlib.h>
#include "cfgfile.h"
#include "errs.h"
#include "wallsett.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets default program settings. 
 */
int
wallset_init (WallSett *ws_sett)
{
    int i_err              = ERR_OK; /* Return result value */
    ws_sett->i_chinterval  = 30;     /* Wallpaper change interval */ 
    ws_sett->i_random      = 0;      /* Random wallpaper change */
    ws_sett->i_lastsett    = 0;      /* Last used wallpaper setting */
    ws_sett->s_lastused    = NULL;   /* Last used wallpaper file name */
    ws_sett->s_bgcmd       = NULL;   /* Background set command */
    ws_sett->sl_walls      = NULL;   /* Wallpaper list */

    memset (ws_sett->s_cfgfile, 0, sizeof (ws_sett->s_cfgfile));

    randomm_init (&ws_sett->rm_mem);

    i_err = set_config_file_path (ws_sett->s_cfgfile);

    return i_err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
wallset_free (WallSett *ws_sett)
{
    stlist_free_p (ws_sett->sl_walls);
    ws_sett->s_bgcmd    = NULL;
    ws_sett->s_lastused = NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get config file name.
 */
const char *
wallset_get_cfg_fn (const WallSett *ws_sett)
{
    return (const char*) ws_sett->s_cfgfile;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets wallpaper list value.
 */
void
wallset_set_wallpaper_list (WallSett *ws_sett,
                            SettList *sl_list)
{
    ws_sett->sl_walls = sl_list;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper list pointer.
 */
const SettList *
wallset_get_wallpaper_list (const WallSett *ws_sett)
{
    return (const SettList *) ws_sett->sl_walls;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set using last used wallpaper on start.
 */
void
wallset_set_last_used_setting (WallSett      *ws_sett,
                               const uint8_t  ui_val)
{
    ws_sett->i_lastsett = ui_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get using last used wallpaper on start.
 */
uint8_t
wallset_get_last_used_setting (const WallSett *ws_sett)
{
    return ws_sett->i_lastsett;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random wallpaper select value.
 */
void
wallset_set_random (WallSett      *ws_sett,
                    const uint8_t  ui_val)
{
    ws_sett->i_random = ui_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get random wallpaper select value.
 */
uint8_t
wallset_get_random (const WallSett *ws_sett)
{
    return ws_sett->i_random;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set wallpaper change interval value.
 */
void
wallset_set_interval (WallSett       *ws_sett,
                      const uint32_t  ui_val)
{
    ws_sett->i_chinterval = ui_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper change interval value.
 */
uint32_t
wallset_get_interval (const WallSett *ws_sett)
{
    return ws_sett->i_chinterval;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Change last used wallpaper file name in program settings. 
 */
void
wallset_set_last_used_fn (WallSett   *ws_sett,
                          const char *s_fn)
{
    ws_sett->s_lastused = s_fn;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get last used wallpaper file name.
 */
const char *
wallset_get_last_used_fn (const WallSett *ws_sett)
{
    return (const char*) ws_sett->s_lastused;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets command for setting wallpaper. 
 */
void
wallset_set_command (WallSett   *ws_sett,
                     const char *s_cmd)
{
    ws_sett->s_bgcmd = s_cmd;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper set command.
 */
const char *
wallset_get_command (const WallSett *ws_sett)
{
    return (const char*) ws_sett->s_bgcmd;
}
/*----------------------------------------------------------------------------*/

