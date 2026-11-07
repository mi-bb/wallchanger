/**
 * @file  cache_query.c
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
 * @brief  Search query cache
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include "config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif
#include <time.h>
#include <err.h>
#include "util/string_functions.h"
#include "util/file_io.h"
#include "util/file_dir_functions.h"
#include "util/errors.h"
#include "settings/cfg_file.h"
#include "settings/json_file.h"
#include "util/hash_djb2.h"
#include "cache_query.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   JS_FOUND_CNT
 * @brief Name of number of found items field
 * @def   JS_ID
 * @brief Name of id field
 * @def   JS_AUTH_NAME
 * @brief Name of author field
 * @def   JS_AUTH_URL
 * @brief Name of author url field
 * @def   JS_FILE_NAME
 * @brief Name of file name field
 * @def   JS_DISP_NAME
 * @brief Name of display name field
 * @def   JS_DISP_MKUP
 * @brief Name of display markup field
 * @def   JS_PAGE_URL
 * @brief Name of image page url field
 * @def   JS_IMAGE_URL
 * @brief Name of image url field
 * @def   JS_THUMB_URL
 * @brief Name of image thumbnail url field
 * @def   JS_SERV_NAME
 * @brief Name of service name field
 * @def   JS_WIDTH
 * @brief Name of image width field
 * @def   JS_HEIGHT
 * @brief Name of image height field
 */
#define JS_FOUND_CNT "found_cnt"
#define JS_ID        "id"
#define JS_AUTH_NAME "auth_name"
#define JS_AUTH_URL  "auth_url"
#define JS_FILE_NAME "file_name"
#define JS_DISP_NAME "disp_name"
#define JS_DISP_MKUP "disp_mkup"
#define JS_PAGE_URL  "page_url"
#define JS_IMAGE_URL "image_url"
#define JS_THUMB_URL "thumb_url"
#define JS_SERV_NAME "serv_name"
#define JS_WIDTH     "width"
#define JS_HEIGHT    "height"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert SearchItem item to json_object.
 *
 * @param[in] item  SearchItem item to convert
 * @return    New json_object with search data.
 */
