/**
 * @file  json_functions.h
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
 * @brief  Json functions
 *
 * Functions to get and store application settings in JSON database.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef JSON_FUNCTIONS_H
#define JSON_FUNCTIONS_H

#include "setting_item.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get settings data from file.
 *
 * Reads settings from json file, returns SettList with settings or empty if
 * file is empty. Returns null if read error. Sets error code in err.
 *
 * @param[in]  fname  Config file name
 * @param[out] err    Pointer to integer for error output
 * @return     List of settings
 */
/*----------------------------------------------------------------------------*/
Setting *json_settings_read (const char *fname,
                             int        *err)
                             __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
/**
 * @fn  char * json_settings_check_for_update  (const Setting *settings,
   *                                            const char    *fname,
 *                                            int           *err)
 *
 * @brief  Check if settings in list are an update to settings stored in
 *         settings file.
 *
 * Function converts Setting list of settings to Json object, reads Json
 * data from file fname, and checks if they differ in content.
 * If there is no difference res_buff is set to null pointer and returned.
 * If they are different, the new config Json raw data is written to
 * res_buff and returned.
 * Function saves error code in err.
 *
 * @param[in]  settings List of Setting items
 * @param[in]  fname    Config file name
 * @param[out] err       Error output
 * @return     String with updated settings or null if there is no update.
 *
 * @fn  int json_settings_update_file (const char *buff, const char *fname)
 *
 * @brief  Update file with new data.
 *
 * @param[in]  buff   String with data to save
 * @param[in]  fname  File name to save data
 * @return     Saving file status, ERR_OK or error code
 *
 * @fn  int json_settings_check_update_file (Setting    *settings,
   *                                         const char *fname)
 *
 * @brief  Check if settings are an update to saved ones and update file with
 *         new data if they are.
 *
 * Function runs json_settings_check_for_update and json_settings_update_file
 * if settings are an update.
 *
 * @param[in]  settings  List of Setting items
 * @param[in]  fname     File name to save data
 * @return     Saving file status, ERR_OK or error code
 */
/*----------------------------------------------------------------------------*/
char *json_settings_check_for_update  (const Setting *settings,
                                       const char    *fname,
                                       int           *err)
                                       __attribute__ ((nonnull (2)));

int   json_settings_update_file       (const char    *buff,
                                       const char    *fname)
                                       __attribute__ ((nonnull (1, 2)));

int   json_settings_check_update_file (const Setting *settings,
                                       const char    *fname)
                                       __attribute__ ((nonnull (2)));
/*----------------------------------------------------------------------------*/
#endif

