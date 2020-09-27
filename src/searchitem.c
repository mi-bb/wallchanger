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
    si_item->id        = 0;
    si_item->name      = NULL;
    si_item->markup    = NULL;
    si_item->url       = NULL;
    si_item->thumb_url = NULL;
    si_item->width     = 0;
    si_item->height    = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free SearchItem item.
 */
void
searchitem_free (SearchItem *si_item)
{
    free (si_item->name);
    free (si_item->markup);
    free (si_item->url);
    free (si_item->thumb_url);
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
 * @brief  Set SearchItem's name.
 */
void
searchitem_set_name (SearchItem *si_item,
                     const char *s_val)
{
    if (si_item->name != NULL)
        free (si_item->name);
    si_item->name = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's markup.
 */
void
searchitem_set_markup (SearchItem *si_item,
                       const char *s_val)
{
    if (si_item->markup != NULL)
        free (si_item->markup);
    si_item->markup = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's url.
 */
void
searchitem_set_url (SearchItem *si_item,
                    const char *s_val)
{
    if (si_item->url != NULL)
        free (si_item->url);
    si_item->url = strdup (s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set SearchItem's thumb url.
 */
void
searchitem_set_thumb_url (SearchItem *si_item,
                          const char *s_val)
{
    if (si_item->thumb_url != NULL)
        free (si_item->thumb_url);
    si_item->thumb_url = strdup (s_val);
}
/*----------------------------------------------------------------------------*/

