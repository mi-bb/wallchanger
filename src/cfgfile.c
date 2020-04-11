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
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include "errs.h"
#include "cres.h"
#include "cfgfile.h"
#include "defs.h"
#include "strfun.h"
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
 * @fn  static int check_file_permissions (const char *s_file)
 *
 * @brief  Check file permissions (read write), existence. 
 *
 * @param[in] s_file  File name to check
 * @return    Checking status
 *
 * @fn  static int check_dir_permissions (const char *s_dir)
 *
 * @brief  Check directory permissions (read, write, execute), existence. 
 *
 * @param[in] s_dir  Directory name to check
 * @return    Checking status
 *
 * @fn  static int check_dir_premissions_create (const char *s_dir)
 *
 * @brief  Check directory permissions, existence and create if needed. 
 *
 * @param[in] s_dir  Directory name to check / create
 * @return    Checking / creating status
 *
 * @fn  static int check_file_premissions_create (const char *s_file)
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

static int check_file_permissions        (const char *s_file)
                                          __attribute__ ((nonnull (1)));

static int check_dir_permissions         (const char *s_dir)
                                          __attribute__ ((nonnull (1)));

static int check_dir_premissions_create  (const char *s_dir)
                                          __attribute__ ((nonnull (1)));

static int check_file_premissions_create (const char *s_file)
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
            warn ("%s", s_name);
            return ERR_FILE;
        }
        else {
            /* Permissions OK */
            return ERR_OK;
        }
    }
    else {
        /* File/dir does not exist */
        return ERR_FILE_EX;
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check file permissions (read write), existence. 
 */
static int
check_file_permissions (const char *s_file)
{
    return check_permissions (s_file, W_OK | R_OK);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check directory permissions (read, write, execute), existence. 
 */
static int
check_dir_permissions (const char *s_dir)
{
    return check_permissions (s_dir, W_OK | R_OK | X_OK);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check directory permissions, existence and create if needed. 
 */
static int
check_dir_premissions_create (const char *s_dir)
{
    int i_res  = ERR_OK;
    int i_res2 = 0;

    i_res = check_dir_permissions (s_dir);

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
            warn ("%s", s_dir);
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
check_file_premissions_create (const char *s_file)
{
    FILE *f_file;
    int   i_res = ERR_OK;

    i_res = check_file_permissions (s_file);

    /* Everything OK */
    if (i_res == ERR_OK) {
        return i_res;
    }
    /* If file does not exist */
    else if (i_res == ERR_FILE_EX) {
        /* try to create it */
        f_file = fopen(s_file, "a+");

        if (f_file == NULL) {
            warn ("%s", s_file);
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
 * @brief  Check if config file and path exists, check read/write permissions. 
 */
char *
cfgfile_get_config_path (int *i_err)
{
    const char *s_sett = "wchanger.json"; /* Settings file name */
    const char *s_cfg  = "/.config/";     /* Settings file path */
    char       *s_path = NULL;            /* Result full config file name */
    char       *s_home = NULL;            /* Home path */
    size_t      ul_len = 0;               /* Config path length */

    /* Getting user's HOME path */
    if ((s_home = getenv ("HOME")) == NULL) {
        s_home = getpwuid (getuid ())->pw_dir;
    }

    /* Total config file path and name length */
    ul_len = strlen (s_home) + strlen (s_cfg) + strlen (s_sett);
    #ifdef DEBUG
    ul_len += 4;
    #endif

    /* Create string for config file path and name */
    cres ((void**) &s_path, (ul_len + 1), sizeof (char));
    //s_path[0] = '\0';

    /* Copy config file path */
    strcpy (s_path, s_home);
    strcat (s_path, s_cfg);

    /* Check config path existence and permissions,
     * create directory if needed */
    *i_err = check_dir_premissions_create (s_path);

    if (*i_err != ERR_OK) {
        free (s_path);
        return NULL;
    }

    /* Append file name to config path */
    strcat (s_path, s_sett);
    #ifdef DEBUG
    strcat (s_path, ".dbg");
    #endif

    /* Check config file existence and permissions,
     * create file if needed */
    *i_err = check_file_premissions_create (s_path);

    if (*i_err != ERR_OK) {
        free (s_path);
        return NULL;
    }

    #ifdef DEBUG
    printf ("Config path : %s\n", s_path);
    #endif

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get config file path or exit app if errors occurred.
 */
char *
cfgfile_get_config_path_exit (void)
{
    char  *s_cfg = NULL;
    int    i_err = ERR_OK;

    s_cfg  = cfgfile_get_config_path (&i_err);

    if (i_err != ERR_OK) {
        exit (EXIT_FAILURE);
    }
    return s_cfg;
}
/*----------------------------------------------------------------------------*/

