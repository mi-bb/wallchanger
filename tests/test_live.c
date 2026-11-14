/**
 * @file  test_live.c
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
 * @brief  Unit tests for the remote live wallpaper services (live/
 *         directory).
 *
 * No network requests are made: responses saved in the tests data directory
 * are parsed. They are trimmed captures of real ones, extended with entries
 * marked "Synthetic" for the cases the captured data does not have.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "live/live_source.h"
#include "live/live_bing.h"
#include "live/live_commons.h"
#include "live/live_wallhaven.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read a response saved in the tests data directory.
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
static bool
parse_response (const LiveSource *source,
                const char       *name,
                const LiveParams *params,
                LiveImage        *image)
{
    char *json = read_response (name);
    bool  res  = source->parse (json, params, image);

    free (json);
    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check that a string ends with a suffix.
 */
static bool
ends_with (const char *str,
           const char *suffix)
{
    size_t len     = strlen (str);
    size_t suf_len = strlen (suffix);

    return len >= suf_len && strcmp (str + len - suf_len, suffix) == 0;
}
/*----------------------------------------------------------------------------*/
static const LiveParams full_hd  = {1920, 1080, nullptr, "2026-11-14"};
static const LiveParams qhd      = {2560, 1440, nullptr, "2026-11-14"};
static const LiveParams portrait = {1080, 1920, nullptr, "2026-11-14"};
/*----------------------------------------------------------------------------*/
START_TEST (test_url_bing)
{
    char *url = live_bing.request_url (&full_hd);

    ck_assert_str_eq (url, "https://www.bing.com/HPImageArchive.aspx"
                           "?format=js&idx=0&n=8");
    free (url);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_url_wallhaven)
{
    LiveParams params = qhd;
    char      *url    = live_wallhaven.request_url (&params);

    ck_assert_ptr_nonnull (strstr (url, "https://wallhaven.cc/api/v1/search?"));
    ck_assert_ptr_nonnull (strstr (url, "sorting=random"));
    ck_assert_ptr_nonnull (strstr (url, "purity=100"));
    ck_assert_ptr_nonnull (strstr (url, "atleast=2560x1440"));
    ck_assert_ptr_nonnull (strstr (url, "ratios=landscape"));
    ck_assert_ptr_null (strstr (url, "q="));
    ck_assert_ptr_null (strstr (url, "apikey"));
    free (url);

    /* Keywords are percent-encoded */
    params.keywords = "space nebula&x";
    url = live_wallhaven.request_url (&params);
    ck_assert_ptr_nonnull (strstr (url, "&q=space%20nebula%26x"));
    free (url);

    url = live_wallhaven.request_url (&portrait);
    ck_assert_ptr_nonnull (strstr (url, "atleast=1080x1920"));
    ck_assert_ptr_nonnull (strstr (url, "ratios=portrait"));
    free (url);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_url_commons)
{
    char *url = live_commons.request_url (&qhd);

    ck_assert_ptr_nonnull (strstr (url, "titles=Template:Potd/2026-11-14&"));
    ck_assert_ptr_nonnull (strstr (url, "generator=images"));
    ck_assert (ends_with (url, "&iiurlwidth=2560"));
    free (url);

    /* The larger screen dimension is requested */
    url = live_commons.request_url (&portrait);
    ck_assert (ends_with (url, "&iiurlwidth=1920"));
    free (url);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_rejects_bad_response)
{
    const LiveSource *sources[] = {&live_bing, &live_commons, &live_wallhaven};
    const char       *bad[]     = {"", "not json", "[]", "{}",
                                   "{\"images\":[]}", "{\"data\":[]}",
                                   "{\"query\":{\"pages\":[]}}"};
    LiveImage         image     = {};

    for (size_t i = 0; i < sizeof (sources) / sizeof (sources[0]); ++i) {
        for (size_t j = 0; j < sizeof (bad) / sizeof (bad[0]); ++j) {
            ck_assert_msg (!sources[i]->parse (bad[j], &full_hd, &image),
                           "%s accepted '%s'", sources[i]->name, bad[j]);
            live_image_clear (&image);
        }
    }
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_bing_size)
{
    ck_assert_str_eq (live_bing_size (1366, 768),  "1366x768");
    ck_assert_str_eq (live_bing_size (1280, 720),  "1366x768");
    ck_assert_str_eq (live_bing_size (1920, 1080), "1920x1080");
    ck_assert_str_eq (live_bing_size (1600, 900),  "1920x1080");
    ck_assert_str_eq (live_bing_size (2560, 1440), "UHD");
    ck_assert_str_eq (live_bing_size (3840, 2160), "UHD");
    ck_assert_str_eq (live_bing_size (1080, 1920), "UHD");
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_bing)
{
    LiveImage image = {};

    /* The first, synthetic image is not marked as a wallpaper */
    ck_assert (parse_response (&live_bing, "live_bing.json", &full_hd,
                               &image));
    ck_assert_str_eq (image.image_url, "https://www.bing.com/th?id="
                      "OHR.BlueHK_ROW5600938316_1920x1080.jpg");
    ck_assert_str_eq (image.file_name,
                      "bing_OHR.BlueHK_ROW5600938316_1920x1080.jpg");
    ck_assert_str_eq (image.title, "Victoria Harbour, Hong Kong, China");
    ck_assert_str_eq (image.author, "Juan Romero/Cavan Images");
    ck_assert_ptr_nonnull (image.license);
    ck_assert_ptr_nonnull (strstr (image.page_url, "https://www.bing.com/"));
    live_image_clear (&image);

    ck_assert (parse_response (&live_bing, "live_bing.json", &qhd, &image));
    ck_assert (ends_with (image.image_url, "_UHD.jpg"));
    ck_assert (ends_with (image.file_name, "_UHD.jpg"));
    live_image_clear (&image);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_wallhaven)
{
    LiveImage  image  = {};
    LiveParams params = {2560, 1600, nullptr, "2026-11-14"};

    /* First image with the screen's aspect ratio */
    ck_assert (parse_response (&live_wallhaven, "live_wallhaven.json",
                               &full_hd, &image));
    ck_assert_str_eq (image.image_url,
                      "https://w.wallhaven.cc/full/0w/wallhaven-0wz88r.jpg");
    ck_assert_str_eq (image.file_name, "wallhaven_0wz88r.jpg");
    ck_assert_str_eq (image.page_url, "https://wallhaven.cc/w/0wz88r");
    live_image_clear (&image);

    /* 16:10 screen */
    ck_assert (parse_response (&live_wallhaven, "live_wallhaven.json",
                               &params, &image));
    ck_assert_str_eq (image.file_name, "wallhaven_4gom7l.jpg");
    live_image_clear (&image);

    /* Portrait screen, synthetic png image keeps its extension */
    ck_assert (parse_response (&live_wallhaven, "live_wallhaven.json",
                               &portrait, &image));
    ck_assert_str_eq (image.file_name, "wallhaven_synth02.jpg");
    live_image_clear (&image);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_parse_commons)
{
    LiveImage image = {};

    /* The first, synthetic page is a video */
    ck_assert (parse_response (&live_commons, "live_commons.json", &full_hd,
                               &image));
    ck_assert_ptr_nonnull (strstr (image.image_url,
                "https://thumb.wikimedia.org/wikipedia/commons/thumb/b/bd/"
                "Eastern_quoll_"));
    ck_assert_ptr_nonnull (strstr (image.image_url, "/1920px-"));
    ck_assert_str_eq (image.file_name, "commons_2026-11-14_1920.jpg");
    ck_assert_str_eq (image.title,
            "Eastern quoll (Dasyurus viverrinus) fawn morph Esk Valley");
    ck_assert_str_eq (image.author, "Charles J. Sharp");
    ck_assert_str_eq (image.license, "CC BY-SA 4.0");
    ck_assert_str_eq (image.license_url,
                      "https://creativecommons.org/licenses/by-sa/4.0");
    ck_assert_ptr_nonnull (strstr (image.page_url,
                "https://commons.wikimedia.org/wiki/File:Eastern_quoll"));
    live_image_clear (&image);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_file_name)
{
    char *name = live_file_name ("bing", "a/b c?../x", ".jpg");

    ck_assert_str_eq (name, "bing_a_b_c_.._x.jpg");
    free (name);

    name = live_file_name ("wallhaven", "abc", nullptr);
    ck_assert_str_eq (name, "wallhaven_abc");
    free (name);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_pick_in_order)
{
    ck_assert_uint_eq (live_pick_first (3, -1, false, nullptr), 0);
    ck_assert_uint_eq (live_pick_first (3,  0, false, nullptr), 1);
    ck_assert_uint_eq (live_pick_first (3,  1, false, nullptr), 2);
    ck_assert_uint_eq (live_pick_first (3,  2, false, nullptr), 0);
    /* Last used service no longer enabled */
    ck_assert_uint_eq (live_pick_first (3,  5, false, nullptr), 0);
    ck_assert_uint_eq (live_pick_first (1,  0, false, nullptr), 0);
    ck_assert_uint_eq (live_pick_first (0, -1, false, nullptr), 0);
}
END_TEST
/*----------------------------------------------------------------------------*/
START_TEST (test_pick_random)
{
    RandMem *rand_mem = randomm_new_with_range (3);
    int      seen[3]  = {0};

    /* Every service is used once before any is used again */
    for (int round = 0; round < 4; ++round) {
        for (int i = 0; i < 3; ++i) {
            size_t pos = live_pick_first (3, 0, true, rand_mem);
            ck_assert_uint_lt (pos, 3);
            ++seen[pos];
        }
        for (int i = 0; i < 3; ++i)
            ck_assert_int_eq (seen[i], round + 1);
    }
    randomm_free (rand_mem);
}
END_TEST
/*----------------------------------------------------------------------------*/
static Suite *
live_suite (void)
{
    Suite *s        = suite_create ("Live");
    TCase *tc_urls  = tcase_create ("Urls");
    TCase *tc_parse = tcase_create ("Parse");
    TCase *tc_pick  = tcase_create ("Pick");

    tcase_add_test (tc_urls, test_url_bing);
    tcase_add_test (tc_urls, test_url_wallhaven);
    tcase_add_test (tc_urls, test_url_commons);

    tcase_add_test (tc_parse, test_parse_rejects_bad_response);
    tcase_add_test (tc_parse, test_bing_size);
    tcase_add_test (tc_parse, test_parse_bing);
    tcase_add_test (tc_parse, test_parse_wallhaven);
    tcase_add_test (tc_parse, test_parse_commons);
    tcase_add_test (tc_parse, test_file_name);

    tcase_add_test (tc_pick, test_pick_in_order);
    tcase_add_test (tc_pick, test_pick_random);

    suite_add_tcase (s, tc_urls);
    suite_add_tcase (s, tc_parse);
    suite_add_tcase (s, tc_pick);

    return s;
}
/*----------------------------------------------------------------------------*/
int
main (void)
{
    int      failed = 0;
    SRunner *sr     = srunner_create (live_suite ());

    srunner_run_all (sr, CK_NORMAL);
    failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
/*----------------------------------------------------------------------------*/
