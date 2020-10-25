/**
 * @file  jsof.c
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
 * @brief  Json open file function
 *
 * Functions to get and store application settings in JSON database.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include "hashfun.h"
#include "rwdt.h"
#include "errs.h"
#ifdef DEBUG
#include <stdio.h>
#endif
#include "jsof.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Open and check file with json data.
 */
json_object *
js_open_file (const char    *s_file_name,
              uint_fast32_t *ui_hash,
              int           *i_err)
{
    json_object   *j_obj;          /* Json object made from file data */
    char          *s_buff  = NULL; /* File data buffer */
    enum json_tokener_error j_err; /* Json error output */

    *i_err = ERR_OK;
    if (ui_hash != NULL)
        *ui_hash = 0;

    s_buff = read_file_data (s_file_name, i_err);
    //s_buff = ui_hash == NULL ?
    //         read_file_data (s_file_name, i_err) :
    //         read_file_data_hash (s_file_name, i_err, ui_hash);

    if (*i_err != ERR_OK && *i_err != ERR_FILE_EX) {
        free (s_buff);
        return NULL;
    }
    if (s_buff == NULL || s_buff[0] == '\0') {
        j_obj = json_object_new_object ();
    }
    else {
        j_obj = json_tokener_parse_verbose (s_buff, &j_err);
        if (j_obj == NULL ||
            json_object_get_type (j_obj) != json_type_object ||
            j_err != json_tokener_success) {
#ifdef DEBUG
            printf ("Json error: %d\n", j_err);
            printf ("Json type:  %d\n", json_object_get_type (j_obj));
            printf ("Error, wrong json file\n");
#endif
            if (j_obj != NULL)
                json_object_put (j_obj);
            j_obj = json_object_new_object ();
        }
    }
    if (ui_hash != NULL)
        *ui_hash = hash (json_object_to_json_string (j_obj));

    free (s_buff);
    return j_obj;
}
/*----------------------------------------------------------------------------*/