static json_object *
cache_query_search_item_to_json (const SearchItem *item)
{
    json_object *result; /* Result Json obj/array to return */
    json_object *value;  /* Json value to add to result */

    result = json_object_new_object();

    if (!str_is_empty (item->id)) {
        value = json_object_new_string (item->id);
        json_object_object_add_ex (result, JS_ID, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->author_name)) {
        value = json_object_new_string (item->author_name);
        json_object_object_add_ex (result, JS_AUTH_NAME, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->author_url)) {
        value = json_object_new_string (item->author_url);
        json_object_object_add_ex (result, JS_AUTH_URL, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->file_name)) {
        value = json_object_new_string (item->file_name);
        json_object_object_add_ex (result, JS_FILE_NAME, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->display_name)) {
        value = json_object_new_string (item->display_name);
        json_object_object_add_ex (result, JS_DISP_NAME, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->display_markup)) {
        value = json_object_new_string (item->display_markup);
        json_object_object_add_ex (result, JS_DISP_MKUP, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->page_url)) {
        value = json_object_new_string (item->page_url);
        json_object_object_add_ex (result, JS_PAGE_URL, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->image_url)) {
        value = json_object_new_string (item->image_url);
        json_object_object_add_ex (result, JS_IMAGE_URL, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->thumb_url)) {
        value = json_object_new_string (item->thumb_url);
        json_object_object_add_ex (result, JS_THUMB_URL, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->service_name)) {
        value = json_object_new_string (item->service_name);
        json_object_object_add_ex (result, JS_SERV_NAME, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (item->width > 0) {
        value = json_object_new_int (item->width);
        json_object_object_add_ex (result, JS_WIDTH, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (item->height > 0) {
        value = json_object_new_int (item->height);
        json_object_object_add_ex (result, JS_HEIGHT, value,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert json_object to SearchItem.
 *
 * @param[in] root  json_object item to convert
 * @return    New SearchItem with search data.
 */
static SearchItem *
cache_query_json_to_search_item (json_object *root)
{
    SearchItem  *item; /* Item to return */
    json_object *value;   /* Reading/checking val */

    item = search_item_new ();

    if (json_object_object_get_ex (root, JS_ID, &value) &&
        json_object_get_type (value) == json_type_string) {

        search_item_set_id_string (item, json_object_get_string (value));
    }
    if (json_object_object_get_ex (root, JS_AUTH_NAME, &value) &&
        json_object_get_type (value) == json_type_string) {

        search_item_set_author_name (item, json_object_get_string (value));
    }
    if (json_object_object_get_ex (root, JS_AUTH_URL, &value) &&
        json_object_get_type (value) == json_type_string) {

        search_item_set_author_url (item, json_object_get_string (value));
    }
    if (json_object_object_get_ex (root, JS_FILE_NAME, &value) &&
        json_object_get_type (value) == json_type_string) {

        search_item_set_file_name (item, json_object_get_string (value));
    }
    if (json_object_object_get_ex (root, JS_DISP_NAME, &value) &&
        json_object_get_type (value) == json_type_string) {

        search_item_set_display_name (item, json_object_get_string (value));
    }
    if (json_object_object_get_ex (root, JS_DISP_MKUP, &value) &&
        json_object_get_type (value) == json_type_string) {

        search_item_set_display_markup (item, json_object_get_string (value));
    }
    if (json_object_object_get_ex (root, JS_PAGE_URL, &value) &&
        json_object_get_type (value) == json_type_string) {

        search_item_set_page_url (item, json_object_get_string (value));
    }
    if (json_object_object_get_ex (root, JS_IMAGE_URL, &value) &&
        json_object_get_type (value) == json_type_string) {

        search_item_set_image_url (item, json_object_get_string (value));
    }
    if (json_object_object_get_ex (root, JS_THUMB_URL, &value) &&
        json_object_get_type (value) == json_type_string) {

        search_item_set_thumb_url (item, json_object_get_string (value));
    }
    if (json_object_object_get_ex (root, JS_SERV_NAME, &value) &&
        json_object_get_type (value) == json_type_string) {

        search_item_set_service_name (item, json_object_get_string (value));
    }
    if (json_object_object_get_ex (root, JS_WIDTH, &value) &&
        json_object_get_type (value) == json_type_int) {

        search_item_set_width (item, json_object_get_int (value));
    }
    if (json_object_object_get_ex (root, JS_HEIGHT, &value) &&
        json_object_get_type (value) == json_type_int) {

        search_item_set_height (item, json_object_get_int (value));
    }
#ifdef DEBUG
    search_item_print (item);
#endif
    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert list of SearchItem elements to json array.
 *
 * @param[in] cache  CacheQuery item
 * @return    New json_object with converted data
 */
static json_object *
cache_query_search_items_to_json_array (const CacheQuery *cache)
{
    json_object *array;
    json_object *value;
    int i = 0;

    array = json_object_new_array ();

    for (i = 0; i < cache->count; ++i) {
        value = cache_query_search_item_to_json (cache->items[i]);
        json_object_array_add (array, value);
    }
    return array;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free CacheQuery item.
 */
void
cache_query_free (CacheQuery *cache)
{
    for (int i = 0; i < cache->count; ++i) {
        search_item_free (cache->items[i]);
    }
    free (cache->items);
    free (cache->file);
    free (cache->date);
    free (cache->query);
    free (cache->search_opts);
    free (cache);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Init CacheQuery item.
 *
 * @param[out] cache  Item to init
 * @return     none
 */
static void
cache_query_init (CacheQuery *cache)
{
    cache->items       = nullptr;
    cache->file        = nullptr;
    cache->date        = nullptr;
    cache->query       = nullptr;
    cache->search_opts = nullptr;
    cache->page        = 0;
    cache->found_cnt   = 0;
    cache->count       = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new CacheQuery item.
 */
CacheQuery *
cache_query_new (const char *service_name,
                 const char *query,
                 const char *search_opts,
                 const int   page)
{
    CacheQuery *cache = nullptr; /* CacheQuery to return */
    struct tm  *local_time;      /* For current date string */
    time_t      now;             /* For current date string */
    char        date[64];       /* Current date string */

    if ((cache = malloc (sizeof (CacheQuery))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    cache_query_init (cache);

    now        = time (nullptr);
    local_time = localtime (&now);

    strftime (date, 64, "%Y%m%d", local_time);

#ifdef DEBUG
    printf ("%s %s %s\n", date, query, search_opts);
#endif

    cache->date        = strdup (date);
    cache->query       = strdup (query);
    cache->search_opts = strdup (search_opts);
    cache->page        = page;
    cache->file        = cfg_file_get_query_path ();
    str_append (&cache->file, "/");
    str_append (&cache->file, service_name);
    str_append (&cache->file, ".json");

    return cache;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append SearchItem item to list.
 */
void
cache_query_append_item (CacheQuery *cache,
                         SearchItem *item)
{
    SearchItem **tmp        = nullptr; /* Temp pointer for malloc / realloc */
    size_t       alloc_size = 0;       /* New alloc size */

    alloc_size = (size_t) (cache->count + 1) * sizeof (SearchItem*);

    tmp = cache->count == 0 ?
             malloc (alloc_size) :
             realloc (cache->items, alloc_size);

    if (tmp == nullptr)
        err (EXIT_FAILURE, nullptr);

    cache->items = tmp;
    cache->items[cache->count] = item;
    ++cache->count;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if there is a cached data for query.
 */
CacheQuery *
cache_query_check_query (const char *service_name,
                         const char *query,
                         const char *search_opts,
                         const int   page,
                         int        *err)
{
    json_object *root;             /* Json object made from file data */
    json_object *date_obj;         /* Date of search */
    json_object *query_obj;        /* Query text */
    json_object *opts_obj;         /* Search options */
    json_object *found_obj;        /* For number of found images */
    json_object *items_obj;        /* Array with image info data */
    json_object *value;            /* For array tith image info values */
    CacheQuery  *cache = nullptr;     /* Cached query data to return */
    SearchItem  *item  = nullptr;     /* For image info data */
    size_t       count = 0;        /* Number of elements in array */
    size_t       i     = 0;        /* i */
    int          goon  = 1;        /* Go on */
    char         page_str[10];     /* String with page number */

    *err   = ERR_OK;
    cache = cache_query_new (service_name, query, search_opts, page);
    sprintf (page_str, "%d", cache->page);

    if ((root = json_open_file (cache->file, nullptr, err)) == nullptr) {
        cache_query_free (cache);
        return nullptr;
    }

    if (goon) {
        goon = (json_object_object_get_ex (root, cache->date, &date_obj)
            && json_object_get_type (date_obj) == json_type_object);
    }
    if (goon) {
        goon = (json_object_object_get_ex (date_obj, query, &query_obj)
                  && json_object_get_type (query_obj) == json_type_object);
    }
    if (goon) {
        goon = (json_object_object_get_ex (query_obj, search_opts, &opts_obj)
                  && json_object_get_type (opts_obj) == json_type_object);
    }
    if (goon) {
        goon = 0;
        if (json_object_object_get_ex (opts_obj, JS_FOUND_CNT, &found_obj)
            && json_object_get_type (found_obj) == json_type_int) {

            cache->found_cnt = json_object_get_int (found_obj);
            goon = 1;
        }
    }
    if (goon) {
        goon = 0;
        if (json_object_object_get_ex (opts_obj, page_str, &items_obj) &&
            json_object_get_type (items_obj) == json_type_array) {

            count = json_object_array_length (items_obj);

            for (i = 0; i < count; ++i) {
                value = json_object_array_get_idx (items_obj, i);
                if (value != nullptr) {
                    item = cache_query_json_to_search_item (value);
                    cache_query_append_item (cache, item);
                }
            }
            goon = 1;
        }
    }
    json_object_put (root);
    if (!goon) {
        cache_query_free (cache);
        cache = nullptr;
    }
    *err = ERR_OK;
    return cache;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save cached search data to a service cache file.
 */
int
cache_query_save (CacheQuery *cache)
{
    json_object   *root;              /* Json object made from file data */
    json_object   *date_obj;          /* For query date */
    json_object   *query_obj;         /* For query string */
    json_object   *page_obj;          /* For per page */
    json_object   *value;             /* For getting values */
    json_object   *array;             /* For array with items */
    const char    *json_buff = nullptr;  /* Json object as string */
    uint_fast32_t  file_hash = 0;        /* Json data file hash */
    int            err       = ERR_OK;  /* For error output */
    char           page_str[10];        /* Page number as string */

    sprintf (page_str, "%d", cache->page);

    if ((root = json_open_file (cache->file, &file_hash, &err)) == nullptr)
        return err;

    if (json_object_object_get_ex (root, cache->date, &value) &&
        json_object_get_type (value) == json_type_object) {

        date_obj = value;
    }
    else {
        date_obj = json_object_new_object ();
        json_object_object_add (root, cache->date, date_obj);
    }

    if (json_object_object_get_ex (date_obj, cache->query, &value) &&
        json_object_get_type (value) == json_type_object) {

        query_obj = value;
    }
    else {
        query_obj = json_object_new_object ();
        json_object_object_add (date_obj, cache->query, query_obj);
    }
    if (json_object_object_get_ex (query_obj, cache->search_opts, &value) &&
        json_object_get_type (value) == json_type_object) {

        page_obj = value;
    }
    else {
        page_obj = json_object_new_object ();
        json_object_object_add (query_obj, cache->search_opts, page_obj);
    }

    value = json_object_new_int (cache->found_cnt);
    json_object_object_add (page_obj, JS_FOUND_CNT, value);

    array = cache_query_search_items_to_json_array (cache);
    json_object_object_add (page_obj, page_str, array);
    json_buff = json_object_to_json_string (root);

    if (hash (json_buff) != file_hash)
        err = save_file_data (cache->file, json_buff);

    json_object_put (root);

    return err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check cache query file for entries older that days and delete
 *         them.
 */
int
cache_query_delete_older_than (const char *service_name,
                               const int   days)
{
    json_object  *root;             /* Json item with query info */
    GDate        *gdate;            /* GDate for date count */
    time_t        now;              /* time_t for date count */
    char          date_str[64];     /* Temp date string */
    char        **dates    = nullptr;  /* List of dates not to delete */
    char         *file     = nullptr;  /* Name of file with query info */
    const char   *json_buff = nullptr;  /* Json file content buffer */
    int           i        = 0;      /* i */
    int           keep     = 0;      /* Item not to delete */
    int           err      = ERR_OK; /* For error output */
    uint_fast32_t file_hash = 0;      /* Json file content hash */

    /* Get current date */
    now   = time (nullptr);
    gdate = g_date_new ();

    g_date_set_time_t (gdate, now);

    /* Create service config file name */
    file = cfg_file_get_query_path ();
    str_append (&file, "/");
    str_append (&file, service_name);
    str_append (&file, ".json");
    /* Open service json config file */
    if ((root = json_open_file (file, &file_hash, &err)) == nullptr) {
        free (file);
        return err;
    }
    /* Alloc pointers for strings with dates */
    dates = malloc ((size_t) days * sizeof (char*));
    /* Store dates subtracting one day from a stored one */
    for (i = 0; i < days; ++i) {
        g_date_strftime (date_str, 64, "%Y%m%d", gdate);
        dates[i] = strdup (date_str);
        g_date_subtract_days (gdate, 1);
    }
    /* Iterate json config file for given service, compare keys with
     * dates from list and delete key if date in config is not on list */
    json_object_object_foreach (root, key, val1) {
        keep = 0;
        for (i = 0; i < days; ++i)
            keep |= (strcmp (key, dates[i]) == 0);
        if (!keep)
            json_object_object_del (root, key);
    }
    json_buff = json_object_to_json_string (root);
    /* Compare hashes and save data if something changed */
    if (hash (json_buff) != file_hash) {
        err = save_file_data (file, json_buff);
    }
    for (i = 0; i < days; ++i) {
        free (dates[i]);
    }
    free (file);
    free (dates);
    json_object_put (root);
    g_date_free (gdate);
    return err;
}
/*----------------------------------------------------------------------------*/

