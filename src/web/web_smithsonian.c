/**
 * @file  web_smithsonian.c
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
 * @brief  Smithsonian Open Access image search service.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "url_data.h"
#include "web_util.h"
#include "web_smithsonian.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   SMITHSONIAN_DEMO_KEY
 * @brief Public api.data.gov key with low request limits, used when the user
 *        has not set an own key.
 *
 * @def   SMITHSONIAN_IMAGE_URL
 * @brief Address of the image delivery service, it takes an image id and
 *        sends the image scaled to the wanted size.
 *
 * @var   SMITHSONIAN_THUMB_SIZE
 * @brief Length of the longer side of the requested thumbnails.
 */
#define SMITHSONIAN_DEMO_KEY  "DEMO_KEY"
#define SMITHSONIAN_IMAGE_URL "https://ids.si.edu/ids/deliveryService"
constexpr int SMITHSONIAN_THUMB_SIZE = 300;
/*----------------------------------------------------------------------------*/
/**
 * @var   smithsonian_units
 * @brief Values of the collection search option: codes of the museums whose
 *        collections are mostly artworks and photographs.
 *
 * @var   smithsonian_options
 * @brief Search options.
 */
static const WebChoice smithsonian_units[] = {
    { "",      "All Smithsonian collections" },
    { "SAAM",  "American Art Museum" },
    { "NPG",   "National Portrait Gallery" },
    { "FSG",   "National Museum of Asian Art" },
    { "CHNDM", "Cooper Hewitt Design Museum" },
    { "HMSG",  "Hirshhorn Museum" },
    { "NASM",  "Air and Space Museum" },
    { }
};
static const WebOption smithsonian_options[] = {
    { .key     = "unit",
      .label   = "Collection:",
      .tooltip = "Museum to search. The whole Smithsonian includes "
                 "natural history specimens and archive documents.",
      .type    = WEB_OPT_CHOICE,
      .choices = smithsonian_units },
    { }
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the code of the museum chosen in search options.
 *
 * @param[in] search_opts  Search options string, can be null
 * @return    Museum code, null when the whole Smithsonian is searched
 */
static const char *
smithsonian_unit (const char *search_opts)
{
    char key[32]; /* Option as it stands in the options string */

    /* The code comes from a saved setting, only the known ones are used */
    for (int i = 1; smithsonian_units[i].value != nullptr; ++i) {
        snprintf (key, sizeof (key), "&unit=%s", smithsonian_units[i].value);
        if (search_opts != nullptr && strstr (search_opts, key) != nullptr)
            return smithsonian_units[i].value;
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the Smithsonian Open Access search request url.
 *
 * @param[in] query        Search query
 * @param[in] search_opts  Search options
 * @param[in] api_key      api.data.gov key, when empty the demo key is used
 * @param[in] page         Results page number
 * @return    New string with url to free with free(), null for a wrong page
 */
static char *
smithsonian_search_url (const char *query,
                        const char *search_opts,
                        const char *api_key,
                        const int   page)
{
    const char *unit  = smithsonian_unit (search_opts);
    char *search      = nullptr; /* Search expression */
    char *escaped     = nullptr; /* Percent-encoded search expression */
    char *escaped_key = nullptr; /* Percent-encoded API key */
    char *url         = nullptr; /* Result url */

    if (page < 1)
        return nullptr;

    search  = g_strconcat (query, " AND online_media_type:\"Images\"",
                           unit != nullptr ? " AND unit_code:" : "",
                           unit != nullptr ? unit : "",
                           nullptr);
    escaped = urldata_escape (search);
    g_free (search);

    escaped_key = urldata_escape (
            (api_key != nullptr && api_key[0] != '\0') ?
            api_key : SMITHSONIAN_DEMO_KEY);

    url = g_strdup_printf (
            "https://api.si.edu/openaccess/api/v1.0/search?q=%s&start=%d"
            "&rows=%d&api_key=%s",
            escaped, (page - 1) * WEB_PAGE_SIZE, WEB_PAGE_SIZE, escaped_key);
    free (escaped);
    free (escaped_key);

    return url;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find the first CC0 image among the media of a found object.
 *
 * @param[in] media  Json array with object's media, can be null
 * @return    Json object of the image, null if the object has none
 */
static json_object *
smithsonian_find_image (json_object *media)
{
    json_object *image  = nullptr; /* Examined media item */
    const char  *type   = nullptr;
    const char  *access = nullptr;
    size_t       cnt    = 0;

    if (media != nullptr)
        cnt = json_object_array_length (media);

    for (size_t i = 0; i < cnt; ++i) {
        image  = json_object_array_get_idx (media, i);
        type   = web_json_str (image, "type");
        access = web_json_str (web_json_get (image, "usage", json_type_object),
                               "access");

        if (type != nullptr && strcmp (type, "Images") == 0 &&
            access != nullptr && strcmp (access, "CC0") == 0 &&
            web_json_str (image, "content") != nullptr) {
            return image;
        }
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find the high resolution JPEG file among the files of an image.
 *
 * @param[in] image  Json object of the image
 * @return    Json object of the file, null if the image has no such file
 */
static json_object *
smithsonian_find_jpeg (json_object *image)
{
    json_object *files = web_json_get (image, "resources", json_type_array);
    json_object *file  = nullptr; /* Examined file */
    const char  *label = nullptr;
    size_t       cnt   = 0;

    if (files != nullptr)
        cnt = json_object_array_length (files);

    for (size_t i = 0; i < cnt; ++i) {
        file  = json_object_array_get_idx (files, i);
        label = web_json_str (file, "label");

        if (label != nullptr && strcmp (label, "High-resolution JPEG") == 0 &&
            web_json_str (file, "url") != nullptr) {
            return file;
        }
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert a Smithsonian object json object to SearchItem item.
 *
 * @param[in] obj  Json object to convert
 * @return    SearchItem item, null when the object has no CC0 image
 */
static SearchItem *
smithsonian_json_obj_to_search_item (json_object *obj)
{
    SearchItem  *item    = nullptr;
    json_object *content = web_json_get (obj, "content", json_type_object);
    json_object *descr   = nullptr; /* Object description */
    json_object *image   = nullptr; /* First CC0 image of the object */
    json_object *jpeg    = nullptr; /* High resolution file of the image */
    json_object *names   = nullptr; /* People connected with the object */
    const char  *id      = nullptr; /* Image id */
    const char  *url     = nullptr; /* Image delivery url */
    const char  *page    = nullptr; /* Object page url */
    char        *thumb   = nullptr; /* Thumbnail url */
    char        *escaped = nullptr; /* Percent-encoded image id */

    descr = web_json_get (content, "descriptiveNonRepeating",
                          json_type_object);
    image = smithsonian_find_image (
                web_json_get (web_json_get (descr, "online_media",
                                            json_type_object),
                              "media", json_type_array));
    if (image == nullptr)
        return nullptr;

    if ((id = web_json_str (image, "idsId")) == nullptr)
        return nullptr;
    url   = web_json_str (image, "content");
    jpeg  = smithsonian_find_jpeg (image);
    names = web_json_get (web_json_get (content, "freetext",
                                        json_type_object),
                          "name", json_type_array);

    if ((page = web_json_str (descr, "record_link")) == nullptr)
        page = web_json_str (descr, "guid");

    item = search_item_new ();
    search_item_set_id_string    (item, id);
    search_item_set_title        (item, web_json_str (obj, "title"));
    search_item_set_author_name  (item,
            web_json_str (web_json_idx (names, 0), "content"));
    search_item_set_license_name (item, "CC0 1.0");
    search_item_set_license_url  (item,
            "https://creativecommons.org/publicdomain/zero/1.0/");
    search_item_set_page_url     (item, page);

    if (jpeg != nullptr) {
        search_item_set_image_url (item, web_json_str (jpeg, "url"));
        search_item_set_width     (item, web_json_int (jpeg, "width"));
        search_item_set_height    (item, web_json_int (jpeg, "height"));
    }
    else {
        search_item_set_image_url (item, url);
    }
    if (item->width <= 0 || item->height <= 0) {
        search_item_set_width  (item, web_json_int (image, "width"));
        search_item_set_height (item, web_json_int (image, "height"));
    }
    /* The thumbnail from the response is too small to show */
    escaped = urldata_escape (id);
    thumb   = g_strdup_printf (SMITHSONIAN_IMAGE_URL "?id=%s&max=%d",
                               escaped, SMITHSONIAN_THUMB_SIZE);
    search_item_set_thumb_url (item, thumb);
    g_free (thumb);
    free (escaped);
    search_item_set_names (item, smithsonian_provider.name, ".jpg");

    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the Smithsonian Open Access search response.
 *
 * @param[in]  json       String with json data
 * @param[out] found_cnt  Number of found objects
 * @param[in]  add_item   Function to pass found images to
 * @param[in]  user_data  Data for add_item
 * @return     false if json is not a search response of the service
 */
static bool
smithsonian_parse (const char  *json,
                   int         *found_cnt,
                   WebItemFunc  add_item,
                   void        *user_data)
{
    json_object *obj  = web_json_parse (json);
    json_object *resp = web_json_get (obj, "response", json_type_object);
    json_object *arr  = web_json_get (resp, "rows", json_type_array);
    SearchItem  *item = nullptr;
    size_t       cnt  = 0;

    if (arr == nullptr) {
        if (obj != nullptr)
            json_object_put (obj);
        return false;
    }
    *found_cnt = web_json_int (resp, "rowCount");
    cnt = json_object_array_length (arr);

    for (size_t i = 0; i < cnt; ++i) {
        item = smithsonian_json_obj_to_search_item (web_json_idx (arr, i));
        if (item != nullptr)
            add_item (item, user_data);
    }
    json_object_put (obj);

    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @var   smithsonian_provider
 * @brief Smithsonian Open Access image search service.
 */
const WebProvider smithsonian_provider = {
    .name         = "Smithsonian",
    .label        = "Smithsonian Open Access",
    .opts_name    = "Smithsonian_opts",
    .api_key_name = "Smithsonian api",
    .api_key_info = "The Smithsonian API needs a free api.data.gov key. "
                    "Without your own key the program uses the public "
                    "DEMO_KEY, which allows only a few dozen searches per "
                    "hour. You can sign up for a key at:",
    .api_key_url  = "https://api.data.gov/signup/",
    .logo         = W_IMG_COUNT,
    .options      = smithsonian_options,
    .search_url   = smithsonian_search_url,
    .parse        = smithsonian_parse
};
/*----------------------------------------------------------------------------*/
