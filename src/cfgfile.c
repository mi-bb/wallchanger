/**
 * @file  cfgfile.c
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
 * @brief  Configuration file examine and set
 *
 * @author Michal Babik <michal.babik@protonmail.com>
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
 * @return    Config file path string,
 *            after use it should be freed using free
 */
static char *
cfgfile_find_config_file (void)
{
    const char **s_cc = nullptr;
    char *s_tmp  = nullptr; /* Temp string */
    char *s_home = nullptr; /* Home path string */
    /* List of possible config paths in home directory */
    const char *s_cfg_files[] = {
        PTH_SEP PTH_APP_SHORT PTH_SEP PTH_CONFIG_FILE,
        PTH_SEP PTH_OLDCFG_FILE,
        PTH_SEP PTH_APP_SHORT PTH_SEP PTH_OLDCFG_FILE,
        nullptr};

    s_cc   = s_cfg_files;
    s_home = dir_get_home_config ();

    while (*s_cc != nullptr) {
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
    s_tmp = str_comb (s_home, s_cfg_files[0]);
    free (s_home);
    return s_tmp;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get application data dir.
 *
 * @param[out] i_err  Error output
 * @return String application data dir or null.
 *         After use it should be freed using free.
 */
static char *
cfgfile_find_app_data_path (int *i_err)
{
    char  *s_dir  = nullptr; /* App share path to return */
    char  *s_p    = nullptr; /* Pointer to separator */
    char  *s_sh   = nullptr; /* System share path */
    size_t ui_len = 0;    /* Length of path string */

    *i_err = 0;
    s_sh = dir_get_data ();

    while ((s_p = strchr (s_sh, ':')) != nullptr) {
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
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks config file existence, creates default if i_create is set.
 */
int
cfgfile_config_file_stuff (char **s_file,
                           int    i_create)
{
    int i_res = 0; /* Function result */

    if (*s_file == nullptr) {
        *s_file = cfgfile_find_config_file ();
    }
    i_res = file_check_permissions (*s_file);

    if (i_res == ERR_FILE_EX) {
        if (i_create) {
            i_res = file_create_with_subdirs (*s_file);
            }
        else {
            i_res = ERR_CFG_NOF;
        }
    }
    if (i_res != ERR_OK) {
        warn ("%s", *s_file);
        free (*s_file);
        *s_file = nullptr;
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
    char *s_path = nullptr; /* Config file path */

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
    char *s_path = nullptr; /* Config file path */

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
    char *s_path = nullptr; /* Config file path */

    *i_err = 0;

    if ((s_path = cfgfile_find_app_data_path (i_err)) == nullptr)
        return nullptr;

    str_append (&s_path, PTH_SEP PTH_WMINFO_FILE);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get application config path
 */
char *
cfgfile_get_app_config_path (void)
{
    char *s_path = nullptr; /* Result directory path */

    s_path = dir_get_home_config ();
    str_append (&s_path, PTH_SEP PTH_APP_SHORT);

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory path for downloaded wallpapers
 */
char *
cfgfile_get_app_wallpapers_path (void)
{
    char *s_path = nullptr; /* Result directory path */

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
    char *s_path = nullptr; /* Result directory path */

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
    char *s_path = nullptr; /* Result directory path */

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
    char *s_path = nullptr; /* Result directory path */

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
    char *s_path = nullptr; /* Result directory path */

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
    char *s_path = nullptr; /* Autostart file path */
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
    char *s_path    = nullptr;   /* Autostart data file path */
    char *s_buff    = nullptr;   /* Buffer for file content */
    char *s_as_path = nullptr;   /* User's autostart path */

    if ((s_path = cfgfile_find_app_data_path (&i_err)) == nullptr)
        return i_err;

    s_as_path = cfgfile_get_autostart_home_file_path ();

    str_append (&s_path, PTH_SEP PTH_ASTART_FILE_S);

    s_buff = read_file_data (s_path, &i_err, nullptr);
    if (s_buff != nullptr && i_err == ERR_OK) {
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
    char *s_path = nullptr; /* Autostart file path */

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
