/**
 * @file  live_source.c
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
 * @brief  Helpers shared by the remote live wallpaper services.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <err.h>
#include "live_source.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free the strings of an image and set them to null.
 */
void
live_image_clear (LiveImage *image)
{
    free (image->image_url);
    free (image->file_name);
    free (image->title);
    free (image->author);
    free (image->license);
    free (image->license_url);
    free (image->page_url);
    *image = (LiveImage) {};
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set a string member of an image to a copy of a value.
 */
void
live_image_set (char       **member,
                const char  *value)
{
    free (*member);
    *member = nullptr;

    if (value != nullptr && (*member = strdup (value)) == nullptr)
        err (EXIT_FAILURE, nullptr);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build a cache file name from parts.
 */
char *
live_file_name (const char *service,
                const char *id,
                const char *ext)
{
    char   *name = nullptr; /* Result file name */
    size_t  len  = 0;       /* Length of the result */

    if (ext == nullptr)
        ext = "";

    len = strlen (service) + 1 + strlen (id) + strlen (ext) + 1;

    if ((name = malloc (len)) == nullptr)
        err (EXIT_FAILURE, nullptr);

    strcpy (name, service);
    strcat (name, "_");
    strcat (name, id);

    for (char *c = name; *c != '\0'; ++c) {
        if (!isalnum ((unsigned char) *c) && *c != '.' && *c != '-')
            *c = '_';
    }
    strcat (name, ext);

    return name;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Choose the service to ask first.
 */
size_t
live_pick_first (const size_t count,
                 const long   last,
                 const bool   random,
                 RandMem     *rand_mem)
{
    if (count == 0)
        return 0;

    if (random)
        return randomm_get_number (rand_mem) % count;

    if (last < 0 || (size_t) last >= count)
        return 0;

    return ((size_t) last + 1) % count;
}
/*----------------------------------------------------------------------------*/
