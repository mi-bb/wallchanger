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
 *
 * @param[in] ii_info1 First ImageInfo item
 * @param[in] ii_info2 Second ImageInfo item
 * @return    1 if a>b, -1 if a<b, 0 if a=b
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
 *
 * @param[out]  ii_info  Pointer to ImageInfo object
 * @return      none
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
 * @brief  Get file info and put it in a ImageInfo object.
 *
 * @param[in]  s_file_name  String with file name
 * @return     File info in ImageInfo format, null if could not alloc memory
 */
ImageInfo *
imageinfo_get_info (const char *s_file_name)
{
    ImageInfo *ii_info = NULL;
    char      *s_p     = NULL;
    int        i_size  = 0;
    
    ii_info = g_malloc (sizeof (ImageInfo));

    ii_info->s_full_path = g_strdup (s_file_name);
    s_p = strrchr (s_file_name, '/');

    if (s_p == NULL) {
        ii_info->s_file_name = g_strdup (s_file_name);
        ii_info->s_file_path = g_strdup ("");
    }
    else {
        i_size = s_p - s_file_name;
        ii_info->s_file_name = g_malloc0 ((strlen (s_p)+1) * sizeof (char));
        ii_info->s_file_path = g_malloc0 ((i_size+1) * sizeof (char));
        ii_info->s_width_height = g_malloc0 ((40) * sizeof (char));
        s_p++;
        strcpy (ii_info->s_file_name, s_p);
        strncpy (ii_info->s_file_path, s_file_name, s_p - s_file_name);
    }
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
 *
 * @param[in] gsl_files1  File list to process
 * @return    List of ImageInfo items
 */
GSList *
file_paths_to_imageinfo (GSList *gsl_files1)
{
    GSList *gsl_iinfo = NULL;
    GSList *gsl_files = NULL;

    gsl_files = g_slist_copy (gsl_files1);

    while (gsl_files != NULL) {
        ImageInfo *ii_info = imageinfo_get_info ( (char *) gsl_files->data);
        if (ii_info->i_height > 0)
            gsl_iinfo = g_slist_append (gsl_iinfo, ii_info);
        gsl_files = gsl_files->next;
    }
    g_slist_free (gsl_files);
    return gsl_iinfo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Extract file paths list of ImageInfo list.
 *
 * @param[in] gsl_iinfo1  List with ImageInfo items
 * @return    List of file paths
 */
GSList *
imageinfo_to_file_paths (GSList *gsl_iinfo1)
{
    GSList *gsl_iinfo = NULL;
    GSList *gsl_files = NULL;

    gsl_iinfo = g_slist_copy (gsl_iinfo1);

    while (gsl_iinfo != NULL) {
        ImageInfo *ii_info = gsl_iinfo->data;
        char *s_fn = g_strdup (ii_info->s_full_path);
        gsl_files = g_slist_append (gsl_files, s_fn);
        gsl_iinfo = gsl_iinfo->next;
    }
    g_slist_free (gsl_iinfo);
    return gsl_files;
}
/*----------------------------------------------------------------------------*/

