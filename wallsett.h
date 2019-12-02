/**
* @file  wallsett.h
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
#ifndef WALLSETT_H
#define WALLSETT_H

#include <stdint.h>
#include "flist.h"
#include "randomm.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Structore with settings for wallpaper changing
 */
typedef struct
WallSett {
    FList     fl_files;      /**< Background file list */
    RandMem   rm_mem;        /**< Random memory */
    char     *s_bgcmd;       /**< Background set command */
    char     *s_lastused;    /**< Last used wallpaper path */
    char     *s_cfgfile;     /**< Path to configuratoin file */
    uint8_t   i_random;      /**< Random background change */
    uint32_t  i_chinterval;  /**< Background change interval */
    uint8_t   i_lastsett;    /**< Remember last used wallpeper */
} WallSett;
/*----------------------------------------------------------------------------*/
/**
 * @fn  int wallset_init (WallSett *ws_sett)
 * @brief         Sets default program settings. 
 * @param[in,out] ws_sett  Program settings
 * @return        Setting status
 *
 * @fn  void wallset_free (WallSett *ws_sett)
 * @brief      Free allocated memory.
 * @param[out] ws_sett  Pointer to WallSett with all settings
 * @return     none
 */
/*----------------------------------------------------------------------------*/
int  wallset_init (WallSett *ws_sett);

void wallset_free (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void wallset_set_last_used_setting (WallSett      *ws_sett,
 *                                          const uint8_t  ui_val)
 * @brief      Set using last used wallpaper on start.
 * @param[out] ws_sett  WallSett object
 * @param[in]  ui_val   Set last used wallpaper value
 * @return     none
 *
 * @fn  uint8_t wallset_get_last_used_setting (const WallSett *ws_sett)
 * @brief     Get using last used wallpaper on start.
 * @param[in] ws_sett  WallSett object
 * @return    Last used setting
 *
 * @fn  void wallset_set_random (WallSett *ws_sett, const uint8_t  ui_val)
 * @brief      Set random wallpaper select value.
 * @param[out] ws_sett  WallSett object
 * @param[in]  ui_val   Random wallpaper setting to set
 * @return     none
 *
 * @fn  uint8_t wallset_get_random (const WallSett *ws_sett)
 * @brief     Get random wallpaper value.
 * @param[in] ws_sett  WallSett object
 * @return    Random wallpaper setting value
 *
 * @fn  void wallset_set_interval (WallSett *ws_sett, const uint32_t ui_val)
 * @brief      Set wallpaper change interval value.
 * @param[out] ws_sett  WallSett object
 * @param[in]  ui_val   Wallpaper change interval value to set
 * @return     none
 *
 * @fn  uint32_t wallset_get_interval (const WallSett *ws_sett)
 * @brief     Get wallpaper change interval value.
 * @param[in] ws_sett  WallSett object
 * @return    Wallpaper change interval value
 * @fn  int wallset_set_last_used_fn (WallSett *ws_sett, const char *s_fn)
 *
 * @brief         Change last used wallpaper file name in program settings. 
 * @param[in,out] ws_sett  Program settings
 * @param[in]     s_fn     Wallpaper file path
 * @return        Wallpaper set status
 *
 * @fn  const char * wallset_get_last_used_fn (const WallSett *ws_sett)
 * @brief     Get last used wallpaper file name.
 * @param[in] ws_sett  WallSett object
 * @return    Last used wallpaper file name
 *
 * @fn  int wallset_set_command (WallSett *ws_sett, const char *s_cmd)
 * @brief      Sets command for setting wallpaper. 
 * @param[out] ws_sett  Program settings
 * @param[in]  s_cmd    New set wallpaper command
 * @return     Change status
 *
 * @fn  const char * wallset_get_command (const WallSett *ws_sett)
 * @brief     Get wallpaper set command.
 * @param[in] ws_sett  WallSett object
 * @return    Wallpaper set command
 *
 * @fn  const char * wallset_get_cfg_fn (const WallSett *ws_sett)
 * @brief     Get config file name.
 * @param[in] ws_sett  WallSett object
 * @return    Config file name
 */
/*----------------------------------------------------------------------------*/
const char * wallset_get_cfg_fn            (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
void         wallset_set_last_used_setting (WallSett       *ws_sett,
                                            const uint8_t   ui_val);

uint8_t      wallset_get_last_used_setting (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
void         wallset_set_random            (WallSett       *ws_sett,
                                            const uint8_t   ui_val);

uint8_t      wallset_get_random            (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
void         wallset_set_interval          (WallSett       *ws_sett,
                                            const uint32_t  ui_val);

uint32_t     wallset_get_interval          (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
int          wallset_set_last_used_fn      (WallSett       *ws_sett,
                                            const char     *s_fn);

const char * wallset_get_last_used_fn      (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
int          wallset_set_command           (WallSett       *ws_sett,
                                            const char     *s_cmd);

const char * wallset_get_command           (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
#endif

