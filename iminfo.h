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
#include <string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "miscfun.h"
#include "flist.h"
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
/**
 * @brief  Function compares ImageInfo items by file name string.
 *
 * @param[in] ii_info1 First ImageInfo item
 * @param[in] ii_info2 Second ImageInfo item
 * @return    1 if a>b, -1 if a<b, 0 if a=b
 */
int compare_imageitems (const ImageInfo *ii_info1,
                        const ImageInfo *ii_info2);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free ImageInfo data.
 *
 * @param[out]  ii_info  Pointer to ImageInfo object
 * @return      none
 */
void imageinfo_free (ImageInfo *ii_info);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Init ImageInfo data.
 *
 * @param[out]  ii_info  Pointer to ImageInfo object
 * @return      none
 */
void imageinfo_init (ImageInfo *ii_info);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get file info and put it in a ImageInfo object.
 *
 * @param[in]  s_file_name  String with file name
 * @return     File info in ImageInfo format, null if could not alloc memory
 */
ImageInfo *imageinfo_get_info (const char *s_file_name);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image info of files in list and store it in ImageInfo list.
 *
 * @param[in] gsl_files1  File list to process
 * @return    List of ImageInfo items
 */
GSList *file_paths_to_imageinfo (GSList *gsl_files1);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image info of files in list and store it in FList.
 *
 * @param[in]  fl_files  File list to process
 * @return     List of ImageInfo items
 */
GSList * flist_to_imageinfo (FList *fl_files);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Extract file paths list of ImageInfo list and append to FList.
 *
 * @param[in]  gsl_iinfo1  List with ImageInfo items
 * @param[out] fl_files    FList to append data
 * @return     Append result
 */
int imageinfo_append_to_flist (GSList *gsl_iinfo1, FList *fl_files);
/*----------------------------------------------------------------------------*/
#endif

