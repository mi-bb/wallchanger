/**
 * @file  web_met.c
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
 * @brief  Metropolitan Museum of Art image search service.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "url_data.h"
#include "web_util.h"
#include "web_met.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   MET_API_URL
 * @brief Address of the collection API.
 *
 * @var   MET_MAX_RESULTS
 * @brief Number of first search results the service gives access to.
 */
#define MET_API_URL "https://collectionapi.metmuseum.org/public/collection"
constexpr int MET_MAX_RESULTS = 10000;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the Met search request url.
 *
 * @param[in] query        Search query
 * @param[in] search_opts  Not used, the service has no search options
 * @param[in] api_key      Not used, the service takes no key
 * @param[in] page         Results page number
 * @return    New string with url to free with free(), null when the page is
 *            beyond the results the service gives access to
 */
static char *
met_search_url (const char *query,
                [[maybe_unused]] const char *search_opts,
                [[maybe_unused]] const char *api_key,
                const int   page)
{
    char *escaped = nullptr; /* Percent-encoded query */
    char *url     = nullptr; /* Result url */
    int   offset  = (page - 1) * WEB_PAGE_SIZE;

    if (page < 1 || offset + WEB_PAGE_SIZE > MET_MAX_RESULTS)
        return nullptr;

    escaped = urldata_escape (query);
    url = g_strdup_printf (
            MET_API_URL "/v1.1/search?hasImages=true&q=%s&offset=%d&limit=%d",
            escaped, offset, WEB_PAGE_SIZE);
    free (escaped);

    return url;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the Met search response. It holds only the ids of found
 *         objects, so the passed items have nothing but the id set.
 *
 * @param[in]  json       String with json data
 * @param[out] found_cnt  Number of found objects
 * @param[in]  add_item   Function to pass found objects to
 * @param[in]  user_data  Data for add_item
 * @return     false if json is not a Met search response
 */
static bool
met_parse (const char  *json,
           int         *found_cnt,
           WebItemFunc  add_item,
           void        *user_data)
{
    json_object *obj   = web_json_parse (json);
    json_object *arr   = web_json_get (obj, "objectIDs", json_type_array);
    json_object *val   = nullptr; /* Object id */
    SearchItem  *item  = nullptr;
    size_t       cnt   = 0;
    char         id[16];

    /* A search without results gives null in place of the id list */
    if (web_json_get (obj, "total", json_type_int) == nullptr) {
        if (obj != nullptr)
            json_object_put (obj);
        return false;
    }
    *found_cnt = web_json_int (obj, "total");

    if (arr != nullptr)
        cnt = json_object_array_length (arr);

    for (size_t i = 0; i < cnt; ++i) {
        val = json_object_array_get_idx (arr, i);
        if (json_object_get_type (val) != json_type_int)
            continue;

        snprintf (id, sizeof (id), "%d", json_object_get_int (val));
        item = search_item_new ();
        search_item_set_id_string (item, id);
        add_item (item, user_data);
    }
    json_object_put (obj);

    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the url of the request for the data of a Met object.
 *
 * @param[in] id  Object id
 * @return    New string with url to free with free()
 */
static char *
met_detail_url (const char *id)
{
    return g_strconcat (MET_API_URL "/v1/objects/", id, nullptr);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the data of a Met object into a found item.
 *
 * The search can not be limited to public domain artworks, so objects that
 * are not marked as public domain (Open Access, CC0) are rejected here.
 *
 * @param[in]     json  String with json data of the object
 * @param[in,out] item  Item with the object id set
 * @return        false if the object has no public domain image
 */
static bool
met_parse_detail (const char *json,
                  SearchItem *item)
{
    json_object *obj   = web_json_parse (json);
    json_object *pd    = web_json_get (obj, "isPublicDomain",
                                       json_type_boolean);
    const char  *image = web_json_str (obj, "primaryImage");
    const char  *thumb = web_json_str (obj, "primaryImageSmall");
    const char  *ext   = web_ext_from_url (image);
    bool         res   = false;

    if (pd != nullptr && json_object_get_boolean (pd) &&
        image != nullptr && ext != nullptr) {

        search_item_set_title        (item, web_json_str (obj, "title"));
        search_item_set_author_name  (item,
                                      web_json_str (obj, "artistDisplayName"));
        search_item_set_license_name (item, "CC0 1.0");
        search_item_set_license_url  (item,
                "https://creativecommons.org/publicdomain/zero/1.0/");
        search_item_set_image_url    (item, image);
        search_item_set_thumb_url    (item, thumb != nullptr ? thumb : image);
        search_item_set_page_url     (item, web_json_str (obj, "objectURL"));
        search_item_set_names        (item, met_provider.name, ext);
        res = true;
    }
    if (obj != nullptr)
        json_object_put (obj);

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @var   met_provider
 * @brief Metropolitan Museum of Art image search service.
 */
const WebProvider met_provider = {
    .name         = "Met",
    .label        = "The Met",
    .opts_name    = "Met_opts",
    .logo         = W_IMG_COUNT,
    .search_url   = met_search_url,
    .parse        = met_parse,
    .detail_url   = met_detail_url,
    .parse_detail = met_parse_detail
};
/*----------------------------------------------------------------------------*/
