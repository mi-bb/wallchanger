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
 * @brief  Get users home dir path.
 *
 * @return  String with home path, after use it thould be freed using free
 */
static char * cfgfile_get_home_dir (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create file with subfolders if it doesn't exist.
 *
 * @param[in] s_fn  File path
 * @return    Process result
 */
static int create_file_with_subdirs (const char *s_fn);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config file in paths from list.
 *
 * @param[in] s_cc  Null terminadet list with config file paths
 * @return    Config file path string or null if config files were not found,
 *            after use it should be freed using free
 */
static char * cfgfile_find_config_file (const char **s_cc);
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
    return check_permissions (s_dir, R_OK | X_OK);
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
static char *
cfgfile_get_home_dir (void)
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
static int
create_file_with_subdirs (const char *s_fn)
{
    char  *s_new  = NULL;          /* New string with dirs */
    char  *s_sls  = NULL;          /* / position */
    char  *s_dup  = strdup (s_fn); /* Duplicate of s_fn */
    char  *s_tmp  = s_dup;         /* Pointer to duplicate of s_fn */
    int    i_err  = 0;             /* Error output */
    size_t ui_len = 0;             /* Name length */

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
        if ((i_err = check_dir_premissions_create (s_new)) != ERR_OK) {
            warn (NULL);
            return i_err;
        }
        ui_len = (size_t) (s_sls - s_tmp);
        /* *s_sls = '/'; */
        s_tmp += ui_len + 1;
    }
    free (s_dup);
    free (s_new);

    if ((i_err = check_file_premissions_create (s_fn)) != ERR_OK) {
        warn (NULL);
        return i_err;
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config file in paths from list.
 */
static char *
cfgfile_find_config_file (const char **s_cc)
{
    char *s_tmp  = NULL; /* Temp string */
    char *s_home = NULL; /* Home path string */

    s_home = cfgfile_get_home_dir ();

    while (*s_cc != NULL) {
        s_tmp = str_comb (s_home, *s_cc);
        if (check_file_permissions (s_tmp) == ERR_OK) {
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
                    *s_file = cfgfile_get_home_dir ();
                    str_append (&(*s_file), s_cfg_files[0]);
                    i_st = 4;
                    break;
                }
                if (*s_file == NULL) {
                    warnx ("No config files found");
                    return ERR_CFG_NOF;
                }
                return ERR_OK;
            case 3: /* Config file passed, check it */
                i_res = check_file_permissions (*s_file);
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
                if ((i_res = create_file_with_subdirs (*s_file)) != ERR_OK) {
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

