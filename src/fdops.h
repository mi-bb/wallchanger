/**
 * @file  fdops.h
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
 * @brief  File, directory operations
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef FDOPS_H
#define FDOPS_H
#include <gmodule.h>
/*----------------------------------------------------------------------------*/
/**
 * @fn         GHashTable * get_pbuf_exts_to_ghash (void)
 * @brief      Get list of extensions supported by GdkPixbuf.
 * @return     Hash table with extensions 
 *
 * @fn         GList * get_directory_content_glist (const char *s_dir)
 * @brief      Get list of files in directory in GList.
 * @param[in]  s_dir  Directory path to scan
 * @return     GList with file names 
 *
 * @fn         GList * glist_filter_by_extensions_list (GList      *gl_files,
 *                                                      GHashTable *gh_exts)
 * @brief      Filter file list with extension list.
 *
 * Remove from file list files that have extensions not present on given
 * extensions list.
 *
 * @param[out] gl_files  File list to process
 * @param[in]  gh_exts   List of extensions
 * @return     none
 */
/*----------------------------------------------------------------------------*/
GHashTable * get_pbuf_exts_to_ghash          (void);

GList      * get_directory_content_glist     (const char *s_dir)
                                              __attribute__ ((nonnull (1)));

GList      * glist_filter_by_extensions_list (GList      *gl_files,
                                              GHashTable *gh_exts);
/*----------------------------------------------------------------------------*/
#endif

