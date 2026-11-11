/**
 * @file  web_nasa.c
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
 * @brief  NASA Image and Video Library search service.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "util/string_functions.h"
#include "url_data.h"
#include "web_util.h"
#include "web_nasa.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   NASA_MAX_RESULTS
 * @brief Number of first search results the service gives access to.
 */
constexpr int NASA_MAX_RESULTS = 10000;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the NASA image library search request url.
 *
 * @param[in] query        Search query
 * @param[in] search_opts  Not used, the service has no search options
 * @param[in] api_key      Not used, the service takes no key
 * @param[in] page         Results page number
 * @return    New string with url to free with free(), null when the page is
 *            beyond the results the service gives access to
 */
static char *
nasa_search_url (const char *query,
                 [[maybe_unused]] const char *search_opts,
                 [[maybe_unused]] const char *api_key,
                 const int   page)
{
    char *escaped = nullptr; /* Percent-encoded query */
    char *url     = nullptr; /* Result url */

    if (page < 1 || page * WEB_PAGE_SIZE > NASA_MAX_RESULTS)
        return nullptr;

    escaped = urldata_escape (query);
    url = g_strdup_printf (
            "https://images-api.nasa.gov/search?q=%s&media_type=image"
            "&page=%d&page_size=%d",
            escaped, page, WEB_PAGE_SIZE);
    free (escaped);

    return url;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find the thumbnail and the image to download among the links of
 *         a found image.
 *
 * The original image is taken when its format can be used as a wallpaper.
 * It can be a TIFF file, then the largest of the scaled copies is taken.
 *
 * @param[in]  links  Json array with image links, can be null
 * @param[out] thumb  Link with the thumbnail, null if there is none
 * @return     Link with the image to download, null if there is none in
 *             a format that can be used as a wallpaper
 */
static json_object *
nasa_find_links (json_object  *links,
                 json_object **thumb)
{
    json_object *link     = nullptr; /* Examined link */
    json_object *original = nullptr; /* Link with the original image */
    json_object *scaled   = nullptr; /* Link with the largest scaled copy */
    const char  *rel      = nullptr; /* Relation of examined link */
    size_t       cnt      = 0;
    int          width    = 0;       /* Width of examined image */
    int          scaled_w = -1;      /* Width of the largest scaled copy */

    *thumb = nullptr;

    if (links != nullptr)
        cnt = json_object_array_length (links);

    for (size_t i = 0; i < cnt; ++i) {
        link = json_object_array_get_idx (links, i);
        rel  = web_json_str (link, "rel");

        if (rel == nullptr)
            continue;

        if (strcmp (rel, "preview") == 0) {
            if (web_json_str (link, "href") != nullptr)
                *thumb = link;
            continue;
        }
        if (web_ext_from_url (web_json_str (link, "href")) == nullptr)
            continue;

        if (strcmp (rel, "canonical") == 0) {
            original = link;
        }
        else if (strcmp (rel, "alternate") == 0) {
            width = web_json_int (link, "width");
            if (width > scaled_w) {
                scaled   = link;
                scaled_w = width;
            }
        }
    }
    return original != nullptr ? original : scaled;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert a NASA image json object to SearchItem item.
 *
 * @param[in] obj  Json object to convert
 * @return    SearchItem item, null when the image can not be used
 */
static SearchItem *
nasa_json_obj_to_search_item (json_object *obj)
{
    SearchItem  *item   = nullptr;
    json_object *thumb  = nullptr; /* Link with thumbnail */
    json_object *image  = nullptr; /* Link with image to download */
    json_object *data   = nullptr; /* Image description */
    const char  *id     = nullptr; /* Image id */
    const char  *author = nullptr;
    char        *text    = nullptr; /* Built urls */
    char        *escaped = nullptr; /* Percent-encoded image id */

    data  = web_json_idx (web_json_get (obj, "data", json_type_array), 0);
    image = nasa_find_links (web_json_get (obj, "links", json_type_array),
                             &thumb);
    id    = web_json_str (data, "nasa_id");

    if (id == nullptr || image == nullptr || thumb == nullptr)
        return nullptr;

    if ((author = web_json_str (data, "photographer")) == nullptr &&
        (author = web_json_str (data, "secondary_creator")) == nullptr) {
        author = web_json_str (data, "center");
    }
    item = search_item_new ();
    search_item_set_id_string    (item, id);
    search_item_set_title        (item, web_json_str (data, "title"));
    search_item_set_author_name  (item, author);
    search_item_set_license_name (item, "NASA media usage guidelines");
    search_item_set_license_url  (item,
            "https://www.nasa.gov/nasa-brand-center/images-and-media/");
    search_item_set_width        (item, web_json_int (image, "width"));
    search_item_set_height       (item, web_json_int (image, "height"));

    /* Image ids and with them file addresses can have spaces */
    text = str_replace_in (web_json_str (image, "href"), " ", "%20");
    search_item_set_image_url (item, text);
    search_item_set_names (item, nasa_provider.name, web_ext_from_url (text));
    free (text);

    text = str_replace_in (web_json_str (thumb, "href"), " ", "%20");
    search_item_set_thumb_url (item, text);
    free (text);

    escaped = urldata_escape (id);
    text    = g_strconcat ("https://images.nasa.gov/details/", escaped,
                           nullptr);
    search_item_set_page_url (item, text);
    g_free (text);
    free (escaped);

    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the NASA image library search response.
 *
 * @param[in]  json       String with json data
 * @param[out] found_cnt  Number of found images
 * @param[in]  add_item   Function to pass found images to
 * @param[in]  user_data  Data for add_item
 * @return     false if json is not a search response of the service
 */
static bool
nasa_parse (const char  *json,
            int         *found_cnt,
            WebItemFunc  add_item,
            void        *user_data)
{
    json_object *obj  = web_json_parse (json);
    json_object *coll = web_json_get (obj, "collection", json_type_object);
    json_object *arr  = web_json_get (coll, "items", json_type_array);
    SearchItem  *item = nullptr;
    size_t       cnt  = 0;

    if (arr == nullptr) {
        if (obj != nullptr)
            json_object_put (obj);
        return false;
    }
    *found_cnt = web_json_int (web_json_get (coll, "metadata",
                                             json_type_object), "total_hits");
    cnt = json_object_array_length (arr);

    for (size_t i = 0; i < cnt; ++i) {
        item = nasa_json_obj_to_search_item (web_json_idx (arr, i));
        if (item != nullptr)
            add_item (item, user_data);
    }
    json_object_put (obj);

    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @var   nasa_provider
 * @brief NASA Image and Video Library search service.
 */
const WebProvider nasa_provider = {
    .name       = "NASA",
    .label      = "NASA Image Library",
    .opts_name  = "NASA_opts",
    .logo       = W_IMG_COUNT,
    .search_url = nasa_search_url,
    .parse      = nasa_parse
};
/*----------------------------------------------------------------------------*/
