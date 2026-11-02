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
#include <err.h>

#include "config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif

#include "url_data.h"
#include "search_item.h"
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
    item->author_name    = nullptr;
    item->author_url     = nullptr;
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
    free (item->author_name);
    free (item->author_url);
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
 * @brief  Set SearchItem's photo id string and int value based on int value.
 */
void
search_item_set_id_uint (SearchItem    *item,
                         const uint64_t val)
{
    char tmp[32];

    sprintf (tmp, "%" PRIu64, val);
    if (item->id != nullptr)
        free (item->id);
    item->id = strdup (tmp);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's photo id string and int value based on string value.
 */
void
search_item_set_id_string (SearchItem *item,
                           const char *val)
{
    if (item->id != nullptr)
        free (item->id);
    item->id = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's author name.
 */
void
search_item_set_author_name (SearchItem *item,
                             const char *val)
{
    if (item->author_name != nullptr)
        free (item->author_name);
    item->author_name = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's photo author url.
 */
void
search_item_set_author_url (SearchItem *item,
                            const char *val)
{
    if (item->author_url != nullptr)
        free (item->author_url);
    item->author_url = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's file name.
 */
void
search_item_set_file_name (SearchItem *item,
                           const char *val)
{
    if (item->file_name != nullptr)
        free (item->file_name);
    item->file_name = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's display name.
 */
void
search_item_set_display_name (SearchItem *item,
                              const char *val)
{
    if (item->display_name != nullptr)
        free (item->display_name);
    item->display_name = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's markup.
 */
void
search_item_set_display_markup (SearchItem *item,
                                const char *val)
{
    if (item->display_markup != nullptr)
        free (item->display_markup);
    item->display_markup = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's page url.
 */
void
search_item_set_page_url (SearchItem *item,
                          const char *val)
{
    if (item->page_url != nullptr)
        free (item->page_url);
    item->page_url = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's image url.
 */
void
search_item_set_image_url (SearchItem *item,
                           const char *val)
{
    if (item->image_url != nullptr)
        free (item->image_url);
    item->image_url = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's thumb url.
 */
void
search_item_set_thumb_url (SearchItem *item,
                           const char *val)
{
    if (item->thumb_url != nullptr)
        free (item->thumb_url);
    item->thumb_url = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's service name.
 */
void
search_item_set_service_name (SearchItem *item,
                              const char *val)
{
    if (item->service_name != nullptr)
        free (item->service_name);
    item->service_name = strdup (val);
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
void
search_item_print (SearchItem *item)
{
    printf ("id          : %s\n", item->id);
    printf ("author name : %s\n", item->author_name);
    printf ("author url  : %s\n", item->author_url);
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

