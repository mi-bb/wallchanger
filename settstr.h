/**
* @file  settstr.h
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
#ifndef SETTSTR_H
#define SETTSTR_H

#include <stdint.h>
#include "flist.h"
#include "randomm.h"
#include "miscfun.h"
#include "errs.h"
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
    int32_t   i_lastused;    /**< Last used wallpaper index */
} WallSett;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets default program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Setting status
 */
int wallset_init (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[out]  ws_sett  Pointer to WallSett with all settings
 * @return      none
 */
void wallset_free (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get config file name.
 *
 * @param[in]  ws_sett  WallSett object
 * @return     Config file name
 */
const char * wallset_get_cfg_fn (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set last used wallpaper position on list.
 *
 * @param[out]  ws_sett  WallSett object
 * @param[in]   ui_val   Last used position value to set
 * @return      none
 */
void wallset_set_last_used_pos (WallSett *ws_sett,
                                int32_t   ui_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get last used position value.
 *
 * @param[in]  ws_sett  WallSett object
 * @return     Last used position value
 */
int32_t wallset_get_last_used_pos (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set using last used wallpaper on start.
 *
 * @param[out]  ws_sett  WallSett object
 * @param[in]   ui_val   Set last used wallpaper value
 * @return      none
 */
void wallset_set_last_used_setting (WallSett *ws_sett,
                                    uint8_t   ui_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get using last used wallpaper on start.
 *
 * @param[in]  ws_sett  WallSett object
 * @return     Last used setting
 */
uint8_t wallset_get_last_used_setting (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random wallpaper select value.
 *
 * @param[out]  ws_sett  WallSett object
 * @param[in]   ui_val   Random wallpaper setting to set
 * @return      none
 */
void wallset_set_random (WallSett *ws_sett,
                         uint8_t   ui_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get random wallpaper select value.
 *
 * @param[in]  ws_sett  WallSett object
 * @return     Random wallpaper setting value
 */
uint8_t wallset_get_random (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set wallpaper change interval value.
 *
 * @param[out]  ws_sett  WallSett object
 * @param[in]   ui_val   Wallpaper change interval value to set
 * @return      none
 */
void wallset_set_interval (WallSett *ws_sett,
                           uint32_t  ui_val);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper change interval value.
 *
 * @param[in]  ws_sett  WallSett object
 * @return     Wallpaper change interval value
 */
uint32_t wallset_get_interval (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Change last used wallpaper file name in program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @param[in]      s_fn     Wallpaper file path
 * @return         Wallpaper set status
 */
int wallset_set_last_used_fn (WallSett   *ws_sett,
                              const char *s_fn);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get last used wallpaper file name.
 *
 * @param[in]  ws_sett  WallSett object
 * @return     Last used wallpaper file name
 */
const char * wallset_get_last_used_fn (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets command for setting wallpaper. 
 *
 * @param[out]  ws_sett  Program settings
 * @param[in]   s_cmd    New set wallpaper command
 * @return      Change status
 */
int wallset_set_command (WallSett   *ws_sett,
                         const char *s_cmd);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper set command.
 *
 * @param[in]  ws_sett  WallSett object
 * @return     Wallpaper set command
 */
const char * wallset_get_command (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
#endif

