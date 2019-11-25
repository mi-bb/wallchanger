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
 * ImageInfo structure and functions I made to work with images.
 * It contains information about image file name, path, dimensions.
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
    char    *s_full_path;     /**< Full file name with path */
    char    *s_file_name;     /**< File name only */
    char    *s_file_path;     /**< File path */
    char    *s_width_height;  /**< Width x Height in string */
    uint16_t ui_width;        /**< Image width in uint */
    uint16_t ui_height;       /**< Image height in uint */
} ImageInfo;
/*----------------------------------------------------------------------------*/
/**
 * @fn  ImageInfo * imageinfo_new (void)
 * @brief  Create new empty ImageInfo object.
 * @return New ImageInfo object or null if could not alloc memory
 *
 * @fn  ImageInfo *imageinfo_new_from_file (const char *s_fname)
 * @brief     Create new ImageInfo object with data gathered from file s_fname
 * @param[in] s_fname  String with file name
 * @return    File info in ImageInfo format, null if could not alloc memory
 */
/*----------------------------------------------------------------------------*/
ImageInfo * imageinfo_new           (void);
/*----------------------------------------------------------------------------*/
ImageInfo * imageinfo_new_from_file (const char *s_fname);
/*----------------------------------------------------------------------------*/
/**
 * @brief      Free ImageInfo data.
 * @param[out] ii_info  Pointer to ImageInfo object
 * @return     none
 */
void  imageinfo_free  (ImageInfo *ii_info);
/*----------------------------------------------------------------------------*/
/**
 * @fn  GSList * imageinfo_sort (GSList *gsl_iinfo)
 * @brief         Sort ImageInfo list.
 * @param[in,out] gsl_iinfo  Pointer to ImageInfo list
 * @return        Sorted ImageInfo list
 *
 * @fn  GSList * imageinfo_remove_duplicates (GSList *gsl_iinfo)
 * @brief         Remove duplicates from ImageInfo list.
 * @param[in,out] gsl_iinfo  Pointer to ImageInfo list
 * @return        Result ImageInfo list
 */
/*----------------------------------------------------------------------------*/
GSList * imageinfo_sort               (GSList  *gsl_iinfo);
/*----------------------------------------------------------------------------*/
GSList * imageinfo_remove_duplicates  (GSList  *gsl_iinfo);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int imageinfo_append_to_flist  (GSList  *gsl_iinfo1,
 *                                      FList   *fl_files)
 * @brief      Extract file paths list of ImageInfo list and append to FList.
 * @param[in]  gsl_iinfo1  List with ImageInfo items
 * @param[out] fl_files    FList to append data
 * @return     Append result
 *
 * @fn  GSList * file_paths_to_imageinfo (GSList *gsl_files1)
 * @brief      Get image info of files in list and store it in ImageInfo list.
 * @param[in]  gsl_files1  File list to process
 * @return     List of ImageInfo items
 *
 * @fn  GSList * flist_to_imageinfo (FList *fl_files)
 * @brief      Get image info of files in list and store it in FList.
 * @param[in]  fl_files  File list to process
 * @return     List of ImageInfo items
 */
/*----------------------------------------------------------------------------*/
int      imageinfo_append_to_flist (GSList  *gsl_iinfo1,
                                    FList   *fl_files);
/*----------------------------------------------------------------------------*/
GSList * file_paths_to_imageinfo   (GSList  *gsl_files1);
/*----------------------------------------------------------------------------*/
GSList * flist_to_imageinfo        (FList   *fl_files);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void imageinfo_set_full_name (ImageInfo *ii_info, const char *s_name)
 * @brief      Set the full file name string (path + file name)
 * @param[out] ii_info  ImageInfo object to set data
 * @param[in]  s_name   String to set as full file name
 * @return     none
 *
 * @fn  const char * imageinfo_get_full_name (const ImageInfo *ii_info)
 * @brief      Get the full file name string (path + file name)
 * @param[out] ii_info  ImageInfo object with data to get
 * @return     Full file name string
 *
 * @fn  void imageinfo_set_file_name (ImageInfo *ii_info, const char *s_name)
 * @brief      Set the file name string
 * @param[out] ii_info  ImageInfo object to set data
 * @param[in]  s_name   String to set as file name
 * @return     none
 *
 * @fn  const char * imageinfo_get_file_name (const ImageInfo *ii_info)
 * @brief      Get the file name string
 * @param[out] ii_info  ImageInfo object with data to get
 * @return     File name string
 *
 * @fn  void imageinfo_set_file_path (ImageInfo *ii_info, const char *s_name)
 * @brief      Set the file path string
 * @param[out] ii_info  ImageInfo object to set data
 * @param[in]  s_name   String to set as file path
 * @return     none
 *
 * @fn  const char * imageinfo_get_file_path (const ImageInfo *ii_info)
 * @brief      Get the file path string
 * @param[out] ii_info  ImageInfo object with data to get
 * @return     File path string
 *
 * @fn  void imageinfo_set_width (ImageInfo *ii_info, uint16_t ui_val)
 * @brief      Set image width information
 * @param[out] ii_info  ImageInfo object to set data
 * @param[in]  ui_val   Image width value
 * @return     none
 *
 * @fn  int imageinfo_get_width (const ImageInfo *ii_info)
 * @brief      Get image width value
 * @param[out] ii_info  ImageInfo object with data to get
 * @return     Width of an image
 *
 * @fn  void imageinfo_set_height (ImageInfo *ii_info, uint16_t ui_val)
 * @brief      Set image height information
 * @param[out] ii_info  ImageInfo object to set data
 * @param[in]  ui_val   Image height value
 * @return     none
 *
 * @fn  int imageinfo_get_height (const ImageInfo *ii_info)
 * @brief      Get image height value
 * @param[out] ii_info  ImageInfo object with data to get
 * @return     Height of an image
 *
 * @fn  const char * imageinfo_get_wxh (ImageInfo *ii_info)
 * @brief      Get string with image dimensions (width x height)
 * @param[out] ii_info  ImageInfo object with data to get
 * @return     String with image dimensions
 */
/*----------------------------------------------------------------------------*/
void         imageinfo_set_full_name  (ImageInfo        *ii_info,
                                       const char       *s_name);
/*----------------------------------------------------------------------------*/
const char * imageinfo_get_full_name  (const ImageInfo  *ii_info);
/*----------------------------------------------------------------------------*/
void         imageinfo_set_file_name  (ImageInfo        *ii_info,
                                       const char       *s_name);
/*----------------------------------------------------------------------------*/
const char * imageinfo_get_file_name  (const ImageInfo  *ii_info);
/*----------------------------------------------------------------------------*/
void         imageinfo_set_file_path  (ImageInfo        *ii_info,
                                       const char       *s_name);
/*----------------------------------------------------------------------------*/
const char * imageinfo_get_file_path  (const ImageInfo  *ii_info);
/*----------------------------------------------------------------------------*/
void         imageinfo_set_width      (ImageInfo        *ii_info,
                                       uint16_t          ui_val);
/*----------------------------------------------------------------------------*/
int          imageinfo_get_width      (const ImageInfo  *ii_info);
/*----------------------------------------------------------------------------*/
void         imageinfo_set_height     (ImageInfo        *ii_info,
                                       uint16_t          ui_val);
/*----------------------------------------------------------------------------*/
int          imageinfo_get_height     (const ImageInfo  *ii_info);
/*----------------------------------------------------------------------------*/
const char * imageinfo_get_wxh        (ImageInfo        *ii_info);
/*----------------------------------------------------------------------------*/
#endif

