/**
 * @file  iminfo.c
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
 * @brief  ImageInfo structure and functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "cres.h"
#include "iminfo.h"
#include "strfun.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static void imageinfo_init (ImageInfo *ii_info)
 *
 * @brief  Init ImageInfo data.
 *
 * @param[out] ii_info  Pointer to ImageInfo object
 * @return     none
 */
static void imageinfo_init (ImageInfo *ii_info);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Init ImageInfo data.
 */
static void
imageinfo_init (ImageInfo *ii_info)
{
    ii_info->s_full_path    = NULL;
    ii_info->s_file_name    = NULL;
    ii_info->s_file_path    = NULL;
    ii_info->s_width_height = NULL;
    ii_info->i_height       = 0;
    ii_info->i_width        = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new empty ImageInfo object.
 */
ImageInfo *
imageinfo_new (void)
{
    ImageInfo *ii_res;

    if ((ii_res = malloc (sizeof (ImageInfo))) == NULL)
        err (EXIT_FAILURE, NULL);

    imageinfo_init (ii_res);

    return ii_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new ImageInfo object with data gathered from file s_fname
 */
ImageInfo *
imageinfo_new_from_file (const char *s_fname)
{
    ImageInfo *ii_info = NULL; /* Result ImageInfo */
    char      *s_path  = NULL; /* String for file path */
    char      *s_p     = NULL; /* Pointer to right position of / */
    int        i_w     = 0;    /* Image width */
    int        i_h     = 0;    /* Image height */

    ii_info = imageinfo_new ();
    imageinfo_set_full_name (ii_info, s_fname);

    if ((s_p = strrchr (s_fname, '/')) == NULL) {
        imageinfo_set_file_name (ii_info, s_fname);
        imageinfo_set_file_path (ii_info, "");
    }
    else {
        ++s_p;
        imageinfo_set_file_name (ii_info, s_p);
        s_path = strndup (s_fname, (size_t) (s_p - s_fname));
        imageinfo_set_file_path (ii_info, s_path);
        free (s_path);
    }

    gdk_pixbuf_get_file_info (s_fname,
                              &i_w,
                              &i_h);

    imageinfo_set_width (ii_info, i_w);
    imageinfo_set_height (ii_info, i_h);

    imageinfo_set_wxh (ii_info, i_w, i_h);

    return ii_info;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free ImageInfo data.
 */
void
imageinfo_free (ImageInfo *ii_info)
{
    g_free (ii_info->s_full_path);
    g_free (ii_info->s_file_name);
    g_free (ii_info->s_file_path);
    g_free (ii_info->s_width_height);
    g_free (ii_info);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image info of files in list and store it in ImageInfo list.
 */
GSList *
file_paths_to_imageinfo (const GSList *gsl_files1)
{
    GSList       *gsl_iinfo = NULL; /* Result ImageInfo list */
    const GSList *gsl_files = NULL; /* Pointer to file list */
    char         *s_fn      = NULL; /* File path */
    ImageInfo    *ii_info;          /* ImageInfo to add to list */

    gsl_files = gsl_files1;

    while (gsl_files != NULL) {

        if ((s_fn = (char *) gsl_files->data) != NULL) {

            ii_info = imageinfo_new_from_file (s_fn);

            if (imageinfo_get_height (ii_info) > 0 &&
                imageinfo_get_width (ii_info) > 0)

                gsl_iinfo = g_slist_append (gsl_iinfo, ii_info);
        }

        gsl_files = gsl_files->next;
    }
    return gsl_iinfo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the full file name string (path + file name)
 */
void
imageinfo_set_full_name (ImageInfo  *ii_info, 
                         const char *s_name)
{
    if (ii_info->s_full_path != NULL)
        free (ii_info->s_full_path);

    ii_info->s_full_path = strdup (s_name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the full file name string (path + file name)
 */
const char *
imageinfo_get_full_name (const ImageInfo *ii_info)
{
     return (const char*) ii_info->s_full_path;       
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the file name string
 */
void
imageinfo_set_file_name (ImageInfo  *ii_info, 
                         const char *s_name)
{
    if (ii_info->s_file_name != NULL)
        free (ii_info->s_file_name);

    ii_info->s_file_name = strdup (s_name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the file name string
 */
const char *
imageinfo_get_file_name (const ImageInfo *ii_info)
{
     return (const char*) ii_info->s_file_name;       
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the file path string
 */
void
imageinfo_set_file_path (ImageInfo  *ii_info, 
                         const char *s_name)
{
    if (ii_info->s_file_path != NULL)
        free (ii_info->s_file_path);

    ii_info->s_file_path = strdup (s_name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the file path string
 */
const char *
imageinfo_get_file_path (const ImageInfo *ii_info)
{
     return (const char*) ii_info->s_file_path;       
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set image width information
 */
void
imageinfo_set_width (ImageInfo *ii_info, 
                     const int  i_val)
{
    ii_info->i_width = i_val; 
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image width value
 */
int
imageinfo_get_width (const ImageInfo *ii_info)
{
    return ii_info->i_width;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set image height information
 */
void
imageinfo_set_height (ImageInfo *ii_info, 
                      const int  i_val)
{
    ii_info->i_height = i_val; 
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image height value
 */
int
imageinfo_get_height (const ImageInfo *ii_info)
{
    return ii_info->i_height;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get string with image dimensions (width x height)
 */
void
imageinfo_set_wxh (ImageInfo *ii_info,
                   const int  i_w,
                   const int  i_h)
{
    char s_tmp [41]; /* Temp string, I think it is long enough */
    int  n = 0;      /* Length of string with dimensions */

    n = snprintf (s_tmp, 41, "%dx%d", i_w, i_h);
    if      (n < 0)  n = 0;
    else if (n > 40) n = 40;
    cres ((void**) &ii_info->s_width_height, (size_t) (n + 1), sizeof (char));
    memcpy (ii_info->s_width_height, s_tmp, (size_t) n);
    ii_info->s_width_height[n] = '\0';
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get string with image dimensions (width x height)
 */
const char *
imageinfo_get_wxh (const ImageInfo *ii_info)
{
    return (const char*) ii_info->s_width_height;
}
/*----------------------------------------------------------------------------*/

