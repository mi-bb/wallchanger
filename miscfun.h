/**
 * @file  miscfun.h
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
#ifndef MISCFUN_H
#define MISCFUN_H
#include <stdio.h>
#include <glib.h>
#include <dirent.h> 
#include "settstr.h"
/*----------------------------------------------------------------------------*/
uint64_t hash(const char *str);
/*----------------------------------------------------------------------------*/
char *get_file_ext (char *s_fn);
/*----------------------------------------------------------------------------*/
int read_file_data (const char *s_fname, char **s_buff);
/*----------------------------------------------------------------------------*/
int save_file_data (const char *s_fname, const char *s_buff);
/*----------------------------------------------------------------------------*/
int compare_strings (const char *a, const char *b);
/*----------------------------------------------------------------------------*/
int my_gslist_get_position (GSList *gsl_list, const char *s_what);
/*----------------------------------------------------------------------------*/
int read_file_data_hash (const char *s_fname, char **s_buff,
                         uint64_t *i_hash);
/*----------------------------------------------------------------------------*/
GSList *get_directory_content (const char *s_path1);
/*----------------------------------------------------------------------------*/
#endif
