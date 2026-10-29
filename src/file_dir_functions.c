/**
 * @file  file_dir_functions.c
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
 * @brief  File / directory operations
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <err.h>
#include <string.h>
#include "errors.h"
#include "string_functions.h"
#include "file_dir_functions.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static int check_permissions (const char *name, const int mode)
 *
 * @brief  Check permissions, existence of file or directory. 
 *
 * @param[in] name  Name of file / directory to check
 * @param[in] mode  Permissions to check
 * @return    Checking status
 *
 * @fn  static int dir_check_permissions_create (const char *dir)
 *
 * @brief  Check directory permissions, existence and create if needed. 
 *
 * @param[in] dir  Directory name to check / create
 * @return    Checking / creating status
 *
 * @fn  static int file_check_permissions_create (const char *file)
 *
 * @brief  Check file permissions, existence and maybe create it. 
 *
 * @param[in] file  Name of file to check / create
 * @return    Checking / creating status
 *
 * @fn  static char * dir_get_home (void)
 *
 * @brief  Get user's home dir path.
 *
 * @return  String with home path, after use it thould be freed using free
 */
/*----------------------------------------------------------------------------*/
static int    check_permissions             (const char *name,
                                             const int   mode)
                                             __attribute__ ((nonnull (1)));

static int    dir_check_permissions_create  (const char *dir)
                                             __attribute__ ((nonnull (1)));

static int    file_check_permissions_create (const char *file)
                                             __attribute__ ((nonnull (1)));

