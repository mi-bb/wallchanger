/**
 * @file  jsfun.h
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
#ifndef JSFUN_H
#define JSFUN_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <json-c/json.h>
#include "settstr.h"
#include "miscfun.h"
#include "errs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get settings data from file.
 *
 * @param[out] ws_sett  Settings data to write to
 * @param[in]  s_fname  Name of file with settings
 * @return     Writting status
 */
int js_settings_read (WallSett *ws_sett, const char *s_fname);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Saving program settings data in file. 
 *
 * @param[in]  ws_sett   Program settings
 * @param[in]  s_fname  Settings file name
 * @return     Saving data status
 */
int js_settings_write (WallSett *ws_sett, const char *s_fname);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update last used wallpaper position in program settings. 
 *
 * @param[in,out]  ws_sett  Program settings
 * @param[in]      s_fname  Settings file name
 * @return         Updating data status
 */
int js_settings_update_last_used (WallSett *ws_sett, const char *s_fname);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update information about main window dimensions in config file.
 *
 * @param[in]  s_fname  Settings file name
 * @param[in]  i_w      window width
 * @param[in]  i_h      window height
 * @return     Updating data status
 */
int js_settings_update_window_size (const char *s_fname, const int i_w,
                                    const int i_h);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings are different that saved ones.
 *
 * @param[in,out]  ws_sett    Program settings
 * @param[out]     i_changed  Setting changed value, 1 if changed, 0 if not
 * @return         Checking settings status
 */
int js_settings_check_changed (WallSett *ws_sett, uint8_t *i_changed);
/*----------------------------------------------------------------------------*/
#endif

