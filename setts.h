/**
 * @file  setts.h
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
#ifndef SETTS_H
#define SETTS_H

#include "jsfun.h"
#include "settstr.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Reading settings status
 */
int settings_read (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Writting settings status
 */
int settings_write (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used wallpaper position in program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @return         Updating settings status
 */
int settings_update_last_used (WallSett *ws_sett);
/*----------------------------------------------------------------------------*/
int settings_update_window_size (WallSett *ws_sett,
                                 const int i_w, const int i_h);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are different that saved ones.
 *
 * @param[in,out]  ws_sett    Program settings
 * @param[out]     i_changed  Setting changed value, 1 if changed, 0 if not
 * @return         Checking settings status
 */
int settings_check_changed (WallSett *ws_sett, uint8_t *i_changed);
/*----------------------------------------------------------------------------*/
#endif