static char * dir_get_home                  (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check permissions, existence of file or directory. 
 */
static int
check_permissions (const char *name,
                   const int   mode)
{
    /* check if file/dir exists */
    if (access (name, F_OK) == 0) {
        /* check permissions */
        if (access (name, mode) != 0) {
            /* warn ("%s", name); */
            return ERR_FILE;
        }
        else {
            /* Permissions OK */
            return ERR_OK;
        }
    }
    else {
        /* warn ("%s", name); */
        /* File/dir does not exist */
        return ERR_FILE_EX;
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check file permissions (read write), existence. 
 */
int
file_check_permissions (const char *file)
{
    return check_permissions (file, W_OK | R_OK);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check directory permissions (read, write, execute), existence. 
 */
int
dir_check_permissions (const char *dir)
{
    return check_permissions (dir, R_OK | X_OK);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check directory permissions, existence and create if needed. 
 */
static int
dir_check_permissions_create (const char *dir)
{
    int res  = ERR_OK; /* Error info */
    int res2 = 0;      /* Result permissions */

    res = dir_check_permissions (dir);

    /* Everything OK */
    if (res == ERR_OK) {
        return res;
    }
    /* If directory does not exist */
    else if (res == ERR_FILE_EX) {
        /* try to create it */
        res2 = mkdir (dir, 0700);

        if (res2 == 0) {
            return ERR_OK;
        }
        else {
            /* warn ("%s", dir); */
            return ERR_FILE_CR;
        }
    }
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check file permissions, existence and maybe create it. 
 */
static int
file_check_permissions_create (const char *file)
{
    FILE *stream;
    int   res = ERR_OK;

    res = file_check_permissions (file);

    /* Everything OK */
    if (res == ERR_OK) {
        return res;
    }
    /* If file does not exist */
    else if (res == ERR_FILE_EX) {
        /* try to create it */
        stream = fopen (file, "a+");

        if (stream == nullptr) {
            /* warn ("%s", file); */
            return ERR_FILE_CR;
        }
        else {
            /* file created */
            fclose (stream);
            return ERR_OK;
        }
    }
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's home dir path.
 */
static char *
dir_get_home (void)
{
    char *dir = nullptr;
    /* Getting user's HOME path */
    dir = getenv ("HOME");

    if (dir == nullptr || dir[0] == '\0') {
        dir = getpwuid (getuid ())->pw_dir;
    }
    return strdup (dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's home config path.
 */
char *
dir_get_home_config (void)
{
    char *dir = nullptr;
    /* Getting user's config path */
    dir = getenv ("XDG_CONFIG_HOME");

    if (dir == nullptr || dir[0] == '\0') {
        dir = dir_get_home ();
        str_append (&dir, "/.config");
        return dir;
    }
    return strdup (dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get temp dir path.
 */
char *
dir_get_temp (void)
{
    char *dir = nullptr;
    /* Getting temp path */
    dir = getenv ("TMPDIR");

    return (dir == nullptr || dir[0] == '\0') ?
        strdup ("/tmp") :
        strdup (dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's cache dir path.
 */
char *
dir_get_cache (void)
{
    char *dir = nullptr;
    /* Getting cache path */
    dir = getenv ("XDG_CACHE_HOME");

    if (dir == nullptr || dir[0] == '\0') {
        dir = dir_get_home ();
        str_append (&dir, "/.cache");
        return dir;
    }
    return strdup (dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's home data dir.
 */
char *
dir_get_home_data (void)
{
    char *dir = nullptr;
    /* Getting home data path */
    dir = getenv ("XDG_DATA_HOME");

    if (dir == nullptr || dir[0] == '\0') {
        dir = dir_get_home ();
        str_append (&dir, "/.local/share");
        return dir;
    }
    return strdup (dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get system data dir path.
 */
char *
dir_get_data (void)
{
    char *dir = nullptr;
    /* Getting system DATA path */
    dir = getenv ("XDG_DATA_DIRS");

    return (dir == nullptr || dir[0] == '\0') ?
        strdup ("/usr/local/share:/usr/share") :
        strdup (dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's autostart dir.
 */
char *
dir_get_autostart (void)
{
    char *dir = nullptr;

    dir = dir_get_home_config ();
    str_append (&dir, "/autostart");

    return dir;
}
/*----------------------------------------------------------------------------*/
int
dir_create_with_subdirs (const char *dir_path)
{
    char  *new_path = nullptr; /* New string with dirs */
    char  *slash    = nullptr; /* / position */
    char  *dup      = nullptr; /* Duplicate of dir_path */
    char  *tmp      = nullptr; /* Pointer to duplicate of dir_path */
    int    err      = 0;       /* Error output */
    size_t len      = 0;       /* Name length */

    dup = strdup (dir_path);
    tmp = dup;

    /* Skip first / in file path */
    if (*tmp == '/')
        ++tmp;

    /* Find every / in path string */
    while ((slash = strchr (tmp, '/')) != nullptr) {
        /* Replace found / with null */
        *slash = '\0';
        str_append (&new_path, "/");
        /* Append directory to new string */
        str_append (&new_path, tmp);
        /* Check path permissions and try to create it if necessary */
        if ((err = dir_check_permissions_create (new_path)) != ERR_OK) {
            free (dup);
            free (new_path);
            warn ("%s", new_path);
            return err;
        }
        len = (size_t) (slash - tmp);
        /* *slash = '/'; */
        tmp += len + 1;
    }
    free (dup);
    free (new_path);
    if ((err = dir_check_permissions_create (dir_path)) != ERR_OK) {
        warn ("%s", dir_path);
        return err;
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create file with subfolders if it doesn't exist.
 */
int
file_create_with_subdirs (const char *file_path)
{
    char  *new_path = nullptr; /* New string with dirs */
    char  *slash    = nullptr; /* / position */
    char  *dup      = nullptr; /* Duplicate of file_path */
    char  *tmp      = nullptr; /* Pointer to duplicate of file_path */
    int    err      = 0;       /* Error output */
    size_t len      = 0;       /* Name length */

    dup = strdup (file_path);
    tmp = dup;

    /* Skip first / in file path */
    if (*tmp == '/')
        ++tmp;

    /* Find every / in path string */
    while ((slash = strchr (tmp, '/')) != nullptr) {
        /* Replace found / with null */
        *slash = '\0';
        str_append (&new_path, "/");
        /* Append directory to new string */
        str_append (&new_path, tmp);
        /* Check path permissions and try to create it if necessary */
        if ((err = dir_check_permissions_create (new_path)) != ERR_OK) {
            free (dup);
            free (new_path);
            warn ("%s", new_path);
            return err;
        }
        len = (size_t) (slash - tmp);
        /* *slash = '/'; */
        tmp += len + 1;
    }
    free (dup);
    free (new_path);

    if ((err = file_check_permissions_create (file_path)) != ERR_OK) {
        warn ("%s", file_path);
        return err;
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
