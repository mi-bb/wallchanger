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
#include "iminfo.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Function compares ImageInfo items by file name string.
 */
int
compare_imageitems (const ImageInfo *ii_info1,
                    const ImageInfo *ii_info2)
{
    return compare_strings (ii_info1->s_file_name, ii_info2->s_file_name);
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
 * @brief  Init ImageInfo data.
 */
void
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
 * @brief  Get file info and put it in a ImageInfo object.
 */
ImageInfo *
imageinfo_get_info (const char *s_file_name)
{
    ImageInfo *ii_info = NULL; /* Result ImageInfo */
    char      *s_p     = NULL; /* Pointer to right position of / */
    
    if (s_file_name == NULL)
        return NULL;

    create_resize ((void**) &ii_info, 1, sizeof (ImageInfo));

    imageinfo_init (ii_info);

    ii_info->s_full_path = str_dup (s_file_name);

    s_p = strrchr (s_file_name, '/');

    if (s_p == NULL) {
        ii_info->s_file_name = str_dup (s_file_name);
        ii_info->s_file_path = str_dup ("");
    }
    else {
        s_p++;
        ii_info->s_file_name = str_dup (s_p);
        ii_info->s_file_path = str_ndup (s_file_name, s_p - s_file_name);
    }
    create_resize ((void**) &ii_info->s_width_height, 40, sizeof (char));
    gdk_pixbuf_get_file_info (s_file_name,
                              &ii_info->i_width,
                              &ii_info->i_height);
    sprintf (ii_info->s_width_height,
             "%dx%d",
             ii_info->i_width, ii_info->i_height);

    return ii_info;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image info of files in list and store it in ImageInfo list.
 */
GSList *
file_paths_to_imageinfo (GSList *gsl_files1)
{
    GSList *gsl_iinfo = NULL; /* Result ImageInfo list */
    GSList *gsl_files = NULL; /* Temp file list */

    gsl_files = g_slist_copy (gsl_files1);

    while (gsl_files != NULL) {
        ImageInfo *ii_info = imageinfo_get_info ((char *) gsl_files->data);
        if (ii_info->i_height > 0)
            gsl_iinfo = g_slist_append (gsl_iinfo, ii_info);
        gsl_files = gsl_files->next;
    }
    g_slist_free (gsl_files);
    return gsl_iinfo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get image info of files in list and store it in FList.
 */
GSList *
flist_to_imageinfo (FList *fl_files)
{
    GSList     *gsl_iinfo = NULL; /* Result ImageInfo list */
    const char *s_fn      = NULL; /* File name string */
    uint32_t    ui_cnt    = 0;    /* Number of items in file list */

    ui_cnt = flist_get_len (fl_files);

    for (uint32_t i = 0; i < ui_cnt; ++i) {
        s_fn = flist_get_data (fl_files, i);
        ImageInfo *ii_info = imageinfo_get_info (s_fn);
        if (ii_info->i_height > 0)
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
    GSList *gsl_iinfo = NULL; /* Temp ImageInfo list */

    gsl_iinfo = g_slist_copy (gsl_iinfo1);

    while (gsl_iinfo != NULL) {
        ImageInfo *ii_info = gsl_iinfo->data;
        flist_insert_data (fl_files, (const char *) ii_info->s_full_path);
        gsl_iinfo = gsl_iinfo->next;
    }
    g_slist_free (gsl_iinfo);
    return 0;
}
/*----------------------------------------------------------------------------*/

