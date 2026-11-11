/**
 * @file  web_commons.c
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
 * @brief  Wikimedia Commons image search service.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <glib.h>
#include "url_data.h"
#include "web_util.h"
#include "web_commons.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   COMMONS_MAX_RESULTS
 * @brief Number of first search results the Commons search gives access to.
 *
 * @var   COMMONS_THUMB_WIDTH
 * @brief Width of the requested thumbnails.
 */
constexpr int COMMONS_MAX_RESULTS = 10000;
constexpr int COMMONS_THUMB_WIDTH = 320;
/*----------------------------------------------------------------------------*/
/**
 * @var   commons_options
 * @brief Search options.
 */
static const WebOption commons_options[] = {
    { .key     = "minw",
      .label   = "Minimum width:",
      .tooltip = "Smallest image width in pixels. 0 allows every width.",
      .type    = WEB_OPT_INT,
      .max     = 20000 },
    { .key     = "minh",
      .label   = "Minimum height:",
      .tooltip = "Smallest image height in pixels. 0 allows every height.",
      .type    = WEB_OPT_INT,
      .max     = 20000 },
    { }
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build the Wikimedia Commons search request url.
 *
 * @param[in] query        Search query
 * @param[in] search_opts  Search options
 * @param[in] api_key      Not used, the service takes no key
 * @param[in] page         Results page number
 * @return    New string with url to free with free(), null when the page is
 *            beyond the results the service gives access to
 */
static char *
commons_search_url (const char *query,
                    const char *search_opts,
                    [[maybe_unused]] const char *api_key,
                    const int   page)
{
    GString *search  = nullptr; /* Search expression */
    char    *escaped = nullptr; /* Percent-encoded search expression */
    char    *url     = nullptr; /* Result url */
    int      offset  = (page - 1) * WEB_PAGE_SIZE;
    int      min_w   = web_opts_get_int (search_opts, "minw");
    int      min_h   = web_opts_get_int (search_opts, "minh");

    if (page < 1 || offset + WEB_PAGE_SIZE > COMMONS_MAX_RESULTS)
        return nullptr;

    /* Search keywords limit results to raster images of at least the
     * wanted size, "filew:>N" means wider than N */
    search = g_string_new ("filetype:bitmap ");
    if (min_w > 0)
        g_string_append_printf (search, "filew:>%d ", min_w - 1);
    if (min_h > 0)
        g_string_append_printf (search, "fileh:>%d ", min_h - 1);
    g_string_append (search, query);

    escaped = urldata_escape (search->str);
    g_string_free (search, TRUE);

    url = g_strdup_printf (
            "https://commons.wikimedia.org/w/api.php?action=query&format=json"
            "&formatversion=2&generator=search&gsrnamespace=6&gsrlimit=%d"
            "&gsroffset=%d&gsrinfo=totalhits&gsrsearch=%s&prop=imageinfo"
            "&iiprop=url%%7Csize%%7Cmime%%7Cextmetadata"
            "&iiextmetadatafilter=Artist%%7CLicenseShortName%%7CLicenseUrl"
            "&iiurlwidth=%d",
            WEB_PAGE_SIZE, offset, escaped, COMMONS_THUMB_WIDTH);
    free (escaped);

    return url;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make an image title from a Commons page title.
 *
 * @param[in] page_title  Page title, e.g. "File:Mountain lake.jpg"
 * @return    New string without the namespace and file extension, null when
 *            page_title is null; free with g_free()
 */
static char *
commons_image_title (const char *page_title)
{
    const char *name = page_title;
    const char *ext  = nullptr;

    if (page_title == nullptr)
        return nullptr;

    if (strncmp (name, "File:", 5) == 0)
        name += 5;

    ext = strrchr (name, '.');

    return (ext != nullptr && ext != name) ?
           g_strndup (name, (gsize) (ext - name)) : g_strdup (name);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the value of an image metadata field.
 *
 * @param[in] meta  Json object with image metadata, can be null
 * @param[in] name  Field name
 * @return    Field value owned by meta, null when it is not set
 */
static const char *
commons_meta_value (json_object *meta,
                    const char  *name)
{
    return web_json_str (web_json_get (meta, name, json_type_object),
                         "value");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert a Commons file page json object to SearchItem item.
 *
 * @param[in] page  Json object to convert
 * @return    SearchItem item, null when the file is not an image that can
 *            be used as a wallpaper
 */
static SearchItem *
commons_page_to_search_item (json_object *page)
{
    SearchItem  *item   = nullptr;
    json_object *info   = nullptr; /* Image file data */
    json_object *meta   = nullptr; /* Author and license data */
    const char  *ext    = nullptr; /* Image file extension */
    const char  *url    = nullptr; /* Image file url */
    const char  *thumb  = nullptr; /* Thumbnail url */
    const char  *lic    = nullptr; /* License name */
    char        *title  = nullptr;
    char        *author = nullptr;
    int          pageid = web_json_int (page, "pageid");
    char         id[16];

    info  = web_json_idx (web_json_get (page, "imageinfo", json_type_array),
                          0);
    meta  = web_json_get (info, "extmetadata", json_type_object);
    ext   = web_ext_from_mime (web_json_str (info, "mime"));
    url   = web_json_str (info, "url");
    thumb = web_json_str (info, "thumburl");

    if (pageid <= 0 || ext == nullptr || url == nullptr || thumb == nullptr)
        return nullptr;

    snprintf (id, sizeof (id), "%d", pageid);
    title  = commons_image_title (web_json_str (page, "title"));
    /* The author field is a piece of html, usually with a link */
    author = web_strip_html (commons_meta_value (meta, "Artist"));
    lic    = commons_meta_value (meta, "LicenseShortName");

    item = search_item_new ();
    search_item_set_id_string    (item, id);
    search_item_set_title        (item, title);
    search_item_set_author_name  (item, author);
    search_item_set_license_name (item,
                                  lic != nullptr ? lic : "See the file page");
    search_item_set_license_url  (item, commons_meta_value (meta,
                                                            "LicenseUrl"));
    search_item_set_image_url    (item, url);
    search_item_set_thumb_url    (item, thumb);
    search_item_set_page_url     (item, web_json_str (info, "descriptionurl"));
    search_item_set_width        (item, web_json_int (info, "width"));
    search_item_set_height       (item, web_json_int (info, "height"));
    search_item_set_names        (item, commons_provider.name, ext);

    g_free (title);
    free (author);

    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare two Commons file pages by their place in search results.
 *
 * @param[in] a  Pointer to json object of the first page
 * @param[in] b  Pointer to json object of the second page
 * @return    Compare result for qsort
 */
static int
commons_compare_pages (const void *a,
                       const void *b)
{
    int index_a = web_json_int (*(json_object *const *) a, "index");
    int index_b = web_json_int (*(json_object *const *) b, "index");

    return (index_a > index_b) - (index_a < index_b);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse the Wikimedia Commons search response.
 *
 * @param[in]  json       String with json data
 * @param[out] found_cnt  Number of found images
 * @param[in]  add_item   Function to pass found images to
 * @param[in]  user_data  Data for add_item
 * @return     false if json is not a Commons search response
 */
static bool
commons_parse (const char  *json,
               int         *found_cnt,
               WebItemFunc  add_item,
               void        *user_data)
{
    json_object  *obj    = web_json_parse (json);
    json_object  *query  = web_json_get (obj, "query", json_type_object);
    json_object  *arr    = web_json_get (query, "pages", json_type_array);
    json_object **pages  = nullptr; /* Pages in search results order */
    SearchItem   *item   = nullptr;
    size_t        cnt    = 0;

    /* A search without results gives a response with no "query" member */
    if (obj == nullptr ||
        web_json_get (obj, "error", json_type_object) != nullptr ||
        (query == nullptr &&
         web_json_get (obj, "batchcomplete", json_type_boolean) == nullptr)) {

        if (obj != nullptr)
            json_object_put (obj);
        return false;
    }
    *found_cnt = web_json_int (web_json_get (query, "searchinfo",
                                             json_type_object), "totalhits");
    if (arr != nullptr)
        cnt = json_object_array_length (arr);

    if (cnt > 0) {
        /* Pages come ordered by page id, "index" is the search result rank */
        if ((pages = malloc (cnt * sizeof (json_object *))) == nullptr)
            err (EXIT_FAILURE, nullptr);

        for (size_t i = 0; i < cnt; ++i)
            pages[i] = json_object_array_get_idx (arr, i);

        qsort (pages, cnt, sizeof (json_object *), commons_compare_pages);

        for (size_t i = 0; i < cnt; ++i) {
            if ((item = commons_page_to_search_item (pages[i])) != nullptr)
                add_item (item, user_data);
        }
        free (pages);
    }
    json_object_put (obj);

    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @var   commons_provider
 * @brief Wikimedia Commons image search service.
 */
const WebProvider commons_provider = {
    .name       = "Commons",
    .label      = "Wikimedia Commons",
    .opts_name  = "Commons_opts",
    .logo       = W_IMG_COUNT,
    .options    = commons_options,
    .search_url = commons_search_url,
    .parse      = commons_parse
};
/*----------------------------------------------------------------------------*/
