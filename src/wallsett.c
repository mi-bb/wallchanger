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
#include <err.h>
#include "cfgfile.h"
#include "errs.h"
#include "wallsett.h"
#include "strfun.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets default program settings. 
 */
void
wallset_init (WallSett *ws_sett)
{
    ws_sett->i_chinterval = DEFAULT_INTERVAL_VAL; /* Wallpaper change interv. */
    ws_sett->i_random     = 0;      /* Random wallpaper change */
    ws_sett->i_lastsett   = 0;      /* Last used wallpaper setting */
    ws_sett->s_cfgfile    = NULL;   /* Configuration file path */
    ws_sett->s_lastused   = NULL;   /* Last used wallpaper file name */
    ws_sett->s_bgcmd      = NULL;   /* Background set command */
    ws_sett->sl_walls     = NULL;   /* Wallpaper list */
    ws_sett->rm_rand      = NULL;   /* Random mem pointer */
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new WallSett item.
 */
WallSett *
wallset_new (RandMem *rm_rand)
{
    WallSett *ws_sett;

    ws_sett = malloc (sizeof (WallSett));

    if (ws_sett == NULL) {
        err (EXIT_FAILURE, NULL);
    }
    wallset_init (ws_sett);
    ws_sett->rm_rand = rm_rand;

    return ws_sett;
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
    ws_sett->s_cfgfile  = NULL;
    ws_sett->rm_rand    = NULL;
    free (ws_sett);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets config file name.
 */
void
wallset_set_cfgfile (WallSett   *ws_sett,
                     const char *s_fn)
{
    ws_sett->s_cfgfile = s_fn;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get config file name.
 */
const char *
wallset_get_cfgfile (const WallSett *ws_sett)
{
    return ws_sett->s_cfgfile;
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
                               const int8_t   i_val)
{
    ws_sett->i_lastsett = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get using last used wallpaper on start.
 */
int8_t
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
                    const int8_t   i_val)
{
    ws_sett->i_random = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get random wallpaper select value.
 */
int8_t
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

