/**
 * @file  fdfn.h
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
 * @brief  File / directory operations
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef FDFN_H
#define FDFN_H
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check file permissions (read write), existence. 
 *
 * @param[in] s_file  File name to check
 * @return    Checking status
 */
int    file_check_permissions (const char *s_file)
       __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check directory permissions, existence and create if needed. 
 *
 * @param[in] s_dir  Directory name to check / create
 * @return    Checking / creating status
 */
int    dir_check_permissions (const char *s_file)
       __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create file with subfolders if it doesn't exist.
 *
 * @param[in] s_fn  File path
 * @return    Process result
 */
int    file_create_with_subdirs (const char *s_fn)
       __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's home config path.
 *
 * @return  String with home config path, after use it thould be freed using
 *          free.
 */
char * dir_get_home_config (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get system data dir path.
 *
 * @return  String with system data path, after use it thould be freed using
 *          free.
 */
char * dir_get_data (void) __attribute__ ((malloc));
/*----------------------------------------------------------------------------*/
#endif

