/**
 * @file  fdops.c
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
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include "strfun.h"
#include "fdops.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static const char * get_file_ext (const char *s_fn)
 *
 * @brief  Get file extenstion.
 *
 * @param[in]  s_fn String with file path
 * @return     Pointer to file extension
 *
 * @fn  static GHashTable * get_pbuf_exts_to_ghash (void)
 *
 * @brief  Get list of extensions supported by GdkPixbuf.
 *
 * @return  Hash table with extensions 
 *
 * @fn  static GList * get_directory_filtered_content_glist (const char *s_dir,
                                                             GHashTable *gh_exts)

 * @brief  Get list of files in directory, filter extensions from GHashTable,
 *         return as GList.
 *
 * @param[in]  s_dir    Directory path to scan
 * @param[in]  gh_exts  GHashTable with extensions
 * @return     GList with file names 
 */
/*----------------------------------------------------------------------------*/
static const char * get_file_ext                         (const char *s_fn);

static GHashTable * get_pbuf_exts_to_ghash               (void);

static GList      * get_directory_filtered_content_glist (const char *s_dir,
                                                          GHashTable *gh_exts)
                    __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
static const char *
get_file_ext (const char *s_fn)
{
    const char *s_ext = NULL; /* Pointer to extension in string */
    char       *s_p   = NULL; /* Pointer to first right . */

    if (s_fn == NULL)
        return NULL;

    s_p = strrchr (s_fn, '.');

    if (s_p != NULL) {
        s_ext = s_p+1;
    }
    return s_ext;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of extensions supported by GdkPixbuf.
 */
static GHashTable *
get_pbuf_exts_to_ghash (void)
{
    GdkPixbufFormat  *gpf          = NULL;
    GSList           *gsl_formats  = NULL;
    GSList           *gsl_formats1 = NULL;
    char            **exts         = NULL;
    char            **it           = NULL;

    GHashTable *gh_res = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                g_free, NULL);
    /* Get information aboout image formats supported by GdkPixbuf */
    gsl_formats1 = gdk_pixbuf_get_formats ();
    gsl_formats  = gsl_formats1;

    #ifdef DEBUG
    printf ("Valid extensions : ");
    #endif

    while (gsl_formats != NULL) {

        gpf = gsl_formats->data;

        /* Get extension list for current format */
        exts = gdk_pixbuf_format_get_extensions (gpf);

        for (it = exts; *it != NULL; it++) {
            #ifdef DEBUG
            printf ("%s ", *it);
            #endif
            /* Insert extension to hash table */
            g_hash_table_add (gh_res, strdup (*it));
        }
        /* Free extensions list */
        g_strfreev (exts);

        gsl_formats = gsl_formats->next;
    }
    #ifdef DEBUG
    printf ("\n");
    #endif

    g_slist_free (gsl_formats1);

    return gh_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of files in directory, filter extensions from GHashTable,
 *         return as GList.
 */
static GList *
get_directory_filtered_content_glist (const char *s_dir,
                                      GHashTable *gh_exts)
{
    GList  *gl_files  = NULL; /* Result file list */
    char   *s_pthfn   = NULL; /* Full file name with path */
    char   *s_path    = NULL; /* File path */
    size_t  ul_dlen   = 0;    /* Path string length */
    DIR    *dr;               /* Dirent directory */
    struct  dirent *de;       /* Dirent struct */

    ul_dlen = strlen (s_dir);

    /* Reserve 1 more for a slash later */
    s_path = malloc ((ul_dlen + 2) * sizeof (char));

    if (s_path == NULL) {
        err (EXIT_FAILURE, NULL);
    }
    memcpy (s_path, s_dir, ul_dlen);

    if (s_path[ul_dlen-1] != '/') {
        s_path[ul_dlen++] = '/';
    }
    s_path[ul_dlen] = '\0';

    dr = opendir (s_path); 
    if (dr == NULL) {
        warn ("%s", s_path);
        free (s_path);
        return NULL; 
    }

    while ((de = readdir(dr)) != NULL) {

        if (de->d_type == DT_REG) {

            const char *s_ext = get_file_ext (de->d_name);

            if (s_ext != NULL &&
                g_hash_table_lookup (gh_exts, s_ext) != NULL) {

                s_pthfn = malloc ((ul_dlen + strlen (de->d_name)+1) *
                                  sizeof (char));
                if (s_pthfn == NULL) {
                    err (EXIT_FAILURE, NULL);
                }
                strcpy (s_pthfn, s_path);
                strcat (s_pthfn, de->d_name);

                gl_files = g_list_append (gl_files, s_pthfn);
            }
        }
    }
    free (s_path);

    closedir(dr);

    return gl_files;
}
/*----------------------------------------------------------------------------*/
GList *
get_dir_content_filter_images (const char *s_dir)
{
    GList      *gl_files = NULL;  /* Images in directory */
    GHashTable *gh_exts  = NULL;  /* List of extensions */

    /* Append to list extensions of image types supported by GdkPixbuf */
    gh_exts = get_pbuf_exts_to_ghash ();

    /* Scan directory for files, filter them and append to file list */
    gl_files = get_directory_filtered_content_glist (s_dir, gh_exts);

    g_hash_table_destroy (gh_exts);
    return gl_files;
}
/*----------------------------------------------------------------------------*/

