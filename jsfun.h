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
 * @param[in]  s_fname  Config file name
 * @param[out] i_err    Pointer to integer for error output
 * @return     SettList list of settings
 */
/*----------------------------------------------------------------------------*/
SettList * js_settings_read (const char *s_fname,
                             int        *i_err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if settings in SettList are an update to settings
 *         stored in settings file.
 *
 * Function converts SettList list of settings to Json object, reads Json
 * data from file s_fname, inserts the converted data to read Json data
 * and finally checks if the changed Json data differs in content.
 * If there is no difference s_out_buff is set to null pointer, if they are
 * different, the new config Json raw data is written to s_out_buff.
 * Function returns ERR_OK if there is no error or appropriate error code.
 *
 * @param[in]  st_list     List of settings
 * @param[in]  s_fname     Config file name
 * @param[out] s_out_buff  Buffer to write data
 * @return     ERR_OK or error code
 */
int js_settings_check_for_update (SettList    *st_list,
                                  const char  *s_fname,
                                  char       **s_out_buff);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update file with new data
 *
 * @param[in]  s_buff   String with data to save
 * @param[in]  s_fname  File name to save data
 * @return     Saving file status, ERR_OK or error code
 */
int js_settings_update_file (const char *s_buff,
                             const char *s_fname);
/*----------------------------------------------------------------------------*/
/**
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
int js_settings_check_update_file (SettList   *st_list,
                                   const char *s_fname);
/*----------------------------------------------------------------------------*/
#endif

