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
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <err.h>
#include <stdlib.h>
#include "errs.h"
#include "strfun.h"
#include "fdfn.h"
#include "cfgfile.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   AUTOSTART_FILE_PATH
 * @brief Path for autostart desktop file for whcnagerd.
 *
 * @def   WM_JSON_FILE_PATH
 * @brief Path for config file with window manager info.
 */
#define AUTOSTART_FILE_PATH "/.config/autostart/wchangerd.desktop"
#define WM_JSON_FILE_PATH   "/.config/wchanger/wms.json"
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
    /* List of possible config paths in home directory */
    const char *s_cfg_files[] = {"/.config/wchanger/config.json",
                                 "/.config/wchanger.json",
                                 "/.config/wchanger/wchanger.json",
                                 NULL};
    int i_res = 0; /* Function result */

    enum cfg_state {
        C_STATE_END = 0,         /* End of checking */
        C_STATE_CHECK_CFG_NAME,  /* Check config name for null */
        C_STATE_SEARCH_DEFAULT,  /* Search for config file */
        C_STATE_CHECK_CFG_PERM,  /* Check config path permissions */
        C_STATE_CREATE_CFG_FILE, /* Create config file */
        C_STATE_WARN_RETURN_ERR  /* Show warning and end verifying */
    };

    enum cfg_state c_state = C_STATE_CHECK_CFG_NAME;

    while (c_state) {
        switch (c_state) {
            case C_STATE_CHECK_CFG_NAME: /* Checking config name */
                c_state = *s_file == NULL ?
                          C_STATE_SEARCH_DEFAULT :
                          C_STATE_CHECK_CFG_PERM;
                break;
            case C_STATE_SEARCH_DEFAULT: /* No file passed, serch for default */
                *s_file = cfgfile_find_config_file (s_cfg_files);
                if (*s_file == NULL && i_create) { /* No file found, create */
                    *s_file = dir_get_home ();
                    str_append (&(*s_file), s_cfg_files[0]);
                    c_state = C_STATE_CREATE_CFG_FILE;
                    break;
                }
                if (*s_file == NULL) {
                    i_res   = ERR_CFG_NOF;
                    c_state = C_STATE_WARN_RETURN_ERR;
                    break;
                }
                i_res   = ERR_OK;
                c_state = C_STATE_END;
                break;
            case C_STATE_CHECK_CFG_PERM: /* Config file passed, check it */
                i_res = file_check_permissions (*s_file);
                if (i_res == ERR_FILE_EX && i_create) { /* Try to create */
                    c_state = C_STATE_CREATE_CFG_FILE;
                    break;
                }
                else if (i_res != ERR_OK) { /* Other error */
                    c_state = C_STATE_WARN_RETURN_ERR;
                    break;
                }
                i_res   = ERR_OK;
                c_state = C_STATE_END;
                break;
            case C_STATE_CREATE_CFG_FILE: /* Try to create config file */
                if ((i_res = file_create_with_subdirs (*s_file)) != ERR_OK) {
                    c_state = C_STATE_WARN_RETURN_ERR;
                    break;
                }
                i_res   = ERR_OK;
                c_state = C_STATE_END;
                break;
            case C_STATE_WARN_RETURN_ERR: /* Warn, free and return error */
                warn ("%s", *s_file);
                free (*s_file);
                *s_file = NULL;
                c_state = C_STATE_END;
                break;
            case C_STATE_END:
                break;
            default:
                break;
        }
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get path for autostart desktop file for wchangerd daemon.
 */
char *
cfgfile_get_autostart_file_path (void)
{
    char *s_path = NULL; /* Config file path */

    s_path = dir_get_home ();
    str_append (&s_path, AUTOSTART_FILE_PATH);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get path for config file with window manager info.
 */
char *
cfgfile_get_wm_info_file_path (void)
{
    char *s_path = NULL; /* Config file path */

    s_path = dir_get_home ();
    str_append (&s_path, WM_JSON_FILE_PATH);

    return s_path;
}
/*----------------------------------------------------------------------------*/

