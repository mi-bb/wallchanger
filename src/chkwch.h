/**
 * @file  chkwch.h
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
 * @brief  Check settings and wallpaper change functions.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef CHKWCH_H
#define CHKWCH_H

#include <stdint.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if display is present, exit if it is not.
 * @return none
 */
void check_display (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Loads settings, checks for changes in wallpaper list length and
 *         changes wallpaper, returns change interval.
 *
 * Function reads settings from json file to SettList list, converts it to
 * WallSett object to use with wallpaper change functions. It checks
 * wallpaper list length if it changed, reinitializes random number range if
 * needed and changes wallpaper.
 * Function returns wallpaper change interval to use with main loop.
 *
 * @return Wallpaper change interval
 */
uint32_t check_settings_change_wallpaper (char *s_cfg_file);
/*----------------------------------------------------------------------------*/
#endif

