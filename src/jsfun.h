/**
 * @file  jsfun.h
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
 * @brief  Json functions
 *
 * Functions to get and store application settings in JSON database.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef JSFUN_H
#define JSFUN_H

#include "settlist.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get settings data from file.
 *
 * Reads settings from json file, returns SettList with settings or empty if
 * file is empty. Returns null if read error. Sets error code in i_err.
 *
 * @param[in]  s_fname  Config file name
 * @param[out] i_err    Pointer to integer for error output
 * @return     SettList list of settings
 */
/*----------------------------------------------------------------------------*/
SettList * js_settings_read (const char *s_fname,
                             int        *i_err)
                             __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
/**
 * @fn  char * js_settings_check_for_update (const SettList *st_list,
 *                                           const char     *s_fname,
 *                                           int            *i_err)
 * @brief  Check if settings in SettList are an update to settings
 *         stored in settings file.
 *
 * Function converts SettList list of settings to Json object, reads Json
 * data from file s_fname, and checks if they differ in content.
 * If there is no difference s_res_buff is set to null pointer and returned.
 * If they are different, the new config Json raw data is written to
 * s_res_buff and returned.
 * Function saves error code in i_err.
 *
 * @param[in]  st_list     List of settings
 * @param[in]  s_fname     Config file name
 * @param[out] i_err       Error output
 * @return     String with updated settings or null if there is no update.
 *
 * @fn  int js_settings_update_file (const char *s_buff, const char *s_fname)
 * @brief      Update file with new data
 * @param[in]  s_buff   String with data to save
 * @param[in]  s_fname  File name to save data
 * @return     Saving file status, ERR_OK or error code
 *
 * @fn  int js_settings_check_update_file (const SettList *st_list,
 *                                         const char     *s_fname)
 * @brief  Check if settings are an update and update file with new data
 *         if they are.
 *
 * Function runs js_settings_check_for_update and js_settings_update_file
 * if settings are an update.
 *
 * @param[in]  st_list  List of settings
 * @param[in]  s_fname  File name to save data
 * @return     Saving file status, ERR_OK or error code
 */
/*----------------------------------------------------------------------------*/
char * js_settings_check_for_update  (const SettList  *st_list,
                                      const char      *s_fname,
                                      int             *i_err)
                                      __attribute__ ((nonnull (2)));

int    js_settings_update_file       (const char      *s_buff,
                                      const char      *s_fname)
                                      __attribute__ ((nonnull (1, 2)));

int    js_settings_check_update_file (const SettList  *st_list,
                                      const char      *s_fname)
                                      __attribute__ ((nonnull (2)));
/*----------------------------------------------------------------------------*/
#endif

