/**
 * @file  jsof.h
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
 * @brief  Json open file function
 *
 * Functions to get and store application settings in JSON database.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef JSOF_H
#define JSOF_H
#include "config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Open and check file with json data.
 *
 * Opens json file, checks if it is a valid json file. If ui_hash pointer is
 * not null, file content hash is counted and set in ui_hash.
 * Function returns null on data read error. Sets error code in i_err.
 *
 * @param[in]  s_file_name  Config file name
 * @param[out] ui_hash      Pointer to integer for hash, it can be null.
 * @param[out] i_err        Pointer to integer for error output
 * @return     Json file with data, or empty when json file was not valid.
 */
json_object * js_open_file (const char    *s_file_name,
                            uint_fast32_t *ui_hash,
                            int           *i_err);
/*----------------------------------------------------------------------------*/
#endif

