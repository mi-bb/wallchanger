/**
 * @file  cfgfile.h
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
 * @brief  Configuration file examine and set
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef CFGFILE_H
#define CFGFILE_H
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks config file existence, creates default if i_create is set.
 *
 * When s_file string is null, no config path is passed, default config file
 * locations are checked for config file existence. If it is present in one
 * if these locations, it is stored in s_file and function returns ERR_OK.
 * If no file was found function returns ERR_CFG_NOF or tries to create
 * config file in default location. Variable i_create defines if config
 * file should be created or not. 
 * When s_file is not null, it is checked for existence and permissions.
 * It will be created or not based on i_create value.
 * If everything went fine ERR_OK is returned and s_file contains path
 * to config file.
 *
 * @param[in,out] s_file   Config file path or null to use default
 * @param[in]     i_create Create config file if it doesn't exist
 */
int cfgfile_config_file_stuff (char **s_file,
                               int    i_create);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get path for autostart desktop file for wchangerd daemon.
 *
 * @return String with autostart file path.
 *         After use it should be freed using free.
 */
char * cfgfile_get_autostart_file_path (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get path for config file with window manager info.
 *
 * @return String with window manager info file path.
 *         After use it should be freed using free.
 */
char * cfgfile_get_wm_info_file_path (void);
/*----------------------------------------------------------------------------*/
#endif

