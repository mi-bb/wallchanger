/**
 * @file  file_io.c
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
 * @brief  File read / write functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
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
#include "errors.h"
#include "cres.h"
#include "hash_djb2.h"
#include "file_io.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read some data from file.
 */
char *
read_file_data (const char    *fname,
                int           *err,
                uint_fast32_t *hash_out)
{
    char   *buff = nullptr; /* Result data from file */
    FILE   *file;           /* Data file */
    long    size = 0;       /* File size */
    size_t  res  = 0;       /* Read data count */

    if (hash_out != nullptr)
        *hash_out = 0;

    errno = 0;

    file = fopen (fname, "rb");
    /* Error opening file */
    if (file == nullptr) {
        *err = errno == ENOENT ? ERR_FILE_EX : ERR_FILE;
        warn ("%s", fname);
        return nullptr;
    }
    /* get file size */
    fseek (file , 0 , SEEK_END);
    size = ftell (file);
    rewind (file);

    /* Empty file */
    if (size == 0) {
        *err = ERR_OK;
        fclose (file);
        return nullptr;
    }
    /* Function failure */
    if (size < 0) {
        *err = ERR_FILE_RW;
        warn ("%s", fname);
        fclose (file);
        return nullptr;
    }
    cres ((void**) &buff, (size_t) size + 1, sizeof (char));
    buff[0] = '\0';
    /* copy the file into the buffer */
    res = fread (buff, 1, (size_t) size, file);
    buff[size] = '\0';

    if (res != (size_t) size) {
        *err = ERR_FILE_RW;
        warn ("%s", fname);
    }
    fclose (file);

    *err = ERR_OK;
    if (hash_out != nullptr)
        *hash_out = hash (buff);

    return buff;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save a bunch of data to file.
 */
int
save_file_data (const char *fname,
                const char *buff)
{
    FILE   *file;  /* File to save data */
    size_t  res;   /* Save data count */
    size_t  size;  /* Buffer size */

    file = fopen (fname, "wb");

    if (file == nullptr) {
        warn ("%s", fname);
        return ERR_FILE;
    }
    size = strlen(buff);
    res  = fwrite (buff, sizeof (char), size, file);
    fclose (file);

    if (res != size) {
        warn ("%s", fname);
        return ERR_FILE_RW;
        }
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/

