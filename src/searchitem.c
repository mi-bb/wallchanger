/**
 * @file  searchitem.c
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
 * @brief  Web search result
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <err.h>

#include "../config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif

#include "urldata.h"
#include "searchitem.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Init SearchItem item.
 *
 * @param[out] si_item  SearchItem item
 * @return     none
 */
static void
searchitem_init (SearchItem *si_item)
{
    si_item->i_id             = 0;
    si_item->s_id             = NULL;
    si_item->s_author_name    = NULL;
    si_item->s_author_url     = NULL;
    si_item->s_file_name      = NULL;
    si_item->s_display_name   = NULL;
    si_item->s_display_markup = NULL;
    si_item->s_page_url       = NULL;
    si_item->s_image_url      = NULL;
    si_item->s_thumb_url      = NULL;
    si_item->s_service_name   = NULL;
    si_item->i_width          = 0;
    si_item->i_height         = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free SearchItem item.
 */
void
searchitem_free (SearchItem *si_item)
{
    free (si_item->s_id);
    free (si_item->s_author_name);
    free (si_item->s_author_url);
    free (si_item->s_file_name);
    free (si_item->s_display_name);
    free (si_item->s_display_markup);
    free (si_item->s_page_url);
    free (si_item->s_image_url);
    free (si_item->s_thumb_url);
    free (si_item->s_service_name);
    free (si_item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new SearchItem item.
 */
SearchItem *
searchitem_new (void)
{
    SearchItem *si_item = NULL;

    if ((si_item = malloc (sizeof (SearchItem))) == NULL)
        err (EXIT_FAILURE, NULL);

    searchitem_init (si_item);

    return si_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's photo id string and int value based on int value.
 */
void
searchitem_set_id_uint (SearchItem    *si_item,
                        const uint64_t val)
{
    char s_tmp[32];

    sprintf (s_tmp, "%" PRIu64, val);
    si_item->i_id = val;
    if (si_item->s_id != NULL)
        free (si_item->s_id);
    si_item->s_id = strdup (s_tmp);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's photo id string and int value based on string value.
 */
void
searchitem_set_id_string (SearchItem *si_item,
                          const char *s_val)
{
    si_item->i_id = (uint64_t) strtoull (s_val, NULL, 10);
    if (si_item->s_id != NULL)
        free (si_item->s_id);
    si_item->s_id = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's file name.
 */
void
searchitem_set_author_name (SearchItem *si_item,
                            const char *s_val)
{
    if (si_item->s_author_name != NULL)
        free (si_item->s_author_name);
    si_item->s_author_name = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's photo author url.
 */
void
searchitem_set_author_url (SearchItem *si_item,
                           const char *s_val)
{
    if (si_item->s_author_url != NULL)
        free (si_item->s_author_url);
    si_item->s_author_url = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's file name.
 */
void
searchitem_set_file_name (SearchItem *si_item,
                          const char *s_val)
{
    if (si_item->s_file_name != NULL)
        free (si_item->s_file_name);
    si_item->s_file_name = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's display name.
 */
void
searchitem_set_display_name (SearchItem *si_item,
                             const char *s_val)
{
    if (si_item->s_display_name != NULL)
        free (si_item->s_display_name);
    si_item->s_display_name = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's markup.
 */
void
searchitem_set_display_markup (SearchItem *si_item,
                               const char *s_val)
{
    if (si_item->s_display_markup != NULL)
        free (si_item->s_display_markup);
    si_item->s_display_markup = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's page url.
 */
void
searchitem_set_page_url (SearchItem *si_item,
                         const char *s_val)
{
    if (si_item->s_page_url != NULL)
        free (si_item->s_page_url);
    si_item->s_page_url = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's image url.
 */
void
searchitem_set_image_url (SearchItem *si_item,
                          const char *s_val)
{
    if (si_item->s_image_url != NULL)
        free (si_item->s_image_url);
    si_item->s_image_url = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's thumb url.
 */
void
searchitem_set_thumb_url (SearchItem *si_item,
                          const char *s_val)
{
    if (si_item->s_thumb_url != NULL)
        free (si_item->s_thumb_url);
    si_item->s_thumb_url = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's service name.
 */
void
searchitem_set_service_name (SearchItem *si_item,
                             const char *s_val)
{
    if (si_item->s_service_name != NULL)
        free (si_item->s_service_name);
    si_item->s_service_name = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
void
searchitem_print (SearchItem *si_item)
{
    printf ("id          : %s\n", si_item->s_id);
    printf ("author name : %s\n", si_item->s_author_name);
    printf ("author url  : %s\n", si_item->s_author_url);
    printf ("file name   : %s\n", si_item->s_file_name);
    printf ("displ name  : %s\n", si_item->s_display_name);
    printf ("displ mkup  : %s\n", si_item->s_display_markup);
    printf ("page url    : %s\n", si_item->s_page_url);
    printf ("image url   : %s\n", si_item->s_image_url);
    printf ("thumb url   : %s\n", si_item->s_thumb_url);
    printf ("serv name   : %s\n", si_item->s_service_name);
    printf ("width       : %d\n", si_item->i_width);
    printf ("height      : %d\n", si_item->i_height);
}
#endif
/*----------------------------------------------------------------------------*/

