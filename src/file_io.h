/**
 * @file  file_io.h
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
#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdint.h>
/*----------------------------------------------------------------------------*/
/**
 * @fn  char * read_file_data (const char    *fname,
 *                             int           *err,
 *                             uint_fast32_t *hash_out)
 *
 * @brief  Read some data from file.
 *
 * @param[in]  fname     File name
 * @param[out] err       Error output
 * @param[out] hash_out  Pointer to hash
 * @return     Read data
 *
 * @fn  int save_file_data (const char *fname, const char *buff)
 *
 * @brief  Save a bunch of data to file.
 *
 * @param[in]  fname File name
 * @param[in]  buff  Buffer with data
 * @return     Writing status
 */
/*----------------------------------------------------------------------------*/
char * read_file_data      (const char    *fname,
                            int           *err,
                            uint_fast32_t *hash_out)
                            __attribute__ ((nonnull (1)));

int    save_file_data      (const char    *fname,
                            const char    *buff)
                            __attribute__ ((nonnull (1,2)));
/*----------------------------------------------------------------------------*/
#endif

