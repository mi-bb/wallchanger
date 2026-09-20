/**
 * @file  json_file.c
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
#include "hash_djb2.h"
#include "rwdt.h"
#include "errors.h"
#ifdef DEBUG
#include <stdio.h>
#endif
#include "json_file.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Open and check file with json data.
 */
json_object *
json_open_file (const char    *file_name,
                uint_fast32_t *json_hash,
                int           *err)
{
    json_object   *j_obj;          /* Json object made from file data */
    char          *buff  = nullptr; /* File data buffer */
    enum json_tokener_error j_err; /* Json error output */

    *err = ERR_OK;
    if (json_hash != nullptr)
        *json_hash = 0;

    buff = read_file_data (file_name, err, nullptr);

    if (*err != ERR_OK && *err != ERR_FILE_EX) {
        free (buff);
        return nullptr;
    }
    if (buff == nullptr || buff[0] == '\0') {
        j_obj = json_object_new_object ();
    }
    else {
        j_obj = json_tokener_parse_verbose (buff, &j_err);
        if (j_obj == nullptr ||
            json_object_get_type (j_obj) != json_type_object ||
            j_err != json_tokener_success) {
#ifdef DEBUG
            printf ("Json error: %d\n", j_err);
            printf ("Json type:  %d\n", json_object_get_type (j_obj));
            printf ("Error, wrong json file\n");
#endif
            if (j_obj != nullptr)
                json_object_put (j_obj);
            j_obj = json_object_new_object ();
        }
    }
    if (json_hash != nullptr)
        *json_hash = hash (json_object_to_json_string (j_obj));

    free (buff);
    return j_obj;
}
/*----------------------------------------------------------------------------*/

