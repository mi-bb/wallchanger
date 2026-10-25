/**
 * @file  wallpaper_change.h
 * @copyright Copyright (C) 2019-2026 Michał Bąbik
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
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef WALLPAPER_CHANGE_H
#define WALLPAPER_CHANGE_H

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
 * @param[in]      change_interval  Wallpaper change interval 
 * @param[in,out]  time_align_count Number of changes before next align count
 *
 * @return Sleep time in seconds
 */
uint32_t check_time_align_val (const uint32_t  change_interval,
                               uint32_t       *time_align_count);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if display is present, exit if it is not.
 *
 * @return none
 */
void check_display_exit (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config files in standard locations, if config_file is not null
 *         check config_file path for existence. Exit on fail.
 *
 * @param[in,out]  config_file  Pointer to config file string variable
 * @return         none
 */
void check_config_file (char **config_file);
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
 * @param[in,out] config_file  Config file path
 * @param[in,out] rand_mem     RandMem object
 * @param[in,out] time_align_option  For time align option value
 * @return    Wallpaper change interval
 */
uint32_t chk_setts_ch_wall (char     *config_file,
                            RandMem  *rand_mem,
                            int      *time_align_option);
/*----------------------------------------------------------------------------*/
#endif

