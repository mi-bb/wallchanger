/**
 * @file  search_item.c
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
 * @brief  Web search result
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <err.h>
#include <glib.h>

#include "search_item.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   DISP_NAME_LEN
 * @brief Number of characters of an image name shown on the image list.
 */
constexpr long DISP_NAME_LEN = 40;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Init SearchItem item.
 *
 * @param[out] item  SearchItem item
 * @return     none
 */
static void
search_item_init (SearchItem *item)
{
    item->id             = nullptr;
    item->title          = nullptr;
    item->author_name    = nullptr;
    item->author_url     = nullptr;
    item->license_name   = nullptr;
    item->license_url    = nullptr;
    item->file_name      = nullptr;
    item->display_name   = nullptr;
    item->display_markup = nullptr;
    item->page_url       = nullptr;
    item->image_url      = nullptr;
    item->thumb_url      = nullptr;
    item->service_name   = nullptr;
    item->width          = 0;
    item->height         = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free SearchItem item.
 */
void
search_item_free (SearchItem *item)
{
    free (item->id);
    free (item->title);
    free (item->author_name);
    free (item->author_url);
    free (item->license_name);
    free (item->license_url);
    free (item->file_name);
    free (item->display_name);
    free (item->display_markup);
    free (item->page_url);
    free (item->image_url);
    free (item->thumb_url);
    free (item->service_name);
    free (item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new SearchItem item.
 */
SearchItem *
search_item_new (void)
{
    SearchItem *item = nullptr;

    if ((item = malloc (sizeof (SearchItem))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    search_item_init (item);

    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace a string field with a copy of val.
 *
 * @param[in,out] field  Field to set, its old value is freed
 * @param[in]     val    New value, null clears the field
 * @return        none
 */
static void
set_field (char      **field,
           const char *val)
{
    char *copy = nullptr;

    if (val != nullptr && (copy = strdup (val)) == nullptr)
        err (EXIT_FAILURE, nullptr);

    free (*field);
    *field = copy;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's image id.
 */
void
search_item_set_id_string (SearchItem *item,
                           const char *val)
{
    set_field (&item->id, val);

    /* The id is a part of the image and thumbnail file names */
    for (char *c = item->id; c != nullptr && *c != '\0'; ++c) {
        if (!isalnum ((unsigned char) *c) && *c != '-' && *c != '_')
            *c = '_';
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's image title.
 */
void
search_item_set_title (SearchItem *item,
                       const char *val)
{
    set_field (&item->title, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's author name.
 */
void
search_item_set_author_name (SearchItem *item,
                             const char *val)
{
    set_field (&item->author_name, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's photo author url.
 */
void
search_item_set_author_url (SearchItem *item,
                            const char *val)
{
    set_field (&item->author_url, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's license name.
 */
void
search_item_set_license_name (SearchItem *item,
                              const char *val)
{
    set_field (&item->license_name, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's license url.
 */
void
search_item_set_license_url (SearchItem *item,
                             const char *val)
{
    set_field (&item->license_url, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's file name.
 */
void
search_item_set_file_name (SearchItem *item,
                           const char *val)
{
    set_field (&item->file_name, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's display name.
 */
void
search_item_set_display_name (SearchItem *item,
                              const char *val)
{
    set_field (&item->display_name, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's markup.
 */
void
search_item_set_display_markup (SearchItem *item,
                                const char *val)
{
    set_field (&item->display_markup, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's page url.
 */
void
search_item_set_page_url (SearchItem *item,
                          const char *val)
{
    set_field (&item->page_url, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's image url.
 */
void
search_item_set_image_url (SearchItem *item,
                           const char *val)
{
    set_field (&item->image_url, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's thumb url.
 */
void
search_item_set_thumb_url (SearchItem *item,
                           const char *val)
{
    set_field (&item->thumb_url, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's service name.
 */
void
search_item_set_service_name (SearchItem *item,
                              const char *val)
{
    set_field (&item->service_name, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the service name and, based on the item's id, title and size,
 *         the file name to save the image to and the names to show on the
 *         image list.
 */
void
search_item_set_names (SearchItem *item,
                       const char *service_name,
                       const char *ext)
{
    const char *name      = nullptr; /* Title, or id when there is no title */
    char       *file_name = nullptr; /* Name for file to save */
    char       *shortened = nullptr; /* Name cut to DISP_NAME_LEN chars */
    char       *disp_name = nullptr; /* Name to show on the image list */
    char       *markup    = nullptr; /* Markup to show on the image list */

    search_item_set_service_name (item, service_name);

    if (item->id == nullptr)
        return;

    file_name = g_strconcat (service_name, "_", item->id,
                             ext != nullptr ? ext : "", nullptr);

    name = (item->title != nullptr && item->title[0] != '\0') ?
           item->title : item->id;

    if (g_utf8_strlen (name, -1) > DISP_NAME_LEN) {
        shortened = g_utf8_substring (name, 0, DISP_NAME_LEN);
        disp_name = g_strconcat (shortened, "...", nullptr);
    }
    else {
        disp_name = g_strdup (name);
    }
    if (item->width > 0 && item->height > 0) {
        markup = g_markup_printf_escaped (
                "%s\n<span font_size=\"small\">[%dx%d]</span>",
                disp_name, item->width, item->height);
    }
    else {
        markup = g_markup_escape_text (disp_name, -1);
    }
    search_item_set_file_name      (item, file_name);
    search_item_set_display_name   (item, disp_name);
    search_item_set_display_markup (item, markup);

    g_free (file_name);
    g_free (shortened);
    g_free (disp_name);
    g_free (markup);
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
void
search_item_print (SearchItem *item)
{
    printf ("id          : %s\n", item->id);
    printf ("title       : %s\n", item->title);
    printf ("author name : %s\n", item->author_name);
    printf ("author url  : %s\n", item->author_url);
    printf ("license     : %s\n", item->license_name);
    printf ("license url : %s\n", item->license_url);
    printf ("file name   : %s\n", item->file_name);
    printf ("displ name  : %s\n", item->display_name);
    printf ("displ mkup  : %s\n", item->display_markup);
    printf ("page url    : %s\n", item->page_url);
    printf ("image url   : %s\n", item->image_url);
    printf ("thumb url   : %s\n", item->thumb_url);
    printf ("serv name   : %s\n", item->service_name);
    printf ("width       : %d\n", item->width);
    printf ("height      : %d\n", item->height);
}
#endif
/*----------------------------------------------------------------------------*/
