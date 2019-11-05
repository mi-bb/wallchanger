/**
 * @file  iminfo.h
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
 * @brief  ImageInfo structure and functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef IMINFO_H
#define IMINFO_H
#include <stdio.h>
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "miscfun.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  ImageInfo data structure
 */
typedef struct
ImageInfo {
    char *s_full_path;     /**< Full file name with path */
    char *s_file_name;     /**< File name only */
    char *s_file_path;     /**< File path */
    char *s_width_height;  /**< Width x Height in string */
    int   i_width;         /**< Image width in int */
    int   i_height;        /**< Image height in int */
} ImageInfo;
/*----------------------------------------------------------------------------*/
int compare_imageitems (const ImageInfo *ii_info1,
                        const ImageInfo *ii_info2);
/*----------------------------------------------------------------------------*/
void imageinfo_free (ImageInfo *ii_info);
/*----------------------------------------------------------------------------*/
ImageInfo *imageinfo_get_info (const char *s_file_name);
/*----------------------------------------------------------------------------*/
GSList *file_paths_to_imageinfo (GSList *gsl_files1);
/*----------------------------------------------------------------------------*/
GSList *imageinfo_to_file_paths (GSList *gsl_iinfo1);
/*----------------------------------------------------------------------------*/
#endif

