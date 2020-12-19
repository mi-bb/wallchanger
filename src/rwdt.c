/**
 * @file  rwdt.c
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
 * @brief  Read / write data functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include "errs.h"
#include "cres.h"
#include "hashfun.h"
#include "rwdt.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file.
 */
char *
read_file_data (const char    *s_fname,
                int           *i_err,
                uint_fast32_t  *i_hash)
{
    char   *s_buff = NULL; /* Result data from file */
    FILE   *f_file;        /* Data file */
    long    l_size = 0;    /* File size */
    size_t  ui_res = 0;    /* Read data count */

    if (i_hash != NULL)
        *i_hash = 0;

    errno = 0;

    f_file = fopen (s_fname, "rb");
    /* Error opening file */
    if (f_file == NULL) {
        *i_err = errno == ENOENT ? ERR_FILE_EX : ERR_FILE;
        warn ("%s", s_fname);
        return NULL;
    }
    /* get file size */
    fseek (f_file , 0 , SEEK_END);
    l_size = ftell (f_file);
    rewind (f_file);

    /* Empty file */
    if (l_size == 0) {
        *i_err = ERR_OK;
        fclose (f_file);
        return NULL;
    }
    /* Function failure */
    if (l_size < 0) {
        *i_err = ERR_FILE_RW;
        warn ("%s", s_fname);
        fclose (f_file);
        return NULL;
    }
    cres ((void**) &s_buff, (size_t) l_size + 1, sizeof (char));
    s_buff[0] = '\0';
    /* copy the file into the buffer */
    ui_res = fread (s_buff, 1, (size_t) l_size, f_file);
    s_buff[l_size] = '\0';

    if (ui_res != (size_t) l_size) {
        *i_err = ERR_FILE_RW;
        warn ("%s", s_fname);
    }
    fclose (f_file);

    *i_err = ERR_OK;
    if (i_hash != NULL)
        *i_hash = hash (s_buff);

    return s_buff;
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
    size_t  ui_res;  /* Save data count */
    size_t  ui_size; /* Buffer size */

    f_file = fopen (s_fname, "wb");

    if (f_file == NULL) {
        warn ("%s", s_fname);
        return ERR_FILE;
    }
    ui_size = strlen(s_buff);
    ui_res  = fwrite (s_buff, sizeof (char), ui_size, f_file);
    fclose (f_file);

    if (ui_res != ui_size) {
        warn ("%s", s_fname);
        return ERR_FILE_RW;
        }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/

