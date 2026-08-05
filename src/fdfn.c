/**
 * @file  fdfn.c
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
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <err.h>
#include <string.h>
#include "errs.h"
#include "strfun.h"
#include "fdfn.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static int check_permissions (const char *s_name, const int i_mode)
 *
 * @brief  Check permissions, existence of file or directory. 
 *
 * @param[in] s_name  Name of file / directory to check
 * @param[in] i_mode  Permissions to check
 * @return    Checking status
 *
 * @fn  static int dir_check_permissions_create (const char *s_dir)
 *
 * @brief  Check directory permissions, existence and create if needed. 
 *
 * @param[in] s_dir  Directory name to check / create
 * @return    Checking / creating status
 *
 * @fn  static int file_check_permissions_create (const char *s_file)
 *
 * @brief  Check file permissions, existence and maybe create it. 
 *
 * @param[in] s_file  Name of file to check / create
 * @return    Checking / creating status
 *
 * @fn  static char * dir_get_home (void)
 *
 * @brief  Get user's home dir path.
 *
 * @return  String with home path, after use it thould be freed using free
 */
/*----------------------------------------------------------------------------*/
static int    check_permissions             (const char *s_name,
                                             const int   i_mode)
                                             __attribute__ ((nonnull (1)));

static int    dir_check_permissions_create  (const char *s_dir)
                                             __attribute__ ((nonnull (1)));

static int    file_check_permissions_create (const char *s_file)
                                             __attribute__ ((nonnull (1)));

