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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdio.h>
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
read_file_data (const char  *s_fname,
                int         *i_err)
{
    char   *s_buff = NULL; /* Read file data */
    FILE   *f_file;        /* Data file */
    long    l_size = 0;    /* File size */
    size_t  ul_res = 0;    /* Read data count */

    f_file = fopen (s_fname, "rb");

    if (f_file == NULL) {

        *i_err = ERR_FILE;

        fputs (s_fname, stderr);
        perror("Error occurred");
        return NULL;
    }

    /* get file size */
    fseek (f_file , 0 , SEEK_END);
    l_size = ftell (f_file);
    rewind (f_file);

    if (l_size == 0) {

        *i_err = ERR_OK;

        fclose (f_file);
        return NULL;
    }
    create_resize ((void**) &s_buff, (size_t) l_size+1, sizeof (char));
    /* copy the file into the buffer */
    ul_res = fread (s_buff, 1, (size_t) l_size, f_file);

    if (ul_res != (size_t) l_size) {

        *i_err = ERR_FILE_RW;

        fputs (s_fname, stderr);
        fputs (" File reading error\n", stderr);
        perror("Error occurred");
    }
    fclose (f_file);

    *i_err = ERR_OK;

    return s_buff;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file and count hash.
 */
char *
read_file_data_hash (const char  *s_fname,
                     int         *i_err,
                     uint32_t    *i_hash)
{
    char *s_buff = NULL;

    s_buff = read_file_data (s_fname, i_err);

    if (*i_err != ERR_OK) {
        *i_hash = 0;
        return NULL;
    }

    if (s_buff == NULL)
        *i_hash = 0;
    else
        *i_hash = hash (s_buff);

    *i_err = ERR_OK;

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
    size_t  ul_res;  /* Save data count */
    size_t  ul_size; /* Buffer size */

    f_file = fopen (s_fname, "wb");

    if (f_file == NULL) {
        fputs (s_fname, stderr);
        perror("Error occurred");
        return ERR_FILE;
    }
    ul_size = strlen(s_buff);
    ul_res = fwrite (s_buff , sizeof(char), ul_size, f_file);
    fclose (f_file);

    if (ul_res != ul_size) {
        fputs (s_fname, stderr);
        fputs (" File writting error\n", stderr);
        perror("Error occurred");
        return ERR_FILE_RW;
        }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/

