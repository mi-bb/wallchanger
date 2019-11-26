/**
 * @file  miscfun.c
 * @copyright Copyright (C) 2019 Michał Bąbik
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
 * @brief  Other functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdio.h>
#include <dirent.h> 
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include "errs.h"
#include "miscfun.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static int check_permissions (const char *s_name, int i_mode)
 * @brief     Check permissions, existence of file or directory. 
 * @param[in] s_name  Name of file / directory to check
 * @param[in] i_mode  Permissions to check
 * @return    Checking status
 *
 * @fn  static int check_file_permissions (const char *s_file)
 * @brief     Check file permissions (read write), existence. 
 * @param[in] s_file  File name to check
 * @return    Checking status
 *
 * @fn  static int check_dir_permissions (const char *s_dir)
 * @brief     Check directory permissions (read, write, execute), existence. 
 * @param[in] s_dir  Directory name to check
 * @return    Checking status
 *
 * @fn  static int check_dir_premissions_create (const char *s_dir)
 * @brief     Check directory permissions, existence and create if needed. 
 * @param[in] s_dir  Directory name to check / create
 * @return    Checking / creating status
 *
 * @fn  static int check_file_premissions_create (const char *s_file)
 * @brief     Check file permissions, existence and maybe create it. 
 * @param[in] s_file  Name of file to check / create
 * @return    Checking / creating status
 */
/*----------------------------------------------------------------------------*/
static int check_permissions             (const char *s_name,
                                          int         i_mode);
/*----------------------------------------------------------------------------*/
static int check_file_permissions        (const char *s_file);
/*----------------------------------------------------------------------------*/
static int check_dir_permissions         (const char *s_dir);
/*----------------------------------------------------------------------------*/
static int check_dir_premissions_create  (const char *s_dir);
/*----------------------------------------------------------------------------*/
static int check_file_premissions_create (const char *s_file);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Hash function.
 */
