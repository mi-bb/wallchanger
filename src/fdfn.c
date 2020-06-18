/**
 * @file  fdfn.c
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
 * @fn  static int dir_check_permissions (const char *s_dir)
 *
 * @brief  Check directory permissions (read, write, execute), existence. 
 *
 * @param[in] s_dir  Directory name to check
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
 */
/*----------------------------------------------------------------------------*/
static int check_permissions             (const char *s_name,
                                          const int   i_mode)
                                          __attribute__ ((nonnull (1)));


static int dir_check_permissions         (const char *s_dir)
                                          __attribute__ ((nonnull (1)));

static int dir_check_permissions_create  (const char *s_dir)
                                          __attribute__ ((nonnull (1)));

static int file_check_permissions_create (const char *s_file)
                                          __attribute__ ((nonnull (1)));
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
static int
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
    int i_res  = ERR_OK;
    int i_res2 = 0;

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

        if (f_file == NULL) {
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
 * @brief  Get users home dir path.
 */
char *
dir_get_home (void)
{
    char *s_home = NULL;
    /* Getting user's HOME path */
    if ((s_home = getenv ("HOME")) == NULL) {
        s_home = getpwuid (getuid ())->pw_dir;
    }
    return strdup (s_home);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create file with subfolders if it doesn't exist.
 */
int
file_create_with_subdirs (const char *s_fn)
{
    char  *s_new  = NULL; /* New string with dirs */
    char  *s_sls  = NULL; /* / position */
    char  *s_dup  = NULL; /* Duplicate of s_fn */
    char  *s_tmp  = NULL; /* Pointer to duplicate of s_fn */
    int    i_err  = 0;    /* Error output */
    size_t ui_len = 0;    /* Name length */

    s_dup = strdup (s_fn);
    s_tmp = s_dup;

    /* Skip first / in file path */
    if (*s_tmp == '/')
        ++s_tmp;

    /* Find every / in path string */
    while ((s_sls = strchr (s_tmp, '/')) != NULL) {
        /* Replace found / with null */
        *s_sls = '\0';
        str_append (&s_new, "/");
        /* Append directory to new string */
        str_append (&s_new, s_tmp);
        /* Check path permissions and try to create it if necessary */
        if ((i_err = dir_check_permissions_create (s_new)) != ERR_OK) {
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
