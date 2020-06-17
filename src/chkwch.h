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
#include "randomm.h"
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
void print_now (void);
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count time to align intervals to next full hour.
 *
 * @return Time in seconds
 */
uint32_t check_time_align_val (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sleep for 500 milliseconds.
 *
 * @return none
 */
void sleep500 (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if display is present, exit if it is not.
 *
 * @return none
 */
void check_display_exit (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config files in standard locations, if s_file is not null
 *         check s_file path for existence. Exit on fail.
 *
 * @param[in,out]  s_file  Pointer to config file string variable
 * @return         none
 */
void check_config_file (char **s_file);
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
 * @param[in,out] s_cfg_file  Config file path
 * @param[in,out] rm_rand     RandMem object
 * @param[in,out] i_algntime  For time align option value
 * @return    Wallpaper change interval
 */
uint32_t check_settings_change_wallpaper (char     *s_cfg_file,
                                          RandMem  *rm_rand,
                                          int      *i_algntime);
/*----------------------------------------------------------------------------*/
#endif

