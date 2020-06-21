/**
 * @file  deffiles.h
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
 * @brief  Default files create / delete ... functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef DEFFILES_H
#define DEFFILES_H
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd daemon desktop file exists in user's autostart
 *         directory.
 *
 * @return 1 if file exists, 0 if not or there was some problem with it.
 */
int deffiles_autostart_exists (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates wchangerd desktop file in user's autostart directory.
 *
 * @return ERR_OK if created successfully, other error value if not.
 */
int deffiles_autostart_create (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if exists and create window manager info json file.
 *
 * @return ERR_OK if created successfully, other error value if not.
 */
int deffiles_wm_check_create (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Removes wchangerd desktop file from user's autostart directory.
 *
 * @return ERR_OK if removed successfully, ERR_FILE_RM if not.
 */
int deffiles_autostart_remove (void);
/*----------------------------------------------------------------------------*/
char * deffiles_wm_get_buff (void);
const char * deffiles_wm_get_buff2 (void);
/*----------------------------------------------------------------------------*/
#endif

