/**
* @file  wallsett.h
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
#ifndef WALLSETT_H
#define WALLSETT_H

#include <stdint.h>
#include "randomm.h"
#include "settlist.h"
#include "defs.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct WallSett
 *
 * @brief  Structure with settings for wallpaper changing
 *
 * @var   WallSett::sl_walls
 * @brief SettList list with wallpaper file paths
 * @var   WallSett::s_bgcmd
 * @brief Command used to set wallpaper
 * @var   WallSett::s_lastused
 * @brief Last used wallpaper file path
 * @var   WallSett::rm_rand
 * @brief Structure used for selecting random numbers
 * @var   WallSett::s_cfgfile
 * @brief Configuration file path
 * @var   WallSett::i_chinterval
 * @brief Wallpaper change time interval
 * @var   WallSett::i_random
 * @brief Random wallpaper select option
 * @var   WallSett::i_lastsett
 * @brief Select last used wallpaper on start option
 */
typedef struct
WallSett {
    SettList   *sl_walls;      /* Wallpaper list */
    const char *s_bgcmd;       /* Background set command */
    const char *s_lastused;    /* Last used wallpaper path */
    const char *s_cfgfile;     /* Path to configuratoin file */
    RandMem    *rm_rand;        /* Random memory */
    uint32_t    i_chinterval;  /* Wallpaper change interval */
    int8_t      i_random;      /* Random wallpaper change */
    int8_t      i_lastsett;    /* Restore last used wallpeper */
} WallSett;
/*----------------------------------------------------------------------------*/
/**
 * @fn  int wallset_init (WallSett *ws_sett)
 *
 * @brief  Sets default program settings. 
 *
 * @param[out] ws_sett  Program settings
 * @return     none
 *
 * @fn  WallSett * wallset_new (void)
 *
 * @brief  Create new WallSett item.
 *
 * @param[in] rm_rand  RandMem object
 * @return    New WallSett item
 *
 * @fn  void wallset_free (WallSett *ws_sett)
 *
 * @brief  Free allocated memory.
 *
 * @param[out] ws_sett  Pointer to WallSett with all settings
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void       wallset_init (WallSett *ws_sett);

WallSett * wallset_new  (RandMem *rm_rand) __attribute__ ((returns_nonnull));

void       wallset_free (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int wallset_set_cfgfile (WallSett *ws_sett, const char *s_fn)
 *
 * @brief  Sets config file name.
 *
 * @param[out] ws_sett Program settings
 * @param[in]  s_fn    String with config file path or NULL.
 * @return     Setting name status.
 *
 * @fn  const char * wallset_get_cfgfile (const WallSett *ws_sett)
 *
 * @brief  Get config file name.
 *
 * @param[in] ws_sett  WallSett object
 * @return    Config file name
 *
 * @fn  void wallset_set_wallpaper_list (WallSett *ws_sett,
 *                                       SettList *sl_list)
 *
 * @brief  Sets wallpaper list value.
 *
 * @param[out] ws_sett  Program settings
 * @param[in]  sl_list  List of wallpapers to set
 * @return     none
 *
 * @fn  const SettList * wallset_get_wallpaper_list (WallSett *ws_sett)
 *
 * @brief  Get wallpaper list pointer.
 *
 * @param[in]  ws_sett  Program settings
 * @return     Pointer to wallpaper list
 *
 * @fn  void wallset_set_last_used_setting (WallSett      *ws_sett,
 *                                          const int8_t  i_val)
 *
 * @brief  Set using last used wallpaper on start.
 *
 * @param[out] ws_sett  WallSett object
 * @param[in]  i_val    Set last used wallpaper value
 * @return     none
 *
 * @fn  int8_t wallset_get_last_used_setting (const WallSett *ws_sett)
 *
 * @brief  Get using last used wallpaper on start.
 *
 * @param[in] ws_sett  WallSett object
 * @return    Last used setting
 *
 * @fn  void wallset_set_random (WallSett *ws_sett, const int8_t i_val)
 *
 * @brief  Set random wallpaper select value.
 *
 * @param[out] ws_sett  WallSett object
 * @param[in]  i_val    Random wallpaper setting to set
 * @return     none
 *
 * @fn  int8_t wallset_get_random (const WallSett *ws_sett)
 *
 * @brief  Get random wallpaper value.
 *
 * @param[in] ws_sett  WallSett object
 * @return    Random wallpaper setting value
 *
 * @fn  void wallset_set_interval (WallSett *ws_sett, const uint32_t ui_val)
 *
 * @brief  Set wallpaper change interval value.
 *
 * @param[out] ws_sett  WallSett object
 * @param[in]  ui_val   Wallpaper change interval value to set
 * @return     none
 *
 * @fn  uint32_t wallset_get_interval (const WallSett *ws_sett)
 *
 * @brief  Get wallpaper change interval value.
 *
 * @param[in] ws_sett  WallSett object
 * @return    Wallpaper change interval value
 * @fn  int wallset_set_last_used_fn (WallSett *ws_sett, const char *s_fn)
 *
 * @fn  void wallset_set_last_used_fn (WallSett *ws_sett, const char *s_fn)
 *
 * @brief  Change last used wallpaper file name in program settings. 
 *
 * @param[in,out] ws_sett  Program settings
 * @param[in]     s_fn     Wallpaper file path
 * @return        Wallpaper set status
 *
 * @fn  const char * wallset_get_last_used_fn (const WallSett *ws_sett)
 *
 * @brief  Get last used wallpaper file name.
 *
 * @param[in] ws_sett  WallSett object
 * @return    Last used wallpaper file name
 *
 * @fn  int wallset_set_command (WallSett *ws_sett, const char *s_cmd)
 *
 * @brief  Sets command for setting wallpaper. 
 *
 * @param[out] ws_sett  Program settings
 * @param[in]  s_cmd    New set wallpaper command
 * @return     Change status
 *
 * @fn  const char * wallset_get_command (const WallSett *ws_sett)
 *
 * @brief  Get wallpaper set command.
 *
 * @param[in] ws_sett  WallSett object
 * @return    Wallpaper set command
 *
 */
/*----------------------------------------------------------------------------*/
void             wallset_set_cfgfile           (WallSett       *ws_sett,
                                                const char     *s_fn);

const char *     wallset_get_cfgfile           (const WallSett *ws_sett)
                                                __attribute__((const));
/*----------------------------------------------------------------------------*/
void             wallset_set_wallpaper_list    (WallSett       *ws_sett,
                                                SettList       *sl_list);

const SettList * wallset_get_wallpaper_list    (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
void             wallset_set_last_used_setting (WallSett       *ws_sett,
                                                const int8_t    i_val);

int8_t           wallset_get_last_used_setting (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
void             wallset_set_random            (WallSett       *ws_sett,
                                                const int8_t    i_val);

int8_t           wallset_get_random            (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
void             wallset_set_interval          (WallSett       *ws_sett,
                                                const uint32_t  ui_val);

uint32_t         wallset_get_interval          (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
void             wallset_set_last_used_fn      (WallSett       *ws_sett,
                                                const char     *s_fn)
                                                __attribute__ ((nonnull (2)));

const char *     wallset_get_last_used_fn      (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
void             wallset_set_command           (WallSett       *ws_sett,
                                                const char     *s_cmd)
                                                __attribute__ ((nonnull (2)));

const char *     wallset_get_command           (const WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
#endif

