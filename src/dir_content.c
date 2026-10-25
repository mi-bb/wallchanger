/**
 * @file  dir_content.c
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
 * @brief  Getting directory content functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <string.h>
#include <stdio.h>
#include <err.h>
#include "string_functions.h"
#include "dir_content.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static const char * get_file_ext (const char *fn)
 *
 * @brief  Get file extension.
 *
 * @param[in]  fn String with file path
 * @return     Pointer to file extension
 *
 * @fn  static GHashTable * get_pbuf_exts_to_ghash (void)
 *
 * @brief  Get list of extensions supported by GdkPixbuf.
 *
 * @return  Hash table with extensions
 *
 * @fn  static GList * get_directory_filtered_content_glist (const char *dir,
                                                             GHashTable *ext_hash)

 * @brief  Get list of files in directory, filter extensions from GHashTable,
 *         return as GList.
 *
 * @param[in]  dir       Directory path to scan
 * @param[in]  ext_hash  GHashTable with extensions
 * @return     GList with file names
 */
/*----------------------------------------------------------------------------*/
static const char * get_file_ext                         (const char *fn);

static GHashTable * get_pbuf_exts_to_ghash               (void);

static GList      * get_directory_filtered_content_glist (const char *dir,
                                                          GHashTable *ext_hash)
                    __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get file extension.
 */
