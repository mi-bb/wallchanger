/**
 * @file  iminfo.c
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
#include <stdio.h>
#include <string.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "miscfun.h"
#include "iminfo.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static int compare_imageitems (const ImageInfo *ii_info1,
 *                                     const ImageInfo *ii_info2)
 * @brief     Function compares ImageInfo items by file name string.
 * @param[in] ii_info1 First ImageInfo item
 * @param[in] ii_info2 Second ImageInfo item
 * @return    1 if a>b, -1 if a<b, 0 if a=b
 *
 * @fn  static void imageinfo_init (ImageInfo *ii_info)
 * @brief      Init ImageInfo data.
 * @param[out] ii_info  Pointer to ImageInfo object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static int  compare_imageitems (const ImageInfo *ii_info1,
                                const ImageInfo *ii_info2);

static void imageinfo_init     (ImageInfo       *ii_info);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Function compares ImageInfo items by file name string.
 */
static int
compare_imageitems (const ImageInfo *ii_info1,
                    const ImageInfo *ii_info2)
{
    return compare_strings (ii_info1->s_file_name, ii_info2->s_file_name);
}
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

    ii_res = calloc (1, sizeof (ImageInfo));

    if (ii_res == NULL)
        return NULL;

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

    if (s_fname == NULL)
        return NULL;

    ii_info = imageinfo_new ();

    imageinfo_set_full_name (ii_info, s_fname);

    s_p = strrchr (s_fname, '/');

    if (s_p == NULL) {
        imageinfo_set_file_name (ii_info, s_fname);
        imageinfo_set_file_path (ii_info, "");
    }
    else {
        s_p++;
        imageinfo_set_file_name (ii_info, s_p);

        s_path = str_ndup (s_fname, s_p - s_fname);
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
 * @brief  Sort ImageInfo list.
 */
GSList * imageinfo_sort (GSList *gsl_iinfo)
{
    return g_slist_sort (gsl_iinfo, (GCompareFunc) compare_imageitems);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove duplicates from ImageInfo list.
 */
GSList * imageinfo_remove_duplicates (GSList *gsl_iinfo)
{
    GSList *gsl_itr = NULL;
    GSList *gsl_nxt = NULL;
    GSList *gsl_act = NULL;

    for (gsl_itr = gsl_iinfo; gsl_itr; gsl_itr = gsl_itr->next) {
        gsl_nxt = gsl_itr->next;

        while (gsl_nxt) {

            gsl_act = gsl_nxt;
            gsl_nxt = gsl_nxt->next;

            if (compare_imageitems (gsl_itr->data, gsl_act->data) == 0) {

                gsl_iinfo = g_slist_remove_link (gsl_iinfo, gsl_act);

                imageinfo_free (gsl_act->data);
            }
        }
    }
    return gsl_iinfo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image info of files in list and store it in ImageInfo list.
 */
GSList *
file_paths_to_imageinfo (GSList *gsl_files1)
{
    GSList    *gsl_iinfo = NULL; /* Result ImageInfo list */
    GSList    *gsl_files = NULL; /* Pointer to file list */
    ImageInfo *ii_info;

    gsl_files = gsl_files1;

    while (gsl_files != NULL) {

        ii_info = imageinfo_new_from_file ((char *) gsl_files->data);

        if (imageinfo_get_height (ii_info) > 0)
            gsl_iinfo = g_slist_append (gsl_iinfo, ii_info);

        gsl_files = gsl_files->next;
    }
    return gsl_iinfo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image info of files in list and store it in FList.
 */
GSList *
flist_to_imageinfo (const FList *fl_files)
{
    ImageInfo  *ii_info;
    GSList     *gsl_iinfo = NULL; /* Result ImageInfo list */
    const char *s_fn      = NULL; /* File name string */
    uint32_t    ui_cnt    = 0;    /* Number of items in file list */
    uint32_t    i         = 0;

    ui_cnt = flist_get_len (fl_files);

    for (i = 0; i < ui_cnt; ++i) {

        s_fn = flist_get_data (fl_files, i);
        ii_info = imageinfo_new_from_file (s_fn);

        if (imageinfo_get_height (ii_info) > 0)
            gsl_iinfo = g_slist_append (gsl_iinfo, ii_info);
    }
    return gsl_iinfo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Extract file paths list of ImageInfo list and append to FList.
 */
int
imageinfo_append_to_flist (GSList *gsl_iinfo1,
                           FList  *fl_files)
{
    GSList    *gsl_iinfo = NULL; /* Pointer to ImageInfo list */
    ImageInfo *ii_info;

    gsl_iinfo = gsl_iinfo1;

    while (gsl_iinfo != NULL) {

        ii_info = gsl_iinfo->data;

        flist_insert_data (fl_files, imageinfo_get_full_name (ii_info));

        gsl_iinfo = gsl_iinfo->next;
    }
    return 0;
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

    ii_info->s_full_path = str_dup (s_name);
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

    ii_info->s_file_name = str_dup (s_name);
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

    ii_info->s_file_path = str_dup (s_name);
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
    char s_tmp[40]; /* Temp string, I think it is long enough */

    memset (s_tmp, 0, sizeof(s_tmp));

    sprintf (s_tmp, "%dx%d", i_w, i_h);

    create_resize ((void**) &ii_info->s_width_height, strlen (s_tmp)+1,
                   sizeof (char));

    strcpy (ii_info->s_width_height, s_tmp);
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

