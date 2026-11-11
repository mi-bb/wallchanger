/**
 * @file  test_web_parse.c
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
 * @brief  Unit tests for the image search services (web/ directory).
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "web/search_item.h"
#include "web/web_provider.h"
#include "web/web_util.h"
#include "web/web_commons.h"
#include "web/web_openverse.h"
#include "web/web_artic.h"
#include "web/web_met.h"
#include "web/web_nasa.h"
#include "web/web_smithsonian.h"
/*----------------------------------------------------------------------------*/
/* Most images a test response holds */
constexpr int MAX_ITEMS = 16;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Images found by a parsed response.
 */
typedef struct Found {
    SearchItem *items[MAX_ITEMS];
    int         count;
} Found;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Collect an image passed by a parse function.
 */
static void
found_add (SearchItem *item,
           void       *user_data)
{
    Found *found = user_data;

    ck_assert_int_lt (found->count, MAX_ITEMS);
    found->items[found->count++] = item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free collected images.
 */
static void
found_free (Found *found)
{
    for (int i = 0; i < found->count; ++i)
        search_item_free (found->items[i]);
    found->count = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read a response saved in the tests data directory.
 *
 * Responses are trimmed captures of real ones, extended with entries marked
 * "Synthetic" for the cases the captured data does not have.
 */
static char *
read_response (const char *name)
{
    char   path[1024];
    char  *data = nullptr;
    FILE  *file = nullptr;
    long   size = 0;

    snprintf (path, sizeof (path), "%s/%s", TEST_DATA_DIR, name);

    file = fopen (path, "rb");
    ck_assert_msg (file != nullptr, "can not open %s", path);
    ck_assert_int_eq (fseek (file, 0, SEEK_END), 0);
    size = ftell (file);
    ck_assert_int_ge (size, 0);
    rewind (file);

    data = malloc ((size_t) size + 1);
    ck_assert_ptr_nonnull (data);
    ck_assert_uint_eq (fread (data, 1, (size_t) size, file), (size_t) size);
    data[size] = '\0';
    fclose (file);

    return data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse a saved response with the service's parse function.
 */
static int
parse_response (const WebProvider *provider,
                const char        *name,
                Found             *found)
{
    char *json      = read_response (name);
    int   found_cnt = -1;

    found->count = 0;
    ck_assert (provider->parse (json, &found_cnt, found_add, found));
    free (json);

    return found_cnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check that every service rejects a response it can not read.
 */
static void
assert_parse_rejects (const char *json)
{
    const WebProvider *providers[] = {
        &commons_provider, &openverse_provider, &artic_provider,
        &met_provider, &nasa_provider, &smithsonian_provider
    };
    Found found     = { .count = 0 };
    int   found_cnt = 0;

    for (size_t i = 0; i < sizeof (providers) / sizeof (providers[0]); ++i) {
        ck_assert_msg (!providers[i]->parse (json, &found_cnt,
                                             found_add, &found),
                       "%s accepted: %s", providers[i]->name, json);
        ck_assert_int_eq (found.count, 0);
    }
}
/*----------------------------------------------------------------------------*/
/* Util */
/*----------------------------------------------------------------------------*/
START_TEST (test_strip_html)
{
    char *text = nullptr;

    text = web_strip_html ("<a href=\"//x.org/u\" title=\"User\">Jane  Doe</a>");
    ck_assert_str_eq (text, "Jane Doe");
    free (text);

    /* Tags separate words, entities are decoded */
    text = web_strip_html ("Tom &amp; Jerry<br>O&#039;Neil &lt;3\n");
    ck_assert_str_eq (text, "Tom & Jerry O'Neil <3");
    free (text);

    /* Unknown entity and a lone ampersand stay as they are */
    text = web_strip_html ("A &copy; B & C");
    ck_assert_str_eq (text, "A &copy; B & C");
    free (text);

    ck_assert_ptr_null (web_strip_html (nullptr));
    ck_assert_ptr_null (web_strip_html ("<span> </span>"));
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_ext_from_url)
{
    ck_assert_str_eq (web_ext_from_url ("https://x.org/a/b.jpg"), ".jpg");
    ck_assert_str_eq (web_ext_from_url ("https://x.org/a/b.JPEG"), ".jpg");
    ck_assert_str_eq (web_ext_from_url ("https://x.org/b.png?utm=1"), ".png");
    ck_assert_str_eq (web_ext_from_url ("https://x.org/b.webp#top"), ".webp");
    /* The dot of the host name is not an extension */
    ck_assert_ptr_null (web_ext_from_url ("https://x.org/download?id=7"));
    ck_assert_ptr_null (web_ext_from_url ("https://x.org/scan.tif"));
    ck_assert_ptr_null (web_ext_from_url (".jpg"));
    ck_assert_ptr_null (web_ext_from_url (nullptr));
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_ext_from_mime)
{
    ck_assert_str_eq (web_ext_from_mime ("image/jpeg"), ".jpg");
    ck_assert_str_eq (web_ext_from_mime ("image/png"), ".png");
    ck_assert_str_eq (web_ext_from_mime ("image/webp"), ".webp");
    ck_assert_ptr_null (web_ext_from_mime ("image/tiff"));
    ck_assert_ptr_null (web_ext_from_mime ("image/svg+xml"));
    ck_assert_ptr_null (web_ext_from_mime (nullptr));
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_opts)
{
    const char *opts = "&minw=1920&size=large&minh=1080";

    ck_assert_int_eq (web_opts_get_int (opts, "minw"), 1920);
    ck_assert_int_eq (web_opts_get_int (opts, "minh"), 1080);
    /* Not a number, a part of another name, not set */
    ck_assert_int_eq (web_opts_get_int (opts, "size"), 0);
    ck_assert_int_eq (web_opts_get_int (opts, "inw"), 0);
    ck_assert_int_eq (web_opts_get_int (opts, "depth"), 0);
    ck_assert_int_eq (web_opts_get_int ("", "minw"), 0);
    ck_assert_int_eq (web_opts_get_int (nullptr, "minw"), 0);
    ck_assert_int_eq (web_opts_get_int ("&minw=-5", "minw"), 0);

    ck_assert (web_opts_has (opts, "size"));
    ck_assert (!web_opts_has (opts, "siz"));
    ck_assert (!web_opts_has (nullptr, "size"));
}
END_TEST
/*----------------------------------------------------------------------------*/
/* SearchItem */
/*----------------------------------------------------------------------------*/
START_TEST (test_item_id_is_file_name_safe)
{
    SearchItem *item = search_item_new ();

    search_item_set_id_string (item, "ark:/65665/m3 db.62");
    ck_assert_str_eq (item->id, "ark__65665_m3_db_62");

    search_item_set_id_string (item, "SAAM-1986_78");
    ck_assert_str_eq (item->id, "SAAM-1986_78");

    search_item_set_id_string (item, nullptr);
    ck_assert_ptr_null (item->id);

    search_item_free (item);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_item_names)
{
    SearchItem *item = search_item_new ();

    /* Without a title the id is shown, without a size only the name */
    search_item_set_id_string (item, "abc123");
    search_item_set_names (item, "Service", ".png");
    ck_assert_str_eq (item->service_name, "Service");
    ck_assert_str_eq (item->file_name, "Service_abc123.png");
    ck_assert_str_eq (item->display_name, "abc123");
    ck_assert_str_eq (item->display_markup, "abc123");

    /* Title is shown with markup characters escaped, size is added */
    search_item_set_title (item, "Cats & <dogs>");
    search_item_set_width (item, 1920);
    search_item_set_height (item, 1080);
    search_item_set_names (item, "Service", ".jpg");
    ck_assert_str_eq (item->file_name, "Service_abc123.jpg");
    ck_assert_str_eq (item->display_name, "Cats & <dogs>");
    ck_assert_str_eq (item->display_markup,
            "Cats &amp; &lt;dogs&gt;\n"
            "<span font_size=\"small\">[1920x1080]</span>");

    /* A long title is cut on a character, not a byte, boundary */
    search_item_set_title (item,
            "ŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁ");
    search_item_set_names (item, "Service", ".jpg");
    ck_assert_str_eq (item->display_name,
            "ŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁŁ...");

    search_item_free (item);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_item_names_without_id)
{
    SearchItem *item = search_item_new ();

    search_item_set_title (item, "Title");
    search_item_set_names (item, "Service", ".jpg");
    ck_assert_str_eq (item->service_name, "Service");
    ck_assert_ptr_null (item->file_name);
    ck_assert_ptr_null (item->display_name);

    search_item_free (item);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* Urls */
/*----------------------------------------------------------------------------*/
START_TEST (test_url_query_is_escaped)
{
    const WebProvider *providers[] = {
        &commons_provider, &openverse_provider, &artic_provider,
        &met_provider, &nasa_provider, &smithsonian_provider
    };
    char *url = nullptr;

    for (size_t i = 0; i < sizeof (providers) / sizeof (providers[0]); ++i) {
        url = providers[i]->search_url ("łódź & co", "", "", 1);
        ck_assert_ptr_nonnull (url);
        ck_assert_msg (strstr (url, "%C5%82%C3%B3d%C5%BA%20%26%20co") != nullptr,
                       "%s: %s", providers[i]->name, url);
        ck_assert_msg (strchr (url, ' ') == nullptr, "%s", url);
        free (url);

        /* Pages are counted from 1 */
        ck_assert_ptr_null (providers[i]->search_url ("cat", "", "", 0));
    }
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_url_paging)
{
    char *url = nullptr;

    url = commons_provider.search_url ("cat", "", "", 3);
    ck_assert_ptr_nonnull (strstr (url, "&gsrlimit=20&gsroffset=40&"));
    free (url);
    url = commons_provider.search_url ("cat", "", "", 500);
    ck_assert_ptr_nonnull (url);
    free (url);
    ck_assert_ptr_null (commons_provider.search_url ("cat", "", "", 501));

    url = met_provider.search_url ("cat", "", "", 3);
    ck_assert_ptr_nonnull (strstr (url, "/v1.1/search?"));
    ck_assert_ptr_nonnull (strstr (url, "&offset=40&limit=20"));
    free (url);
    ck_assert_ptr_null (met_provider.search_url ("cat", "", "", 501));

    url = artic_provider.search_url ("cat", "", "", 50);
    ck_assert_ptr_nonnull (strstr (url, "&page=50&limit=20"));
    ck_assert_ptr_nonnull (strstr (
            url, "query%5Bterm%5D%5Bis_public_domain%5D=true"));
    free (url);
    ck_assert_ptr_null (artic_provider.search_url ("cat", "", "", 51));

    url = openverse_provider.search_url ("cat", "", "", 12);
    ck_assert_ptr_nonnull (strstr (url, "&page=12&page_size=20"));
    free (url);
    ck_assert_ptr_null (openverse_provider.search_url ("cat", "", "", 13));

    url = nasa_provider.search_url ("cat", "", "", 500);
    ck_assert_ptr_nonnull (strstr (url, "&page=500&page_size=20"));
    free (url);
    ck_assert_ptr_null (nasa_provider.search_url ("cat", "", "", 501));

    url = smithsonian_provider.search_url ("cat", "", "", 3);
    ck_assert_ptr_nonnull (strstr (url, "&start=40&rows=20&"));
    free (url);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_url_options)
{
    char *url = nullptr;

    /* Commons: minimum size becomes "larger than" search keywords */
    url = commons_provider.search_url ("cat", "&minw=1920&minh=1080", "", 1);
    ck_assert_ptr_nonnull (strstr (
            url, "gsrsearch=filetype%3Abitmap%20filew%3A%3E1919%20"
                 "fileh%3A%3E1079%20cat&"));
    free (url);
    url = commons_provider.search_url ("cat", "", "", 1);
    ck_assert_ptr_nonnull (strstr (url, "gsrsearch=filetype%3Abitmap%20cat&"));
    free (url);

    /* Openverse: licenses are always limited */
    url = openverse_provider.search_url ("cat", "", "", 1);
    ck_assert_ptr_nonnull (strstr (url, "&license=cc0,pdm,by,by-sa"));
    free (url);
    url = openverse_provider.search_url ("cat", "&size=large", "", 1);
    ck_assert_ptr_nonnull (strstr (url, "&license=cc0,pdm,by,by-sa"));
    ck_assert_ptr_nonnull (strstr (url, "&size=large"));
    free (url);
    url = openverse_provider.search_url ("cat", "&license=cc0,pdm", "", 1);
    ck_assert_ptr_nonnull (strstr (url, "&license=cc0,pdm"));
    ck_assert_ptr_null (strstr (url, "by-sa"));
    free (url);

    /* Smithsonian: demo key without the user's own, museum filter */
    url = smithsonian_provider.search_url ("cat", "", "", 1);
    ck_assert_ptr_nonnull (strstr (url, "&api_key=DEMO_KEY"));
    ck_assert_ptr_nonnull (strstr (
            url, "q=cat%20AND%20online_media_type%3A%22Images%22&"));
    free (url);
    url = smithsonian_provider.search_url ("cat", "&unit=SAAM", "my key", 1);
    ck_assert_ptr_nonnull (strstr (url, "&api_key=my%20key"));
    ck_assert_ptr_nonnull (strstr (url, "%20AND%20unit_code%3ASAAM&"));
    free (url);
    /* Only the known museum codes get into the search expression */
    url = smithsonian_provider.search_url ("cat", "&unit=X OR 1", "", 1);
    ck_assert_ptr_null (strstr (url, "unit_code"));
    free (url);
}
END_TEST
/*----------------------------------------------------------------------------*/
/* Parse */
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_rejects_bad_response)
{
    assert_parse_rejects ("");
    assert_parse_rejects ("<html>Gateway Time-out</html>");
    assert_parse_rejects ("[1, 2, 3]");
    assert_parse_rejects ("{}");
    assert_parse_rejects ("{\"error\":{\"code\":\"badvalue\"}}");
    assert_parse_rejects ("{\"detail\":\"Request was throttled.\"}");
    assert_parse_rejects ("{\"message\":\"Not Found\"}");
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_commons)
{
    Found       found;
    SearchItem *item = nullptr;

    ck_assert_int_eq (parse_response (&commons_provider,
                                      "commons_search.json", &found), 68750);
    /* The TIFF file is left out, the rest comes in search rank order */
    ck_assert_int_eq (found.count, 4);
    ck_assert_str_eq (found.items[0]->id, "134682226");
    ck_assert_str_eq (found.items[1]->id, "154986747");
    ck_assert_str_eq (found.items[2]->id, "99421650");
    ck_assert_str_eq (found.items[3]->id, "900000002");

    item = found.items[0];
    ck_assert_str_eq (item->service_name, "Commons");
    ck_assert_str_eq (item->title,
            "Mountains in snow, Mountain lake, Chola Valley, Nepal, Himalayas");
    ck_assert_str_eq (item->author_name, "Vyacheslav Argenberg");
    ck_assert_str_eq (item->license_name, "CC BY 4.0");
    ck_assert_str_eq (item->license_url,
                      "https://creativecommons.org/licenses/by/4.0");
    ck_assert_str_eq (item->file_name, "Commons_134682226.jpg");
    ck_assert_int_eq (item->width, 4032);
    ck_assert_int_eq (item->height, 2688);
    ck_assert_ptr_nonnull (strstr (item->image_url,
                                   "https://upload.wikimedia.org/"));
    ck_assert_ptr_nonnull (strstr (item->thumb_url, "/330px-"));
    ck_assert_ptr_nonnull (strstr (item->page_url,
                                   "https://commons.wikimedia.org/wiki/File:"));

    /* No author and license data, extension from the media type */
    item = found.items[3];
    ck_assert_str_eq (item->title, "Synthetic no metadata");
    ck_assert_ptr_null (item->author_name);
    ck_assert_str_eq (item->license_name, "See the file page");
    ck_assert_ptr_null (item->license_url);
    ck_assert_str_eq (item->file_name, "Commons_900000002.png");

    found_free (&found);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_commons_no_results)
{
    Found found     = { .count = 0 };
    int   found_cnt = -1;

    ck_assert (commons_provider.parse ("{\"batchcomplete\":true}",
                                       &found_cnt, found_add, &found));
    ck_assert_int_eq (found_cnt, 0);
    ck_assert_int_eq (found.count, 0);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_openverse)
{
    Found       found;
    SearchItem *item = nullptr;

    ck_assert_int_eq (parse_response (&openverse_provider,
                                      "openverse_search.json", &found), 240);
    /* The gif is left out */
    ck_assert_int_eq (found.count, 2);

    item = found.items[0];
    ck_assert_str_eq (item->id, "c25a8620-4fdc-4ef7-b263-3573ce0065e8");
    ck_assert_str_eq (item->title, "mountain");
    ck_assert_str_eq (item->author_name, "Kiwi Tom");
    ck_assert_str_eq (item->author_url,
                      "https://www.flickr.com/photos/127665714@N08");
    ck_assert_str_eq (item->license_name, "CC BY 2.0");
    ck_assert_str_eq (item->license_url,
                      "https://creativecommons.org/licenses/by/2.0/");
    ck_assert_str_eq (item->page_url,
            "https://www.flickr.com/photos/127665714@N08/15692571170");
    ck_assert_str_eq (item->file_name,
            "Openverse_c25a8620-4fdc-4ef7-b263-3573ce0065e8.jpg");
    ck_assert_int_eq (item->width, 1024);
    ck_assert_int_eq (item->height, 444);

    /* Extension from the file type, no author, unknown size */
    item = found.items[1];
    ck_assert_str_eq (item->license_name, "Public Domain Mark");
    ck_assert_ptr_null (item->author_name);
    ck_assert_ptr_nonnull (strstr (item->file_name, ".png"));
    ck_assert_int_eq (item->width, 0);
    ck_assert_str_eq (item->display_markup, "Synthetic public domain image");

    found_free (&found);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_artic)
{
    Found       found;
    SearchItem *item = nullptr;

    ck_assert_int_eq (parse_response (&artic_provider,
                                      "artic_search.json", &found), 62119);
    /* Two of three artworks have no image */
    ck_assert_int_eq (found.count, 1);

    item = found.items[0];
    ck_assert_str_eq (item->id, "85885");
    ck_assert_str_eq (item->title, "View of Mountain Lake, n.d.");
    ck_assert_str_eq (item->author_name, "James \"Drunken\" Robertson");
    ck_assert_str_eq (item->license_name, "CC0 1.0");
    ck_assert_str_eq (item->file_name, "ArtIC_85885.jpg");
    ck_assert_str_eq (item->page_url, "https://www.artic.edu/artworks/85885");
    ck_assert_str_eq (item->image_url,
            "https://www.artic.edu/iiif/2/"
            "42c88a5b-bd3d-460e-8bd2-49444b36f84b/full/3000,/0/default.jpg");
    ck_assert_str_eq (item->thumb_url,
            "https://www.artic.edu/iiif/2/"
            "42c88a5b-bd3d-460e-8bd2-49444b36f84b/full/200,/0/default.jpg");
    ck_assert_int_eq (item->width, 3000);
    ck_assert_int_eq (item->height, 1961);

    found_free (&found);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_artic_image_size)
{
    const char *json =
        "{\"pagination\":{\"total\":2},\"data\":["
        "{\"id\":1,\"title\":\"Large\",\"image_id\":\"aa\","
        " \"thumbnail\":{\"width\":6000,\"height\":4000}},"
        "{\"id\":2,\"title\":\"Small\",\"image_id\":\"bb\","
        " \"thumbnail\":{\"width\":948,\"height\":768}},"
        "{\"id\":3,\"title\":\"Unknown\",\"image_id\":\"cc\"}]}";
    Found found     = { .count = 0 };
    int   found_cnt = -1;

    ck_assert (artic_provider.parse (json, &found_cnt, found_add, &found));
    ck_assert_int_eq (found_cnt, 2);
    ck_assert_int_eq (found.count, 3);

    /* Larger than the server sends: downloaded scaled down */
    ck_assert_ptr_nonnull (strstr (found.items[0]->image_url,
                                   "/aa/full/3000,/0/default.jpg"));
    ck_assert_int_eq (found.items[0]->width, 3000);
    ck_assert_int_eq (found.items[0]->height, 2000);
    ck_assert_str_eq (found.items[0]->title, "Large");

    /* Smaller: downloaded in its own size, never scaled up */
    ck_assert_ptr_nonnull (strstr (found.items[1]->image_url,
                                   "/bb/full/948,/0/default.jpg"));
    ck_assert_int_eq (found.items[1]->width, 948);
    ck_assert_int_eq (found.items[1]->height, 768);

    /* Unknown size, default server address */
    ck_assert_str_eq (found.items[2]->image_url,
            "https://www.artic.edu/iiif/2/cc/full/3000,/0/default.jpg");
    ck_assert_int_eq (found.items[2]->width, 0);

    found_free (&found);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_met)
{
    Found  found;
    char  *json = nullptr;
    char  *url  = nullptr;

    /* The search response holds ids only */
    ck_assert_int_eq (parse_response (&met_provider,
                                      "met_search.json", &found), 314);
    ck_assert_int_eq (found.count, 3);
    ck_assert_str_eq (found.items[0]->id, "486649");
    ck_assert_str_eq (found.items[1]->id, "10381");
    ck_assert_ptr_null (found.items[1]->image_url);

    url = met_provider.detail_url (found.items[1]->id);
    ck_assert_str_eq (url, "https://collectionapi.metmuseum.org/public/"
                           "collection/v1/objects/10381");
    free (url);

    json = read_response ("met_object.json");
    ck_assert (met_provider.parse_detail (json, found.items[1]));
    free (json);
    ck_assert_str_eq (found.items[1]->title, "Mountain Lake Scene");
    ck_assert_str_eq (found.items[1]->author_name, "John William Casilear");
    ck_assert_str_eq (found.items[1]->license_name, "CC0 1.0");
    ck_assert_str_eq (found.items[1]->file_name, "Met_10381.jpg");
    ck_assert_str_eq (found.items[1]->image_url,
            "https://images.metmuseum.org/CRDImages/ad/original/APS1620.jpg");
    ck_assert_str_eq (found.items[1]->thumb_url,
            "https://images.metmuseum.org/CRDImages/ad/web-large/APS1620.jpg");
    ck_assert_str_eq (found.items[1]->page_url,
            "https://www.metmuseum.org/art/collection/search/10381");
    ck_assert_str_eq (found.items[1]->display_markup, "Mountain Lake Scene");

    /* An object that is not in the public domain is rejected */
    json = read_response ("met_object_restricted.json");
    ck_assert (!met_provider.parse_detail (json, found.items[0]));
    free (json);
    ck_assert_ptr_null (found.items[0]->image_url);

    ck_assert (!met_provider.parse_detail ("{\"message\":\"Not Found\"}",
                                           found.items[2]));
    ck_assert (!met_provider.parse_detail ("not json", found.items[2]));

    found_free (&found);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_met_no_results)
{
    Found found     = { .count = 0 };
    int   found_cnt = -1;

    ck_assert (met_provider.parse ("{\"total\":0,\"objectIDs\":null}",
                                   &found_cnt, found_add, &found));
    ck_assert_int_eq (found_cnt, 0);
    ck_assert_int_eq (found.count, 0);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_nasa)
{
    Found       found;
    SearchItem *item = nullptr;

    ck_assert_int_eq (parse_response (&nasa_provider,
                                      "nasa_search.json", &found), 221);
    /* The item without links is left out */
    ck_assert_int_eq (found.count, 4);

    /* Original image without size data */
    item = found.items[0];
    ck_assert_str_eq (item->id, "s36-81-054");
    ck_assert_str_eq (item->title, "Great Salt Lake, Utah, USA");
    ck_assert_str_eq (item->author_name, "JSC");
    ck_assert_str_eq (item->image_url,
            "https://images-assets.nasa.gov/image/s36-81-054/"
            "s36-81-054~orig.jpg");
    ck_assert_str_eq (item->thumb_url,
            "https://images-assets.nasa.gov/image/s36-81-054/"
            "s36-81-054~thumb.jpg");
    ck_assert_str_eq (item->page_url,
                      "https://images.nasa.gov/details/s36-81-054");
    ck_assert_str_eq (item->file_name, "NASA_s36-81-054.jpg");
    ck_assert_int_eq (item->width, 0);

    item = found.items[1];
    ck_assert_int_eq (item->width, 8256);
    ck_assert_int_eq (item->height, 5504);

    /* TIFF original: the largest jpeg copy is taken, spaces are encoded */
    item = found.items[3];
    ck_assert_str_eq (item->id, "SYN_0001");
    ck_assert_str_eq (item->image_url,
            "https://example.org/image/SYN%200001/SYN%200001~large.jpg");
    ck_assert_str_eq (item->thumb_url,
            "https://example.org/image/SYN%200001/SYN%200001~thumb.jpg");
    ck_assert_str_eq (item->page_url,
                      "https://images.nasa.gov/details/SYN%200001");
    ck_assert_int_eq (item->width, 1920);
    ck_assert_str_eq (item->author_name, "GSFC");

    found_free (&found);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_smithsonian)
{
    Found       found;
    SearchItem *item = nullptr;

    ck_assert_int_eq (parse_response (&smithsonian_provider,
                                      "smithsonian_search.json", &found), 41);
    /* The image with usage conditions and the object without media are
     * left out */
    ck_assert_int_eq (found.count, 2);

    item = found.items[0];
    ck_assert_str_eq (item->id, "SAAM-1986_78_1");
    ck_assert_str_eq (item->title, "Whiteface Mountain from Lake Placid");
    ck_assert_str_eq (item->author_name,
            "Sanford Robinson Gifford, born Greenfield, NY 1823-died "
            "New York City 1880");
    ck_assert_str_eq (item->license_name, "CC0 1.0");
    ck_assert_str_eq (item->file_name, "Smithsonian_SAAM-1986_78_1.jpg");
    ck_assert_str_eq (item->image_url,
            "https://ids.si.edu/ids/download?id=SAAM-1986.78_1.jpg");
    ck_assert_str_eq (item->thumb_url,
            "https://ids.si.edu/ids/deliveryService?id=SAAM-1986.78_1"
            "&max=300");
    ck_assert_str_eq (item->page_url,
            "https://americanart.si.edu/collections/search/artwork/?id=9005");
    /* Size of the downloaded file, not of the archived original */
    ck_assert_int_eq (item->width, 3000);
    ck_assert_int_eq (item->height, 1747);

    found_free (&found);
}
END_TEST
/*----------------------------------------------------------------------------*/
static Suite *
web_parse_suite (void)
{
    Suite *s        = suite_create ("WebParse");
    TCase *tc_util  = tcase_create ("Util");
    TCase *tc_item  = tcase_create ("SearchItem");
    TCase *tc_urls  = tcase_create ("Urls");
    TCase *tc_parse = tcase_create ("Parse");

    tcase_add_test (tc_util, test_strip_html);
    tcase_add_test (tc_util, test_ext_from_url);
    tcase_add_test (tc_util, test_ext_from_mime);
    tcase_add_test (tc_util, test_opts);

    tcase_add_test (tc_item, test_item_id_is_file_name_safe);
    tcase_add_test (tc_item, test_item_names);
    tcase_add_test (tc_item, test_item_names_without_id);

    tcase_add_test (tc_urls, test_url_query_is_escaped);
    tcase_add_test (tc_urls, test_url_paging);
    tcase_add_test (tc_urls, test_url_options);

    tcase_add_test (tc_parse, test_parse_rejects_bad_response);
    tcase_add_test (tc_parse, test_parse_commons);
    tcase_add_test (tc_parse, test_parse_commons_no_results);
    tcase_add_test (tc_parse, test_parse_openverse);
    tcase_add_test (tc_parse, test_parse_artic);
    tcase_add_test (tc_parse, test_parse_artic_image_size);
    tcase_add_test (tc_parse, test_parse_met);
    tcase_add_test (tc_parse, test_parse_met_no_results);
    tcase_add_test (tc_parse, test_parse_nasa);
    tcase_add_test (tc_parse, test_parse_smithsonian);

    suite_add_tcase (s, tc_util);
    suite_add_tcase (s, tc_item);
    suite_add_tcase (s, tc_urls);
    suite_add_tcase (s, tc_parse);

    return s;
}
/*----------------------------------------------------------------------------*/
int
main (void)
{
    int      failed;
    Suite   *s  = web_parse_suite ();
    SRunner *sr = srunner_create (s);

    srunner_run_all (sr, CK_NORMAL);
    failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
/*----------------------------------------------------------------------------*/
