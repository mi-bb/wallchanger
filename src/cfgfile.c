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
#include <string.h>
#include <stdio.h>
#include "errs.h"
#include "strfun.h"
#include "fdfn.h"
#include "rwdt.h"
#include "cfgfile.h"
#include "defs.h"
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

    s_home = dir_get_home_config ();

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
 * @brief  Get application data dir.
 *
 * @return String application data dir or null.
 *         After use it should be freed using free.
 */
static char *
cfgfile_find_app_data_path (int *i_err)
{
    char  *s_dir  = NULL; /* App share path to return */
    char  *s_p    = NULL; /* Pointer to separator */
    char  *s_sh   = NULL; /* System share path */
    size_t ui_len = 0;    /* Length of path string */

    *i_err = 0;
    s_sh = dir_get_data ();

    while ((s_p = strchr (s_sh, ':')) != NULL) {
        *s_p++ = '\0';
        s_dir = str_comb (s_sh, PTH_SEP PTH_APP_SHORT);
        if ((*i_err = dir_check_permissions (s_dir)) == ERR_OK) {
            free (s_sh);
            return s_dir;
        }
        ui_len = strlen (s_p);
        memmove (s_sh, s_p, ui_len);
        s_sh[ui_len] = '\0';
        free (s_dir);
    }
    s_dir = str_comb (s_sh, PTH_SEP PTH_APP_SHORT);
    if ((*i_err = dir_check_permissions (s_dir)) == ERR_OK) {
        free (s_sh);
        return s_dir;
    }
    free (s_dir);
    free (s_sh);
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
    const char *s_cfg_files[] = {
        PTH_SEP PTH_APP_SHORT PTH_SEP PTH_CONFIG_FILE,
        PTH_SEP PTH_OLDCFG_FILE,
        PTH_SEP PTH_APP_SHORT PTH_SEP PTH_OLDCFG_FILE,
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
                    *s_file = dir_get_home_config ();
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
cfgfile_get_autostart_home_file_path (void)
{
    char *s_path = NULL; /* Config file path */

    s_path = dir_get_autostart ();
    str_append (&s_path, PTH_SEP PTH_ASTART_FILE_D);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get path for user's config file with window manager info.
 */
char *
cfgfile_get_wm_info_home_file_path (void)
{
    char *s_path = NULL; /* Config file path */

    s_path = dir_get_home_config ();
    str_append (&s_path, PTH_SEP PTH_APP_SHORT PTH_SEP PTH_WMINFO_FILE);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get path for default config file with window manager info.
 */
char *
cfgfile_get_wm_info_data_file_path (int *i_err)
{
    char *s_path = NULL; /* Config file path */

    *i_err = 0;

    if ((s_path = cfgfile_find_app_data_path (i_err)) == NULL)
        return NULL;

    str_append (&s_path, PTH_SEP PTH_WMINFO_FILE);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory path for downloaded wallpapers
 */
char *
cfgfile_get_app_wallpapers_path (void)
{
    char *s_path = NULL; /* Result directory path */

    s_path = dir_get_home_data ();
    str_append (&s_path, PTH_SEP PTH_APP_SHORT PTH_SEP PTH_WALLP_DIR);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get app cache directory.
 */
char *
cfgfile_get_app_cache_path (void)
{
    char *s_path = NULL; /* Result directory path */

    s_path = dir_get_cache ();
    str_append (&s_path, PTH_SEP PTH_APP_SHORT);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory path for thumbanils
 */
char *
cfgfile_get_app_thumbnails_path (void)
{
    char *s_path = NULL; /* Result directory path */

    s_path = dir_get_cache ();
    str_append (&s_path, PTH_SEP PTH_APP_SHORT PTH_SEP PTH_THUMB_DIR);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory with information about images.
 */
char *
cfgfile_get_image_info_path (void)
{
    char *s_path = NULL; /* Result directory path */

    s_path = dir_get_cache ();
    str_append (&s_path, PTH_SEP PTH_APP_SHORT PTH_SEP PTH_IINFO_DIR);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory with cached queries.
 */
char *
cfgfile_get_query_path (void)
{
    char *s_path = NULL; /* Result directory path */

    s_path = dir_get_cache ();
    str_append (&s_path, PTH_SEP PTH_APP_SHORT PTH_SEP PTH_QUERY_DIR);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd daemon desktop file exists in user's autostart
 *         directory.
 */
int
cfgfile_autostart_exists (void)
{
    char *s_path = NULL; /* Autostart file path */
    int  i_res   = 0;    /* File presence value to return */

    s_path = cfgfile_get_autostart_home_file_path ();

    if (file_check_permissions (s_path) == ERR_OK) {
        i_res = 1;
    }
    free (s_path);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates wchangerd desktop file in user's autostart directory.
 */
int
cfgfile_autostart_create (void)
{
    int   i_err     = ERR_OK; /* Error value to return */
    char *s_path    = NULL;   /* Autostart data file path */
    char *s_buff    = NULL;   /* Buffer for file content */
    char *s_as_path = NULL;   /* User's autostart path */

    if ((s_path = cfgfile_find_app_data_path (&i_err)) == NULL)
        return i_err;

    s_as_path = cfgfile_get_autostart_home_file_path ();

    str_append (&s_path, PTH_SEP PTH_ASTART_FILE_S);

    s_buff = read_file_data (s_path, &i_err, NULL);
    if (s_buff != NULL && i_err == ERR_OK) {
        if ((i_err = file_check_permissions (s_as_path)) == ERR_FILE_EX) {
            if ((i_err = file_create_with_subdirs (s_as_path)) == ERR_OK) {
                i_err = save_file_data (s_as_path, s_buff);
            }
        }
    }
    free (s_buff);
    free (s_path);
    free (s_as_path);
    return i_err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Removes wchangerd desktop file from user's autostart directory.
 */
int
cfgfile_autostart_remove (void)
{
    char *s_path = NULL; /* Autostart file path */

    s_path = cfgfile_get_autostart_home_file_path ();

    if (remove (s_path) != 0) {
        warn ("%s", s_path);
        free (s_path);
        return ERR_FILE_RM;
    }
    free (s_path);
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/

