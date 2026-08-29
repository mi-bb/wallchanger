/**
 * @file  dirlist.h
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
 * @brief  Getting directory content functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef DIRLIST_H
#define DIRLIST_H

#include <gmodule.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of files in a directory, filter out non images and
 *         return list in GList format.
 *
 * @param[in]  s_dir  Directory path to scan
 * @return     GList with file names 
 */
GList * get_dir_content_filter_images   (const char *s_dir)
                                         __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get size of all files in a directory.
 *
 * @param[in]  s_path  Directory path to scan
 * @return     Size of files in folder
 */
goffset dirlist_get_dir_size            (const char *s_path)
                                         __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete files in given directory.
 *
 * @param[in]  s_path  Directory path to scan
 * @return     none
 */
void    dirlist_delete_dir_content      (const char *s_path);
/*----------------------------------------------------------------------------*/
#endif

