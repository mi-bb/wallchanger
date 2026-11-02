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
#include "string_functions.h"
#include "file_io.h"
#include "file_dir_functions.h"
#include "errors.h"
#include "cfg_file.h"
#include "json_file.h"
#include "hash_djb2.h"
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
    json_object *j_res;  /* Result Json obj/array to return */
    json_object *j_val;  /* Json value to add to result */

    j_res = json_object_new_object();

    if (!str_is_empty (item->id)) {
        j_val = json_object_new_string (item->id);
        json_object_object_add_ex (j_res, JS_ID, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->author_name)) {
        j_val = json_object_new_string (item->author_name);
        json_object_object_add_ex (j_res, JS_AUTH_NAME, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->author_url)) {
        j_val = json_object_new_string (item->author_url);
        json_object_object_add_ex (j_res, JS_AUTH_URL, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->file_name)) {
        j_val = json_object_new_string (item->file_name);
        json_object_object_add_ex (j_res, JS_FILE_NAME, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->display_name)) {
        j_val = json_object_new_string (item->display_name);
        json_object_object_add_ex (j_res, JS_DISP_NAME, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->display_markup)) {
        j_val = json_object_new_string (item->display_markup);
        json_object_object_add_ex (j_res, JS_DISP_MKUP, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->page_url)) {
        j_val = json_object_new_string (item->page_url);
        json_object_object_add_ex (j_res, JS_PAGE_URL, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->image_url)) {
        j_val = json_object_new_string (item->image_url);
        json_object_object_add_ex (j_res, JS_IMAGE_URL, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->thumb_url)) {
        j_val = json_object_new_string (item->thumb_url);
        json_object_object_add_ex (j_res, JS_THUMB_URL, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (item->service_name)) {
        j_val = json_object_new_string (item->service_name);
        json_object_object_add_ex (j_res, JS_SERV_NAME, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (item->width > 0) {
        j_val = json_object_new_int (item->width);
        json_object_object_add_ex (j_res, JS_WIDTH, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (item->height > 0) {
        j_val = json_object_new_int (item->height);
        json_object_object_add_ex (j_res, JS_HEIGHT, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    return j_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert json_object to SearchItem.
 *
 * @param[in] j_obj  json_object item to convert
 * @return    New SearchItem with search data.
 */
static SearchItem *
cache_query_json_to_search_item (json_object *j_obj)
{
    SearchItem  *item; /* Item to return */
    json_object *j_val;   /* Reading/checking val */

    item = search_item_new ();

    if (json_object_object_get_ex (j_obj, JS_ID, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        search_item_set_id_string (item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_AUTH_NAME, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        search_item_set_author_name (item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_AUTH_URL, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        search_item_set_author_url (item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_FILE_NAME, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        search_item_set_file_name (item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_DISP_NAME, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        search_item_set_display_name (item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_DISP_MKUP, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        search_item_set_display_markup (item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_PAGE_URL, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        search_item_set_page_url (item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_IMAGE_URL, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        search_item_set_image_url (item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_THUMB_URL, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        search_item_set_thumb_url (item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_SERV_NAME, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        search_item_set_service_name (item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_WIDTH, &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        search_item_set_width (item, json_object_get_int (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_HEIGHT, &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        search_item_set_height (item, json_object_get_int (j_val));
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
 * @param[in] cq_query  CacheQuery item
 * @return    New json_object with converted data
 */
static json_object *
cache_query_search_items_to_json_array (const CacheQuery *cq_query)
{
    json_object *j_arr;
    json_object *j_val;
    int i = 0;

    j_arr = json_object_new_array ();

    for (i = 0; i < cq_query->count; ++i) {
        j_val = cache_query_search_item_to_json (cq_query->items[i]);
        json_object_array_add (j_arr, j_val);
    }
    return j_arr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free CacheQuery item.
 */
void
cache_query_free (CacheQuery *cq_query)
{
    for (int i = 0; i < cq_query->count; ++i) {
        search_item_free (cq_query->items[i]);
    }
    free (cq_query->items);
    free (cq_query->file);
    free (cq_query->date);
    free (cq_query->query);
    free (cq_query->search_opts);
    free (cq_query);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Init CacheQuery item.
 *
 * @param[out] cq_query  Item to init
 * @return     none
 */
static void
cache_query_init (CacheQuery *cq_query)
{
    cq_query->items       = nullptr;
    cq_query->file        = nullptr;
    cq_query->date        = nullptr;
    cq_query->query       = nullptr;
    cq_query->search_opts = nullptr;
    cq_query->page        = 0;
    cq_query->found_cnt   = 0;
    cq_query->count       = 0;
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
    CacheQuery *cq_query = nullptr; /* CacheQuery to return */
    struct tm  *tm_time;         /* For current date string */
    time_t      t_time;          /* For current date string */
    char        date[64];       /* Current date string */

    if ((cq_query = malloc (sizeof (CacheQuery))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    cache_query_init (cq_query);

    t_time  = time (nullptr);
    tm_time = localtime (&t_time);

    strftime (date, 64, "%Y%m%d", tm_time);

#ifdef DEBUG
    printf ("%s %s %s\n", date, query, search_opts);
#endif

    cq_query->date        = strdup (date);
    cq_query->query       = strdup (query);
    cq_query->search_opts = strdup (search_opts);
    cq_query->page        = page;
    cq_query->file        = cfg_file_get_query_path ();
    str_append (&cq_query->file, "/");
    str_append (&cq_query->file, service_name);
    str_append (&cq_query->file, ".json");

    return cq_query;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append SearchItem item to list.
 */
void
cache_query_append_item (CacheQuery *cq_query,
                         SearchItem *item)
{
    SearchItem **tmp        = nullptr; /* Temp pointer for malloc / realloc */
    size_t       alloc_size = 0;       /* New alloc size */

    alloc_size = (size_t) (cq_query->count + 1) * sizeof (SearchItem*);

    tmp = cq_query->count == 0 ?
             malloc (alloc_size) :
             realloc (cq_query->items, alloc_size);

    if (tmp == nullptr)
        err (EXIT_FAILURE, nullptr);

    cq_query->items = tmp;
    cq_query->items[cq_query->count] = item;
    ++cq_query->count;
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
    json_object *j_obj;            /* Json object made from file data */
    json_object *j_date;           /* Date of search */
    json_object *j_query;          /* Query text */
    json_object *j_sopts;          /* Search options */
    json_object *j_fnd_cnt;        /* For number of found images */
    json_object *j_items;          /* Array with image info data */
    json_object *j_val;            /* For array tith image info values */
    CacheQuery  *cq_query = nullptr;  /* Cached query data to return */
    SearchItem  *item     = nullptr;  /* For image info data */
    size_t       count    = 0;     /* Number of elements in array */
    size_t       i        = 0;     /* i */
    int          goon     = 1;     /* Go on */
    char         page_str[10];     /* String with page number */

    *err   = ERR_OK;
    cq_query = cache_query_new (service_name, query, search_opts, page);
    sprintf (page_str, "%d", cq_query->page);

    if ((j_obj = json_open_file (cq_query->file, nullptr, err)) == nullptr) {
        cache_query_free (cq_query);
        return nullptr;
    }

    if (goon) {
        goon = (json_object_object_get_ex (j_obj, cq_query->date, &j_date)
            && json_object_get_type (j_date) == json_type_object);
    }
    if (goon) {
        goon = (json_object_object_get_ex (j_date, query, &j_query)
                  && json_object_get_type (j_query) == json_type_object);
    }
    if (goon) {
        goon = (json_object_object_get_ex (j_query, search_opts, &j_sopts)
                  && json_object_get_type (j_sopts) == json_type_object);
    }
    if (goon) {
        goon = 0;
        if (json_object_object_get_ex (j_sopts, JS_FOUND_CNT, &j_fnd_cnt)
            && json_object_get_type (j_fnd_cnt) == json_type_int) {

            cq_query->found_cnt = json_object_get_int (j_fnd_cnt);
            goon = 1;
        }
    }
    if (goon) {
        goon = 0;
        if (json_object_object_get_ex (j_sopts, page_str, &j_items) &&
            json_object_get_type (j_items) == json_type_array) {

            count = json_object_array_length (j_items);

            for (i = 0; i < count; ++i) {
                j_val = json_object_array_get_idx (j_items, i);
                if (j_val != nullptr) {
                    item = cache_query_json_to_search_item (j_val);
                    cache_query_append_item (cq_query, item);
                }
            }
            goon = 1;
        }
    }
    json_object_put (j_obj);
    if (!goon) {
        cache_query_free (cq_query);
        cq_query = nullptr;
    }
    *err = ERR_OK;
    return cq_query;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save cached search data to a service cache file.
 */
int
cache_query_save (CacheQuery *cq_query)
{
    json_object   *j_obj;             /* Json object made from file data */
    json_object   *j_date;            /* For query date */
    json_object   *j_query;           /* For query string */
    json_object   *j_per_page;        /* For per page */
    json_object   *j_val;             /* For getting values */
    json_object   *j_arr;             /* For array with items */
    const char    *json_buff = nullptr;  /* Json object as string */
    uint_fast32_t  file_hash = 0;        /* Json data file hash */
    int            err       = ERR_OK;  /* For error output */
    char           page_str[10];        /* Page number as string */

    sprintf (page_str, "%d", cq_query->page);

    if ((j_obj = json_open_file (cq_query->file, &file_hash, &err)) == nullptr)
        return err;

    if (json_object_object_get_ex (j_obj, cq_query->date, &j_val) &&
        json_object_get_type (j_val) == json_type_object) {

        j_date = j_val;
    }
    else {
        j_date = json_object_new_object ();
        json_object_object_add (j_obj, cq_query->date, j_date);
    }

    if (json_object_object_get_ex (j_date, cq_query->query, &j_val) &&
        json_object_get_type (j_val) == json_type_object) {

        j_query = j_val;
    }
    else {
        j_query = json_object_new_object ();
        json_object_object_add (j_date, cq_query->query, j_query);
    }
    if (json_object_object_get_ex (j_query, cq_query->search_opts, &j_val) &&
        json_object_get_type (j_val) == json_type_object) {

        j_per_page = j_val;
    }
    else {
        j_per_page = json_object_new_object ();
        json_object_object_add (j_query, cq_query->search_opts, j_per_page);
    }

    j_val = json_object_new_int (cq_query->found_cnt);
    json_object_object_add (j_per_page, JS_FOUND_CNT, j_val);

    j_arr = cache_query_search_items_to_json_array (cq_query);
    json_object_object_add (j_per_page, page_str, j_arr);
    json_buff = json_object_to_json_string (j_obj);

    if (hash (json_buff) != file_hash)
        err = save_file_data (cq_query->file, json_buff);

    json_object_put (j_obj);

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
    json_object  *j_obj;            /* Json item with query info */
    GDate        *gd_date;          /* GDate for date count */
    time_t        t_time;           /* time_t for date count */
    char          date_str[64];     /* Temp date string */
    char        **dates    = nullptr;  /* List of dates not to delete */
    char         *file     = nullptr;  /* Name of file with query info */
    const char   *json_buff = nullptr;  /* Json file content buffer */
    int           i        = 0;      /* i */
    int           keep     = 0;      /* Item not to delete */
    int           err      = ERR_OK; /* For error output */
    uint_fast32_t file_hash = 0;      /* Json file content hash */

    /* Get current date */
    t_time  = time (nullptr);
    gd_date = g_date_new ();

    g_date_set_time_t (gd_date, t_time);

    /* Create service config file name */
    file = cfg_file_get_query_path ();
    str_append (&file, "/");
    str_append (&file, service_name);
    str_append (&file, ".json");
    /* Open service json config file */
    if ((j_obj = json_open_file (file, &file_hash, &err)) == nullptr) {
        free (file);
        return err;
    }
    /* Alloc pointers for strings with dates */
    dates = malloc ((size_t) days * sizeof (char*));
    /* Store dates subtracting one day from a stored one */
    for (i = 0; i < days; ++i) {
        g_date_strftime (date_str, 64, "%Y%m%d", gd_date);
        dates[i] = strdup (date_str);
        g_date_subtract_days (gd_date, 1);
    }
    /* Iterate json config file for given service, compare keys with
     * dates from list and delete key if date in config is not on list */
    json_object_object_foreach (j_obj, key, val1) {
        keep = 0;
        for (i = 0; i < days; ++i)
            keep |= (strcmp (key, dates[i]) == 0);
        if (!keep)
            json_object_object_del (j_obj, key);
    }
    json_buff = json_object_to_json_string (j_obj);
    /* Compare hashes and save data if something changed */
    if (hash (json_buff) != file_hash) {
        err = save_file_data (file, json_buff);
    }
    for (i = 0; i < days; ++i) {
        free (dates[i]);
    }
    free (file);
    free (dates);
    json_object_put (j_obj);
    g_date_free (gd_date);
    return err;
}
/*----------------------------------------------------------------------------*/

