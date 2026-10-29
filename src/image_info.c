/**
 * @file  image_info.c
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
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "cres.h"
#include "image_info.h"
#include "string_functions.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static void image_info_init (ImageInfo *info)
 *
 * @brief  Init ImageInfo data.
 *
 * @param[out] info  Pointer to ImageInfo object
 * @return     none
 */
static void
image_info_init (ImageInfo *info)
{
    info->file_path    = nullptr;
    info->file_name    = nullptr;
    info->file_dir     = nullptr;
    info->width_height = nullptr;
    info->height       = 0;
    info->width        = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new empty ImageInfo object.
 */
ImageInfo *
image_info_new (void)
{
    ImageInfo *res;

    if ((res = malloc (sizeof (ImageInfo))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    image_info_init (res);

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new ImageInfo object with data gathered from file fname
 */
ImageInfo *
image_info_new_from_file (const char *fname)
{
    ImageInfo  *info  = nullptr; /* Result ImageInfo */
    char       *path  = nullptr; /* String for file path */
    const char *slash = nullptr; /* Pointer to right position of / */
    int         width = 0;       /* Image width */
    int         height = 0;      /* Image height */

    info = image_info_new ();
    image_info_set_file_path (info, fname);

    if ((slash = strrchr (fname, '/')) == nullptr) {
        image_info_set_file_name (info, fname);
        image_info_set_file_dir (info, "");
    }
    else {
        ++slash;
        image_info_set_file_name (info, slash);
        path = strndup (fname, (size_t) (slash - fname));
        image_info_set_file_dir (info, path);
        free (path);
    }

    gdk_pixbuf_get_file_info (fname,
                              &width,
                              &height);

    image_info_set_width (info, width);
    image_info_set_height (info, height);

    image_info_set_wxh (info, width, height);

    return info;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free ImageInfo data.
 */
void
image_info_free (ImageInfo *info)
{
    g_free (info->file_path);
    g_free (info->file_name);
    g_free (info->file_dir);
    g_free (info->width_height);
    g_free (info);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image info of files in list and store it in ImageInfo list.
 */
GSList *
file_paths_to_image_info (const GSList *files)
{
    GSList     *info_list = nullptr; /* Result ImageInfo list */
    char       *fn        = nullptr; /* File path */
    ImageInfo  *info;                /* ImageInfo to add to list */

    while (files != nullptr) {

        if ((fn = (char *) files->data) != nullptr) {

            info = image_info_new_from_file (fn);

            if (image_info_get_height (info) > 0 &&
                image_info_get_width (info) > 0)

                info_list = g_slist_append (info_list, info);
        }
        files = files->next;
    }
    return info_list;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the full file name string (dir + file name)
 */
void
image_info_set_file_path (ImageInfo  *info,
                          const char *name)
{
    if (info->file_path != nullptr)
        free (info->file_path);

    info->file_path = strdup (name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the file name string
 */
void
image_info_set_file_name (ImageInfo  *info,
                          const char *name)
{
    if (info->file_name != nullptr)
        free (info->file_name);

    info->file_name = strdup (name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the file dir string
 */
void
image_info_set_file_dir (ImageInfo  *info,
                         const char *name)
{
    if (info->file_dir != nullptr)
        free (info->file_dir);

    info->file_dir = strdup (name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get string with image dimensions (width x height)
 */
void
image_info_set_wxh (ImageInfo *info,
                    const int  width,
                    const int  height)
{
    char tmp [41]; /* Temp string, I think it is long enough */
    int  len = 0;  /* Length of string with dimensions */

    len = snprintf (tmp, 41, "%dx%d", width, height);
    if      (len < 0)  len = 0;
    else if (len > 40) len = 40;
    cres ((void**) &info->width_height, (size_t) (len + 1), sizeof (char));
    memcpy (info->width_height, tmp, (size_t) len);
    info->width_height[len] = '\0';
}
/*----------------------------------------------------------------------------*/
