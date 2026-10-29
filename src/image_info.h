/**
 * @file  image_info.h
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
 * @brief  ImageInfo structure and functions
 *
 * ImageInfo structure and functions I made to work with images.
 * It contains information about image file name, path, dimensions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef IMAGE_INFO_H
#define IMAGE_INFO_H

#include <glib.h>
/*----------------------------------------------------------------------------*/
/**
 * @struct ImageInfo
 *
 * @brief  ImageInfo data structure
 *
 * @var   ImageInfo::file_path
 * @brief Full file name with path
 *
 * @var   ImageInfo::file_name
 * @brief File name only
 *
 * @var   ImageInfo::file_dir
 * @brief File dir only
 *
 * @var   ImageInfo::width_height
 * @brief String with Width x Height
 *
 * @var   ImageInfo::width
 * @brief Image width int
 *
 * @var   ImageInfo::height
 * @brief Image height int
 */
typedef struct
ImageInfo {
    char    *file_path;     /* Full file name with path */
    char    *file_name;     /* File name only */
    char    *file_dir;      /* File path */
    char    *width_height;  /* Width x Height in string */
    int      width;         /* Image width in uint */
    int      height;        /* Image height in uint */
} ImageInfo;
/*----------------------------------------------------------------------------*/
/**
 * @fn  ImageInfo * image_info_new (void)
 *
 * @brief  Create new empty ImageInfo object.
 *
 * @return New ImageInfo object or null if could not alloc memory
 *
 * @fn  ImageInfo *image_info_new_from_file (const char *fname)
 *
 * @brief  Create new ImageInfo object with data gathered from file fname
 *
 * @param[in] fname  String with file name
 * @return    File info in ImageInfo format, null if could not alloc memory
 *
 * @fn void image_info_free  (ImageInfo *info)
 *
 * @brief  Free ImageInfo data.
 *
 * @param[out] info  Pointer to ImageInfo object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
ImageInfo *  image_info_new            (void)
             __attribute__ ((returns_nonnull));

ImageInfo *  image_info_new_from_file  (const char       *fname)
             __attribute__ ((nonnull (1), returns_nonnull));

void         image_info_free           (ImageInfo        *info);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image info of files in list and store it in ImageInfo list.
 *
 * @param[in]  files  File list to process
 * @return     List of ImageInfo items
 */
GSList     * file_paths_to_image_info  (const GSList     *files);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void image_info_set_file_path (ImageInfo *info, const char *name)
 *
 * @brief  Set the full file name string (dir + file name)
 *
 * @param[out] info  ImageInfo object to set data
 * @param[in]  name  String to set as full file name
 * @return     none
 *
 * @fn  const char * image_info_get_file_path (const ImageInfo *info)
 *
 * @brief  Get the full file name string (path + file name)
 *
 * @param[out] info  ImageInfo object with data to get
 * @return     Full file name string
 *
 * @fn  void image_info_set_file_name (ImageInfo *info, const char *name)
 *
 * @brief  Set the file name string
 *
 * @param[out] info  ImageInfo object to set data
 * @param[in]  name  String to set as file name
 * @return     none
 *
 * @fn  const char * image_info_get_file_name (const ImageInfo *info)
 *
 * @brief  Get the file name string
 *
 * @param[out] info  ImageInfo object with data to get
 * @return     File name string
 *
 * @fn  void image_info_set_file_dir (ImageInfo *info, const char *name)
 *
 * @brief  Set the file path string
 *
 * @param[out] info  ImageInfo object to set data
 * @param[in]  name  String to set as file path
 * @return     none
 *
 * @fn  const char * image_info_get_file_dir (const ImageInfo *info)
 *
 * @brief  Get the file dir string
 *
 * @param[out] info  ImageInfo object with data to get
 * @return     File path string
 *
 * @fn  void image_info_set_width (ImageInfo *info, const int width)
 *
 * @brief  Set image width information
 *
 * @param[out] info   ImageInfo object to set data
 * @param[in]  width  Image width value
 * @return     none
 *
 * @fn  int image_info_get_width (const ImageInfo *info)
 *
 * @brief  Get image width value
 *
 * @param[out] info  ImageInfo object with data to get
 * @return     Width of an image
 *
 * @fn  void image_info_set_height (ImageInfo *info, const int height)
 *
 * @brief  Set image height information
 *
 * @param[out] info    ImageInfo object to set data
 * @param[in]  height  Image height value
 * @return     none
 *
 * @fn  int image_info_get_height (const ImageInfo *info)
 *
 * @brief  Get image height value
 *
 * @param[out] info  ImageInfo object with data to get
 * @return     Height of an image
 *
 * @fn  const char * image_info_get_wxh (const ImageInfo *info)
 *
 * @brief  Get string with image dimensions (width x height)
 *
 * @param[out] info  ImageInfo object with data to get
 * @return     String with image dimensions
 */
/*----------------------------------------------------------------------------*/
void image_info_set_file_path (ImageInfo       *info,
                               const char      *name)
     __attribute__ ((nonnull (2)));

static inline const char * image_info_get_file_path (const ImageInfo *info) {
     return (const char*) info->file_path;
}
/*----------------------------------------------------------------------------*/
void image_info_set_file_name  (ImageInfo        *info,
                                const char       *name)
     __attribute__ ((nonnull (2)));

static inline const char * image_info_get_file_name (const ImageInfo *info) {
     return (const char*) info->file_name;
}
/*----------------------------------------------------------------------------*/
void image_info_set_file_dir  (ImageInfo        *info,
                               const char       *name)
     __attribute__ ((nonnull (2)));

static inline const char * image_info_get_file_dir (const ImageInfo *info) {
     return (const char*) info->file_dir;
}
/*----------------------------------------------------------------------------*/
static inline void image_info_set_width (ImageInfo        *info,
                                         const int         width) {
    info->width = width;
}
static inline int image_info_get_width  (const ImageInfo  *info) {
    return info->width;
}
/*----------------------------------------------------------------------------*/
static inline void image_info_set_height (ImageInfo        *info,
                                          const int         height) {
    info->height = height;
}
static inline int  image_info_get_height (const ImageInfo  *info) {
    return info->height;
}
/*----------------------------------------------------------------------------*/
void         image_info_set_wxh        (ImageInfo        *info,
                                        const int         width,
                                        const int         height);

static inline const char * image_info_get_wxh (const ImageInfo *info) {
    return (const char*) info->width_height;
}
/*----------------------------------------------------------------------------*/
#endif