uint64_t
hash(const char *str)
{
    uint64_t hash = 5381;
    int         c = 0;
    if (str == NULL)
        return 0;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create or resize dynamic array
 */
int
create_resize (void        **v_ptr,
               const size_t  num,
               const size_t  size)
{
    void *s_tmp = NULL; /* Temp pointer for realloc */

    if (size == 0 || num == 0) {
        if (*v_ptr != NULL) {
            free (*v_ptr);
            *v_ptr = NULL;
        }
        return ERR_OK;
    }
    else {
        if (*v_ptr == NULL) {
            *v_ptr = calloc (num, size);
            if (*v_ptr == NULL) {
                fputs ("Alloc error\n", stderr);
                /*return ERR_ALLOC;*/
                exit (EXIT_FAILURE);
            }
        }
        else {
            s_tmp = realloc (*v_ptr, num * size);
            if (s_tmp == NULL) {
                free (*v_ptr);
                fputs ("Alloc error\n", stderr);
                /*return ERR_ALLOC;*/
                exit (EXIT_FAILURE);
            }
            else {
            *v_ptr = s_tmp;
            }
        }
    }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check permissions, existence of file or directory. 
 */
static int
check_permissions (const char *s_name,
                   int         i_mode)
{
    /* check if file/dir exists */
    if (access (s_name, F_OK) == 0) {
        /* check permissions */
        if (access (s_name, i_mode) != 0) {
            fputs ("Bad permissions\n", stderr);
            return ERR_FILE_RW;
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
            fputs ("Directory can not be created\n", stderr);
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
            fputs ("File can not be created\n", stderr);
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
check_config_path_file (int *i_err)
{
    const char *s_sett = "wchanger.json"; /* Settings file name */
    const char *s_cfg  = "/.config/";     /* Settings file path */
    char       *s_path = NULL;            /* Result full config file name */
    char       *s_home = NULL;            /* Home path */
    size_t      i_len  = 0;               /* Config path length */

    /* Getting user's HOME path */
    if ((s_home = getenv ("HOME")) == NULL) {
        s_home = getpwuid (getuid ())->pw_dir;
    }

    /* Total config file path and name length */
    i_len = strlen (s_home) + strlen (s_cfg) + strlen (s_sett);

    /* Create string for config file path and name */
    create_resize ((void**) &s_path, (i_len + 1), sizeof (char));

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

    /* Check config file existence and permissions,
     * create file if needed */
    *i_err = check_file_premissions_create (s_path);
    if (*i_err != ERR_OK) {
        free (s_path);
        return NULL;
    }

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file.
 */
int
read_file_data (const char  *s_fname,
                char       **s_buff)
{
    FILE  *f_file;     /* Data file */
    long   l_size = 0; /* File size */
    size_t st_res = 0; /* Read data count */

    f_file = fopen (s_fname, "rb");
    if (f_file == NULL) {
        fputs ("File error\n", stderr);
        perror("Error occurred");
        return ERR_FILE;
    }

    /* get file size */
    fseek (f_file , 0 , SEEK_END);
    l_size = ftell (f_file);
    rewind (f_file);

    if (l_size == 0) {
        *s_buff = NULL;
        fclose (f_file);
        return 0;
    }

    /* allocate memory to contain the whole file */
    *s_buff = calloc (l_size+1, sizeof (char));
    if (*s_buff == NULL) {
        fputs ("Alloc error\n", stderr);
        perror("Error occurred");
        exit (EXIT_FAILURE);
        /*return ERR_ALLOC;*/
    }

    /* copy the file into the buffer */
    st_res = fread (*s_buff, 1, l_size, f_file);
    if (st_res != (size_t) l_size) {
        fputs ("File reading error\n", stderr);
        perror("Error occurred");
        return ERR_FILE_RW;
    }
    fclose (f_file);

    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file and count hash.
 */
int
read_file_data_hash (const char  *s_fname,
                     char       **s_buff,
                     uint64_t    *i_hash)
{
    int i_res = ERR_OK; /* Function result */

    i_res = read_file_data (s_fname, s_buff);
    if (i_res != 0) {
        *i_hash = 0;
        return i_res;
    }
    *i_hash = hash (*s_buff);

    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save a bunch of data to file.
 */
int
save_file_data (const char *s_fname,
                const char *s_buff)
{
    FILE   *f_file;  /* File to save data */
    size_t  st_res;  /* Save data count */
    size_t  st_size; /* Buffer size */

    f_file = fopen (s_fname, "wb");
    if (f_file == NULL) {
        fputs ("File error\n", stderr);
        perror("Error occurred");
        return ERR_FILE;
    }
    st_size = strlen(s_buff);
    st_res = fwrite (s_buff , sizeof(char), st_size, f_file);
    fclose (f_file);
    if (st_res != st_size) {
        fputs ("File writting error\n", stderr);
        perror("Error occurred");
        return ERR_FILE_RW;
        }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get directory content in FList 
 */
void
get_directory_content_append_to_flist (const char *s_path1,
                                       FList      *fl_files)
{
    char   *s_pthfn   = NULL; /* Full file name with path */
    char   *s_path    = NULL; /* File path */
    size_t  i_dlen    = 0;    /* Path string length */
    DIR    *dr;               /* Dirent directory */
    struct  dirent *de;       /* Dirent struct */

    i_dlen = strlen (s_path1);

    /* Reserve 1 more for a slash later */
    create_resize ((void**) &s_path, i_dlen + 2, sizeof (char));
    strcpy (s_path, s_path1);
    if (s_path[i_dlen-1] != '/') {
        strcat (s_path, "/");
        i_dlen++;
    }

    dr = opendir (s_path); 
    if (dr == NULL) {
        printf ("Could not open current directory\n"); 
        free (s_path);
        return; 
    } 
    while ((de = readdir(dr)) != NULL) {
        /*if (de->d_type == DT_REG) {*/
        if (de->d_type == 8) {
            s_pthfn = calloc ((i_dlen + strlen (de->d_name)+1), sizeof (char));
            if (s_pthfn == NULL) {
                fputs ("Alloc error\n", stderr);
                /*return ERR_ALLOC;*/
                exit (EXIT_FAILURE);
            }
            strcpy (s_pthfn, s_path);
            strcat (s_pthfn, de->d_name);
            flist_insert_data (fl_files, s_pthfn);
            free (s_pthfn);
        }
    }
    if (s_path != NULL)
        free (s_path);

    closedir(dr);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  String compare function for getting string position function.
 */
int
compare_strings (const char *a,
                 const char *b)
{
    int i_res = 0; /* Function result */

    if (a == NULL || b == NULL) {
        if (a == NULL && b == NULL)
            i_res = 0;
        else {
            if (a == NULL)
                i_res = -1;
            else
                i_res = 1;
        }
    }
    else {
        i_res = strcmp (a, b);
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates string with name made of s_name string and number
 */ 
char *
string_name_with_number (const char   *s_name,
                         const size_t  i_no)
{
    char   *s_res = NULL;
    size_t  i_tmp = i_no;
    size_t  i_l = 0;

    do {
        i_tmp /= 10;
        i_l++;
    }
    while (i_tmp);

    s_res = calloc (strlen (s_name) + i_l + 1, sizeof (char));
    if (s_res == NULL)
        return NULL;

    sprintf (s_res, "%s%ld", s_name, i_no);
    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace one string with another in a given src_dest string.
 */ 
char *
string_replace_in (const char *s_src,
                   const char *s_fr,
                   const char *s_to)
{
    const char *srcdst = NULL; /* copy s_src pointer */
    const char *pn     = NULL; /* find string pointer */
    char       *s_res  = NULL; /* result string */
    size_t      ui_len = 0;    /* length to allocate */

    srcdst = s_src; 

    ui_len = strlen (s_src) + 1;
    create_resize ((void**) &s_res, ui_len, sizeof (char));

    /* find the first occurence of "replace from" */
    pn = strstr (srcdst, s_fr); 

    /* while there are "replace from" in source string */
    while (pn != NULL) {
        ui_len += (strlen (s_to) - strlen (s_fr));
        create_resize ((void**) &s_res, ui_len, sizeof (char));
        /* append original text from last found up to new found */
        strncat (s_res, srcdst, pn - srcdst);
        /* append "replace to" */
        strcat (s_res, s_to);
        /* change source pointer to "after found" */
        srcdst = pn + strlen (s_fr); 
        /* find another "replace from" str in src */
        pn = strstr (srcdst, s_fr);  
    }
    strcat (s_res, srcdst);
    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate n bytes of string.
 */
char *
str_ndup (const char *s_str,
          size_t      st_len)
{
    char *s_res = NULL;

    if (s_str == NULL)
        return NULL;

    create_resize ((void**) &s_res, st_len+1, sizeof (char));
    /*
    s_res = calloc (st_len, sizeof (char));
    if (s_res == NULL)
        return NULL;

    strcpy (s_res, s_str);
    */
    memcpy (s_res, s_str, st_len);

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Duplicate string.
 */
char *
str_dup (const char *s_str)
{
    return str_ndup (s_str, strlen (s_str));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make command to set wallpaper.
 */ 
char *
set_up_wallpaper_command (const char *s_cmd,
                          const char *s_fname,
                          const char *s_sign)
{
    char   *s_res  = NULL;
    size_t  ui_siz = 0;
    
    if (strstr (s_cmd, s_sign) == NULL) {

        ui_siz = strlen (s_cmd) + strlen (s_fname) + 4;
        create_resize ((void**) &s_res, ui_siz, sizeof (char));

        /* Print command, file name and & to string */
        sprintf (s_res, "%s %s &", s_cmd, s_fname);
    }
    else {
        s_res = string_replace_in (s_cmd, s_sign, s_fname);
        ui_siz = strlen (s_res) + 3;
        create_resize ((void**) &s_res, ui_siz, sizeof (char));
        strcat (s_res, " &");
    }
    return s_res;
}
/*----------------------------------------------------------------------------*/

