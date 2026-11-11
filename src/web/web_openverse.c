/**
 * @file  web_openverse.c
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
 * @brief  Openverse image search service.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <glib.h>
#include "url_data.h"
#include "web_util.h"
#include "web_openverse.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   OPENVERSE_DEFAULT_LICENSES
 * @brief Licenses searched when the user has not chosen any: public domain
 *        and the licenses that only ask for attribution.
 *
 * @var   OPENVERSE_MAX_RESULTS
 * @brief Number of first search results available without an account.
 */
#define OPENVERSE_DEFAULT_LICENSES "cc0,pdm,by,by-sa"
constexpr int OPENVERSE_MAX_RESULTS = 240;
/*----------------------------------------------------------------------------*/
/**
 * @var   openverse_licenses
 * @brief Values of the license search option.
 *
 * @var   openverse_ratios
 * @brief Values of the aspect ratio search option.
 *
 * @var   openverse_sizes
 * @brief Values of the image size search option.
 *
 * @var   openverse_options
 * @brief Search options, their keys are the names of request parameters.
 */
static const WebChoice openverse_licenses[] = {
    { OPENVERSE_DEFAULT_LICENSES, "Public domain, CC BY, CC BY-SA" },
    { "cc0,pdm,by",               "Public domain, CC BY" },
    { "cc0,pdm",                  "Public domain only" },
    { }
};
static const WebChoice openverse_ratios[] = {
    { "",       "Any" },
    { "wide",   "Wide" },
    { "tall",   "Tall" },
    { "square", "Square" },
    { }
};
static const WebChoice openverse_sizes[] = {
    { "",       "Any" },
    { "large",  "Large" },
    { "medium", "Medium" },
    { "small",  "Small" },
    { }
};
static const WebOption openverse_options[] = {
    { .key     = "license",
      .label   = "Licenses:",
      .tooltip = "Licenses of the images to search for.",
      .type    = WEB_OPT_CHOICE,
      .choices = openverse_licenses },
    { .key     = "aspect_ratio",
      .label   = "Aspect ratio:",
      .type    = WEB_OPT_CHOICE,
      .choices = openverse_ratios },
    { .key     = "size",
      .label   = "Image size:",
      .type    = WEB_OPT_CHOICE,
      .choices = openverse_sizes },
    { }
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the Openverse search request url.
 *
 * @param[in] query        Search query
 * @param[in] search_opts  Search options
 * @param[in] api_key      Not used, the service is searched anonymously
 * @param[in] page         Results page number
 * @return    New string with url to free with free(), null when the page is
 *            beyond the results the service gives access to
 */
static char *
openverse_search_url (const char *query,
                      const char *search_opts,
                      [[maybe_unused]] const char *api_key,
                      const int   page)
{
    char *escaped = nullptr; /* Percent-encoded query */
    char *url     = nullptr; /* Result url */

    if (page < 1 || page * WEB_PAGE_SIZE > OPENVERSE_MAX_RESULTS)
        return nullptr;

    escaped = urldata_escape (query);
    url = g_strdup_printf (
            "https://api.openverse.org/v1/images/?q=%s&page=%d&page_size=%d"
            "%s%s",
            escaped, page, WEB_PAGE_SIZE,
            web_opts_has (search_opts, "license") ?
                "" : "&license=" OPENVERSE_DEFAULT_LICENSES,
            search_opts != nullptr ? search_opts : "");
    free (escaped);

    return url;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make a license name from Openverse license data.
 *
 * @param[in] license  License code, e.g. "by-sa", can be null
 * @param[in] version  License version, e.g. "4.0", can be null
 * @return    New string with the license name, e.g. "CC BY-SA 4.0";
 *            free with g_free()
 */
static char *
openverse_license_name (const char *license,
                        const char *version)
{
    char *upper = nullptr; /* Upper case license code */
    char *name  = nullptr; /* Result name */

    if (license == nullptr)
        return g_strdup ("See the image page");
    if (strcmp (license, "pdm") == 0)
        return g_strdup ("Public Domain Mark");

    upper = g_ascii_strup (license, -1);
    name  = g_strconcat ("CC ", upper,
                         version != nullptr ? " " : "",
                         version != nullptr ? version : "", nullptr);
    g_free (upper);

    return name;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the file extension of an Openverse image.
 *
 * @param[in] url       Image url
 * @param[in] filetype  Image file type from the response, can be null
 * @return    Extension with the dot, null for a format that can not be used
 *            as a wallpaper
 */
static const char *
openverse_image_ext (const char *url,
                     const char *filetype)
{
    const char *ext = web_ext_from_url (url);

    if (ext != nullptr || filetype == nullptr)
        /* Most images come without the file type and with an url that has
         * no extension, nearly all of them are jpeg files */
        return ext != nullptr ? ext : ".jpg";

    if (strcasecmp (filetype, "jpg") == 0 || strcasecmp (filetype, "jpeg") == 0)
        return ".jpg";
    if (strcasecmp (filetype, "png") == 0)
        return ".png";
    if (strcasecmp (filetype, "webp") == 0)
        return ".webp";

    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert an Openverse image json object to SearchItem item.
 *
 * @param[in] obj  Json object to convert
 * @return    SearchItem item, null when the image can not be used
 */
static SearchItem *
openverse_json_obj_to_search_item (json_object *obj)
{
    SearchItem *item    = nullptr;
    const char *id      = web_json_str (obj, "id");
    const char *url     = web_json_str (obj, "url");
    const char *thumb   = web_json_str (obj, "thumbnail");
    const char *ext     = nullptr; /* Image file extension */
    char       *license = nullptr; /* License name */

    if (id == nullptr || url == nullptr || thumb == nullptr)
        return nullptr;

    ext = openverse_image_ext (url, web_json_str (obj, "filetype"));
    if (ext == nullptr)
        return nullptr;

    license = openverse_license_name (web_json_str (obj, "license"),
                                      web_json_str (obj, "license_version"));
    item = search_item_new ();
    search_item_set_id_string    (item, id);
    search_item_set_title        (item, web_json_str (obj, "title"));
    search_item_set_author_name  (item, web_json_str (obj, "creator"));
    search_item_set_author_url   (item, web_json_str (obj, "creator_url"));
    search_item_set_license_name (item, license);
    search_item_set_license_url  (item, web_json_str (obj, "license_url"));
    search_item_set_image_url    (item, url);
    search_item_set_thumb_url    (item, thumb);
    search_item_set_page_url     (item,
                                  web_json_str (obj, "foreign_landing_url"));
    search_item_set_width        (item, web_json_int (obj, "width"));
    search_item_set_height       (item, web_json_int (obj, "height"));
    search_item_set_names        (item, openverse_provider.name, ext);

    g_free (license);

    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the Openverse search response.
 *
 * @param[in]  json       String with json data
 * @param[out] found_cnt  Number of found images
 * @param[in]  add_item   Function to pass found images to
 * @param[in]  user_data  Data for add_item
 * @return     false if json is not an Openverse search response
 */
static bool
openverse_parse (const char  *json,
                 int         *found_cnt,
                 WebItemFunc  add_item,
                 void        *user_data)
{
    json_object *obj  = web_json_parse (json);
    json_object *arr  = web_json_get (obj, "results", json_type_array);
    SearchItem  *item = nullptr;
    size_t       cnt  = 0;

    if (arr == nullptr) {
        if (obj != nullptr)
            json_object_put (obj);
        return false;
    }
    *found_cnt = web_json_int (obj, "result_count");
    cnt = json_object_array_length (arr);

    for (size_t i = 0; i < cnt; ++i) {
        item = openverse_json_obj_to_search_item (web_json_idx (arr, i));
        if (item != nullptr)
            add_item (item, user_data);
    }
    json_object_put (obj);

    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @var   openverse_provider
 * @brief Openverse image search service.
 */
const WebProvider openverse_provider = {
    .name       = "Openverse",
    .label      = "Openverse",
    .opts_name  = "Openverse_opts",
    .logo       = W_IMG_COUNT,
    .options    = openverse_options,
    .search_url = openverse_search_url,
    .parse      = openverse_parse
};
/*----------------------------------------------------------------------------*/
