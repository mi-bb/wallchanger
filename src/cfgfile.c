/**
 * @file  cfgfile.c
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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <err.h>
#include <stdlib.h>
#include "errs.h"
#include "strfun.h"
#include "fdfn.h"
#include "cfgfile.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config file in paths from list.
 *
 * @param[in] s_cc  Null terminated list with config file paths
 * @return    Config file path string or null if config files were not found,
 *            after use it should be freed using free
 */
static char *
cfgfile_find_config_file (const char **s_cc)
{
    char *s_tmp  = NULL; /* Temp string */
    char *s_home = NULL; /* Home path string */

    s_home = dir_get_home ();

    while (*s_cc != NULL) {
        s_tmp = str_comb (s_home, *s_cc);
        if (file_check_permissions (s_tmp) == ERR_OK) {
            free (s_home);
            return s_tmp;
        }
        /* else {
            warn ("%s", *s_cc);
        } */
        free (s_tmp);
        ++s_cc;
    }
    free (s_home);
    return NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks config file existence, creates default if i_create is set.
 */
int
cfgfile_config_file_stuff (char **s_file,
                           int    i_create)
{
    const char *s_cfg_files[] = {"/.config/wchanger/config.json",
                                 "/.config/wchanger.json",
                                 "/.config/wchanger/wchanger.json",
                                 NULL};
    int i_res = 0; /* Function result */
    int i_st  = 1; /* Config process state */

    while (i_st) {
        switch (i_st) {
            case 1: /* Checking config name */
                i_st = *s_file == NULL ? 2 : 3;
                break;
            case 2: /* No file passed, serch for default */
                *s_file = cfgfile_find_config_file (s_cfg_files);
                if (*s_file == NULL && i_create) { /* No file found, create */
                    *s_file = dir_get_home ();
                    str_append (&(*s_file), s_cfg_files[0]);
                    i_st = 4;
                    break;
                }
                if (*s_file == NULL) {
                    return ERR_CFG_NOF;
                }
                return ERR_OK;
            case 3: /* Config file passed, check it */
                i_res = file_check_permissions (*s_file);
                if (i_res == ERR_FILE_EX && i_create) { /* Try to create */
                    i_st = 4;
                    break;
                }
                else if (i_res != ERR_OK) { /* Other error */
                    i_st = 5;
                    break;
                }
                return ERR_OK;
            case 4: /* Try to create config file */
                if ((i_res = file_create_with_subdirs (*s_file)) != ERR_OK) {
                    i_st = 5;
                    break;
                }
                return ERR_OK;
            case 5: /* Warn, free and return error */
                warn ("%s", *s_file);
                free (*s_file);
                *s_file = NULL;
                return i_res;
            default:
                break;
        }
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
