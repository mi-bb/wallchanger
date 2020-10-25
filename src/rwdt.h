/**
 * @file  rwdt.h
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
#ifndef RWDT_H
#define RWDT_H

#include <stdint.h>
/*----------------------------------------------------------------------------*/
/**
 * #fn  char * read_file_data (const char    *s_fname,
 *                             int           *i_err,
 *                             int           *i_hash)
 *
 * @brief  Read some data from file.
 *
 * @param[in]  s_fname File name
 * @param[out] i_err   Error output
 * @param[out] i_hash  Pointer to hash
 * @return     Read data
 *
 * @fn  int save_file_data (const char *s_fname, const char *s_buff)
 *
 * @brief  Save a bunch of data to file.
 *
 * @param[in]  s_fname File name
 * @param[in]  s_buff  Buffer with data
 * @return     Writting status
 */
/*----------------------------------------------------------------------------*/
char * read_file_data      (const char    *s_fname,
                            int           *i_err,
                            int           *i_hash)
                            __attribute__ ((nonnull (1)));

int    save_file_data      (const char    *s_fname,
                            const char    *s_buff)
                            __attribute__ ((nonnull (1,2)));
/*----------------------------------------------------------------------------*/
#endif