static const char *
get_file_ext (const char *fn)
{
    const char *ext = nullptr; /* Pointer to extension in string */
    const char *dot = nullptr; /* Pointer to first right . */

    if (fn == nullptr)
        return nullptr;

    if ((dot = strrchr (fn, '.')) != nullptr) {
        ext = dot+1;
    }
    return ext;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of extensions supported by GdkPixbuf.
 */
static GHashTable *
get_pbuf_exts_to_ghash (void)
{
    GdkPixbufFormat  *format       = nullptr; /* Single pixbuf format */
    GSList           *formats      = nullptr; /* Copy of pixbuf list */
    GSList           *formats_list = nullptr; /* Pixbuf list */
    char            **exts         = nullptr; /* Exstension list for a pixbuf */
    char            **it           = nullptr; /* For list iteration */

    GHashTable *ext_hash = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                  g_free, nullptr);
    /* Get information about image formats supported by GdkPixbuf */
    formats_list = gdk_pixbuf_get_formats ();
    formats      = formats_list;

#ifdef DEBUG
    printf ("Valid extensions : ");
#endif

    while (formats != nullptr) {

        format = formats->data;

        /* Get extension list for current format */
        exts = gdk_pixbuf_format_get_extensions (format);

        for (it = exts; *it != nullptr; it++) {
#ifdef DEBUG
            printf ("%s ", *it);
#endif
            /* Insert extension to hash table */
            g_hash_table_add (ext_hash, strdup (*it));
        }
        /* Free extensions list */
        g_strfreev (exts);

        formats = formats->next;
    }
#ifdef DEBUG
    printf ("\n");
#endif

    g_slist_free (formats_list);

    return ext_hash;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of files in directory, filter extensions from GHashTable,
 *         return as GList.
 */
static GList *
get_directory_filtered_content_glist (const char *dir,
                                      GHashTable *ext_hash)
{
    GList      *files     = nullptr; /* Result file list */
    char       *full_path = nullptr; /* Full file name with path */
    char       *path      = nullptr; /* File path */
    const char *ext       = nullptr; /* File extension */
    size_t      dir_len   = 0; /* Path string length */
    size_t      name_len  = 0; /* File name length */
    DIR        *dr;            /* Dirent directory */
    struct dirent *de;         /* Dirent struct */

    dir_len = strlen (dir);
    /* Reserve 1 more for a slash later */
    path = malloc ((dir_len + 2) * sizeof (char));

    if (path == nullptr) {
        err (EXIT_FAILURE, nullptr);
    }
    memcpy (path, dir, dir_len);

    if (path[dir_len-1] != '/') {
        path[dir_len++] = '/';
    }
    path[dir_len] = '\0';

    if ((dr = opendir (path)) == nullptr) {
        warn ("%s", path);
        free (path);
        return nullptr;
    }

    while ((de = readdir(dr)) != nullptr) {

        if (de->d_type == DT_REG) {

            ext = get_file_ext (de->d_name);

            if (ext != nullptr &&
                g_hash_table_lookup (ext_hash, ext) != nullptr) {

                name_len = strlen (de->d_name);

                full_path = malloc ((dir_len + name_len + 1) * sizeof (char));
                if (full_path == nullptr) {
                    err (EXIT_FAILURE, nullptr);
                }
                memcpy (full_path, path, dir_len);
                memcpy (full_path + dir_len, de->d_name, name_len);
                full_path[dir_len + name_len] = '\0';

                files = g_list_prepend (files, full_path);
            }
        }
    }
    free (path);

    closedir(dr);

    return g_list_reverse (files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of files in directory, filter out non images and
 *         return list in GList format.
 */
GList *
get_dir_content_filter_images (const char *dir)
{
    GList      *files     = nullptr;  /* Images in directory */
    GHashTable *ext_hash  = nullptr;  /* List of extensions */

    /* Append to list extensions of image types supported by GdkPixbuf */
    ext_hash = get_pbuf_exts_to_ghash ();

    /* Scan directory for files, filter them and append to file list */
    files = get_directory_filtered_content_glist (dir, ext_hash);

    g_hash_table_destroy (ext_hash);
    return files;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Enumerate folder content and get size of files.
 *
 * @param[in]  dir       Gfile of directory to scan
 * @param[in]  recursive Scan also subfilders
 * @param[out] size      Size output
 * @return     none
 */
static void
enumerate_folder_get_size (GFile    *dir,
                           const int recursive,
                           goffset  *size)
{
    GFileEnumerator *enumerator;
    GError          *error;
    GFile           *file;
    GFileInfo       *info;
    GFileType        file_type;

    #ifdef DEBUG
        printf ("Enumerating folder %s\n", g_file_peek_path (dir));
    #endif

    enumerator = g_file_enumerate_children (dir,
                                           "standard::*",
                                           G_FILE_QUERY_INFO_NONE,
                                           nullptr,
                                           &error);
    do {
        g_file_enumerator_iterate (enumerator, &info, &file, nullptr, &error);
        if (info != nullptr) {
            file_type = g_file_info_get_file_type (info);
            if (file_type == G_FILE_TYPE_DIRECTORY && recursive) {
                enumerate_folder_get_size (file, recursive, size);
                }
            if (file_type == G_FILE_TYPE_REGULAR) {
                *size += g_file_info_get_size (info);
            }
        }
    }
    while (info != nullptr);

    g_object_unref (enumerator);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Enumerate and delete folder content.
 *
 * @param[in]  dir       Gfile of directory to scan
 * @param[in]  recursive Scan also subfilders
 * @return     none
 */
static void
enumerate_folder_delete (GFile    *dir,
                         const int recursive)
{
    GFileEnumerator *enumerator;
    GError          *error;
    GFile           *file;
    GFileInfo       *info;
    GFileType        file_type;

    #ifdef DEBUG
        printf ("Enumerating folder %s\n", g_file_peek_path (dir));
    #endif

    enumerator = g_file_enumerate_children (dir,
                                           "standard::*",
                                           G_FILE_QUERY_INFO_NONE,
                                           nullptr,
                                           &error);
    do {
        g_file_enumerator_iterate (enumerator, &info, &file, nullptr, &error);
        if (info != nullptr) {
            file_type = g_file_info_get_file_type (info);
            if (file_type == G_FILE_TYPE_DIRECTORY && recursive) {
                enumerate_folder_delete (file, recursive);
                }
            if (file_type == G_FILE_TYPE_REGULAR) {
                g_file_delete (file, nullptr, nullptr);
            }
        }
    }
    while (info != nullptr);

    g_object_unref (enumerator);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get size of all files in a directory.
 */
goffset
get_dir_size (const char *path)
{
    GFile   *file;
    goffset  size = 0;

    file = g_file_new_for_path (path);

    enumerate_folder_get_size (file, 0, &size);

    g_object_unref (file);

    return size;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete files in given directory.
 */
void
delete_dir_content (const char *path)
{
    GFile *file;

    file = g_file_new_for_path (path);

    enumerate_folder_delete (file, 0);

    g_object_unref (file);
}
/*----------------------------------------------------------------------------*/
