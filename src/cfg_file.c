/**
 * @file  cfg_file.c
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
#include "errors.h"
#include "string_functions.h"
#include "file_dir_functions.h"
#include "file_io.h"
#include "cfg_file.h"
#include "defs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config file in paths from list.
 *
 * @return    Config file path string,
 *            after use it should be freed using free
 */
static char *
cfg_file_find_config_file (void)
{
    const char **paths = nullptr;
    char *tmp  = nullptr; /* Temp string */
    char *home = nullptr; /* Home path string */
    /* List of possible config paths in home directory */
    const char *cfg_files[] = {
        PTH_SEP PTH_APP_SHORT PTH_SEP PTH_CONFIG_FILE,
        PTH_SEP PTH_OLDCFG_FILE,
        PTH_SEP PTH_APP_SHORT PTH_SEP PTH_OLDCFG_FILE,
        nullptr};

    paths = cfg_files;
    home  = dir_get_home_config ();

    while (*paths != nullptr) {
        tmp = str_comb (home, *paths);
        if (file_check_permissions (tmp) == ERR_OK) {
            free (home);
            return tmp;
        }
        /* else {
            warn ("%s", *paths);
        } */
        free (tmp);
        ++paths;
    }
    tmp = str_comb (home, cfg_files[0]);
    free (home);
    return tmp;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get application data dir.
 *
 * @param[out] err  Error output
 * @return String application data dir or null.
 *         After use it should be freed using free.
 */
static char *
cfg_file_find_app_data_path (int *err)
{
    char  *dir = nullptr; /* App share path to return */
    char  *sep = nullptr; /* Pointer to separator */
    char  *sh  = nullptr; /* System share path */
    size_t len = 0;       /* Length of path string */

    *err = 0;
    sh = dir_get_data ();

    while ((sep = strchr (sh, ':')) != nullptr) {
        *sep++ = '\0';
        dir = str_comb (sh, PTH_SEP PTH_APP_SHORT);
        if ((*err = dir_check_permissions (dir)) == ERR_OK) {
            free (sh);
            return dir;
        }
        len = strlen (sep);
        memmove (sh, sep, len);
        sh[len] = '\0';
        free (dir);
    }
    dir = str_comb (sh, PTH_SEP PTH_APP_SHORT);
    if ((*err = dir_check_permissions (dir)) == ERR_OK) {
        free (sh);
        return dir;
    }
    free (dir);
    free (sh);
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks config file existence, creates default if create is set.
 */
int
cfg_file_config_file_stuff (char **file,
                            int    create)
{
    int res = 0; /* Function result */

    if (*file == nullptr) {
        *file = cfg_file_find_config_file ();
    }
    res = file_check_permissions (*file);

    if (res == ERR_FILE_EX) {
        if (create) {
            res = file_create_with_subdirs (*file);
        }
        else {
            res = ERR_CFG_NOF;
        }
    }
    if (res != ERR_OK) {
        warn ("%s", *file);
        free (*file);
        *file = nullptr;
    }
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get path for autostart desktop file for wchangerd daemon.
 */
char *
cfg_file_get_autostart_home_file_path (void)
{
    char *path = nullptr; /* Config file path */

    path = dir_get_autostart ();
    str_append (&path, PTH_SEP PTH_ASTART_FILE_D);

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get path for user's config file with window manager info.
 */
char *
cfg_file_get_wm_info_home_file_path (void)
{
    char *path = nullptr; /* Config file path */

    path = dir_get_home_config ();
    str_append (&path, PTH_SEP PTH_APP_SHORT PTH_SEP PTH_WMINFO_FILE);

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get path for default config file with window manager info.
 */
char *
cfg_file_get_wm_info_data_file_path (int *err)
{
    char *path = nullptr; /* Config file path */

    *err = 0;

    if ((path = cfg_file_find_app_data_path (err)) == nullptr)
        return nullptr;

    str_append (&path, PTH_SEP PTH_WMINFO_FILE);

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get application config path
 */
char *
cfg_file_get_app_config_path (void)
{
    char *path = nullptr; /* Result directory path */

    path = dir_get_home_config ();
    str_append (&path, PTH_SEP PTH_APP_SHORT);

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory path for downloaded wallpapers
 */
char *
cfg_file_get_app_wallpapers_path (void)
{
    char *path = nullptr; /* Result directory path */

    path = dir_get_home_data ();
    str_append (&path, PTH_SEP PTH_APP_SHORT PTH_SEP PTH_WALLP_DIR);

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get app cache directory.
 */
char *
cfg_file_get_app_cache_path (void)
{
    char *path = nullptr; /* Result directory path */

    path = dir_get_cache ();
    str_append (&path, PTH_SEP PTH_APP_SHORT);

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory path for thumbanils
 */
char *
cfg_file_get_app_thumbnails_path (void)
{
    char *path = nullptr; /* Result directory path */

    path = dir_get_cache ();
    str_append (&path, PTH_SEP PTH_APP_SHORT PTH_SEP PTH_THUMB_DIR);

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory with information about images.
 */
char *
cfg_file_get_image_info_path (void)
{
    char *path = nullptr; /* Result directory path */

    path = dir_get_cache ();
    str_append (&path, PTH_SEP PTH_APP_SHORT PTH_SEP PTH_IINFO_DIR);

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory with cached queries.
 */
char *
cfg_file_get_query_path (void)
{
    char *path = nullptr; /* Result directory path */

    path = dir_get_cache ();
    str_append (&path, PTH_SEP PTH_APP_SHORT PTH_SEP PTH_QUERY_DIR);

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd daemon desktop file exists in user's autostart
 *         directory.
 */
bool
cfg_file_autostart_exists (void)
{
    char *path = nullptr; /* Autostart file path */
    bool  res  = false;   /* File presence value to return */

    path = cfg_file_get_autostart_home_file_path ();

    if (file_check_permissions (path) == ERR_OK) {
        res = true;
    }
    free (path);

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates wchangerd desktop file in user's autostart directory.
 */
int
cfg_file_autostart_create (void)
{
    int   err          = ERR_OK; /* Error value to return */
    char *path         = nullptr;   /* Autostart data file path */
    char *buff         = nullptr;   /* Buffer for file content */
    char *autostart_path = nullptr; /* User's autostart path */

    if ((path = cfg_file_find_app_data_path (&err)) == nullptr)
        return err;

    autostart_path = cfg_file_get_autostart_home_file_path ();

    str_append (&path, PTH_SEP PTH_ASTART_FILE_S);

    buff = read_file_data (path, &err, nullptr);
    if (buff != nullptr && err == ERR_OK) {
        if ((err = file_check_permissions (autostart_path)) == ERR_FILE_EX) {
            if ((err = file_create_with_subdirs (autostart_path)) == ERR_OK) {
                err = save_file_data (autostart_path, buff);
            }
        }
    }
    free (buff);
    free (path);
    free (autostart_path);
    return err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Removes wchangerd desktop file from user's autostart directory.
 */
int
cfg_file_autostart_remove (void)
{
    char *path = nullptr; /* Autostart file path */

    path = cfg_file_get_autostart_home_file_path ();

    if (remove (path) != 0) {
        warn ("%s", path);
        free (path);
        return ERR_FILE_RM;
    }
    free (path);
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