static char * dir_get_home                  (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check permissions, existence of file or directory. 
 */
static int
check_permissions (const char *s_name,
                   const int   i_mode)
{
    /* check if file/dir exists */
    if (access (s_name, F_OK) == 0) {
        /* check permissions */
        if (access (s_name, i_mode) != 0) {
            /* warn ("%s", s_name); */
            return ERR_FILE;
        }
        else {
            /* Permissions OK */
            return ERR_OK;
        }
    }
    else {
        /* warn ("%s", s_name); */
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
file_check_permissions (const char *s_file)
{
    return check_permissions (s_file, W_OK | R_OK);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check directory permissions (read, write, execute), existence. 
 */
int
dir_check_permissions (const char *s_dir)
{
    return check_permissions (s_dir, R_OK | X_OK);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check directory permissions, existence and create if needed. 
 */
static int
dir_check_permissions_create (const char *s_dir)
{
    int i_res  = ERR_OK; /* Error info */
    int i_res2 = 0;      /* Result permissions */

    i_res = dir_check_permissions (s_dir);

    /* Everything OK */
    if (i_res == ERR_OK) {
        return i_res;
    }
    /* If directory does not exist */
    else if (i_res == ERR_FILE_EX) {
        /* try to create it */
        i_res2 = mkdir (s_dir, 0700);

        if (i_res2 == 0) {
            return ERR_OK;
        }
        else {
            /* warn ("%s", s_dir); */
            return ERR_FILE_CR;
        }
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check file permissions, existence and maybe create it. 
 */
static int
file_check_permissions_create (const char *s_file)
{
    FILE *f_file;
    int   i_res = ERR_OK;

    i_res = file_check_permissions (s_file);

    /* Everything OK */
    if (i_res == ERR_OK) {
        return i_res;
    }
    /* If file does not exist */
    else if (i_res == ERR_FILE_EX) {
        /* try to create it */
        f_file = fopen (s_file, "a+");

        if (f_file == nullptr) {
            /* warn ("%s", s_file); */
            return ERR_FILE_CR;
        }
        else {
            /* file created */
            fclose (f_file);
            return ERR_OK;
        }
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's home dir path.
 */
static char *
dir_get_home (void)
{
    char *s_dir = nullptr;
    /* Getting user's HOME path */
    s_dir = getenv ("HOME");

    if (s_dir == nullptr || s_dir[0] == '\0') {
        s_dir = getpwuid (getuid ())->pw_dir;
    }
    return strdup (s_dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's home config path.
 */
char *
dir_get_home_config (void)
{
    char *s_dir = nullptr;
    /* Getting user's config path */
    s_dir = getenv ("XDG_CONFIG_HOME");

    if (s_dir == nullptr || s_dir[0] == '\0') {
        s_dir = dir_get_home ();
        str_append (&s_dir, "/.config");
        return s_dir;
    }
    return strdup (s_dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get temp dir path.
 */
char *
dir_get_temp (void)
{
    char *s_dir = nullptr;
    /* Getting temp path */
    s_dir = getenv ("TMPDIR");

    return (s_dir == nullptr || s_dir[0] == '\0') ?
        strdup ("/tmp") :
        strdup (s_dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's cache dir path.
 */
char *
dir_get_cache (void)
{
    char *s_dir = nullptr;
    /* Getting cache path */
    s_dir = getenv ("XDG_CACHE_HOME");

    if (s_dir == nullptr || s_dir[0] == '\0') {
        s_dir = dir_get_home ();
        str_append (&s_dir, "/.cache");
        return s_dir;
    }
    return strdup (s_dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's home data dir.
 */
char *
dir_get_home_data (void)
{
    char *s_dir = nullptr;
    /* Getting home data path */
    s_dir = getenv ("XDG_DATA_HOME");

    if (s_dir == nullptr || s_dir[0] == '\0') {
        s_dir = dir_get_home ();
        str_append (&s_dir, "/.local/share");
        return s_dir;
    }
    return strdup (s_dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get system data dir path.
 */
char *
dir_get_data (void)
{
    char *s_dir = nullptr;
    /* Getting system DATA path */
    s_dir = getenv ("XDG_DATA_DIRS");

    return (s_dir == nullptr || s_dir[0] == '\0') ?
        strdup ("/usr/local/share:/usr/share") :
        strdup (s_dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get user's autostart dir.
 */
char *
dir_get_autostart (void)
{
    char *s_dir = nullptr;

    s_dir = dir_get_home_config ();
    str_append (&s_dir, "/autostart");

    return s_dir;
}
/*----------------------------------------------------------------------------*/
int
dir_create_with_subdirs (const char *s_dn)
{
    char  *s_new  = nullptr; /* New string with dirs */
    char  *s_sls  = nullptr; /* / position */
    char  *s_dup  = nullptr; /* Duplicate of s_fn */
    char  *s_tmp  = nullptr; /* Pointer to duplicate of s_fn */
    int    i_err  = 0;    /* Error output */
    size_t ui_len = 0;    /* Name length */

    s_dup = strdup (s_dn);
    s_tmp = s_dup;

    /* Skip first / in file path */
    if (*s_tmp == '/')
        ++s_tmp;

    /* Find every / in path string */
    while ((s_sls = strchr (s_tmp, '/')) != nullptr) {
        /* Replace found / with null */
        *s_sls = '\0';
        str_append (&s_new, "/");
        /* Append directory to new string */
        str_append (&s_new, s_tmp);
        /* Check path permissions and try to create it if necessary */
        if ((i_err = dir_check_permissions_create (s_new)) != ERR_OK) {
            free (s_dup);
            free (s_new);
            warn ("%s", s_new);
            return i_err;
        }
        ui_len = (size_t) (s_sls - s_tmp);
        /* *s_sls = '/'; */
        s_tmp += ui_len + 1;
    }
    free (s_dup);
    free (s_new);
    if ((i_err = dir_check_permissions_create (s_dn)) != ERR_OK) {
        warn ("%s", s_dn);
        return i_err;
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create file with subfolders if it doesn't exist.
 */
int
file_create_with_subdirs (const char *s_fn)
{
    char  *s_new  = nullptr; /* New string with dirs */
    char  *s_sls  = nullptr; /* / position */
    char  *s_dup  = nullptr; /* Duplicate of s_fn */
    char  *s_tmp  = nullptr; /* Pointer to duplicate of s_fn */
    int    i_err  = 0;    /* Error output */
    size_t ui_len = 0;    /* Name length */

    s_dup = strdup (s_fn);
    s_tmp = s_dup;

    /* Skip first / in file path */
    if (*s_tmp == '/')
        ++s_tmp;

    /* Find every / in path string */
    while ((s_sls = strchr (s_tmp, '/')) != nullptr) {
        /* Replace found / with null */
        *s_sls = '\0';
        str_append (&s_new, "/");
        /* Append directory to new string */
        str_append (&s_new, s_tmp);
        /* Check path permissions and try to create it if necessary */
        if ((i_err = dir_check_permissions_create (s_new)) != ERR_OK) {
            free (s_dup);
            free (s_new);
            warn ("%s", s_new);
            return i_err;
        }
        ui_len = (size_t) (s_sls - s_tmp);
        /* *s_sls = '/'; */
        s_tmp += ui_len + 1;
    }
    free (s_dup);
    free (s_new);

    if ((i_err = file_check_permissions_create (s_fn)) != ERR_OK) {
        warn ("%s", s_fn);
        return i_err;
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/

