/**
 * @file  chquery.c
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
 * @brief  Search query cache
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdio.h>
#include "../config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif
#include <time.h>
#include <err.h>
#include "strfun.h"
#include "rwdt.h"
#include "fdfn.h"
#include "errs.h"
#include "cfgfile.h"
#include "jsof.h"
#include "hashfun.h"
#include "chquery.h"

#define JS_QUERY     "query"
#define JS_DATE      "date"
#define JS_PAGE      "page"
#define JS_PER_PAGE  "per_page"
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
 * @param[in] si_item  SearchItem item to convert
 * @return    New json_object with search data.
 */
static json_object *
chquery_searchitem_to_json (const SearchItem *si_item)
{
    json_object *j_res;  /* Result Json obj/array to return */
    json_object *j_val;  /* Json value to add to result */

    j_res = json_object_new_object();

    if (!str_is_empty (si_item->s_id)) {
        j_val = json_object_new_string (si_item->s_id);
        json_object_object_add_ex (j_res, JS_ID, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (si_item->s_author_name)) {
        j_val = json_object_new_string (si_item->s_author_name);
        json_object_object_add_ex (j_res, JS_AUTH_NAME, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (si_item->s_author_url)) {
        j_val = json_object_new_string (si_item->s_author_url);
        json_object_object_add_ex (j_res, JS_AUTH_URL, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (si_item->s_file_name)) {
        j_val = json_object_new_string (si_item->s_file_name);
        json_object_object_add_ex (j_res, JS_FILE_NAME, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (si_item->s_display_name)) {
        j_val = json_object_new_string (si_item->s_display_name);
        json_object_object_add_ex (j_res, JS_DISP_NAME, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (si_item->s_display_markup)) {
        j_val = json_object_new_string (si_item->s_display_markup);
        json_object_object_add_ex (j_res, JS_DISP_MKUP, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (si_item->s_page_url)) {
        j_val = json_object_new_string (si_item->s_page_url);
        json_object_object_add_ex (j_res, JS_PAGE_URL, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (si_item->s_image_url)) {
        j_val = json_object_new_string (si_item->s_image_url);
        json_object_object_add_ex (j_res, JS_IMAGE_URL, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (si_item->s_thumb_url)) {
        j_val = json_object_new_string (si_item->s_thumb_url);
        json_object_object_add_ex (j_res, JS_THUMB_URL, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (!str_is_empty (si_item->s_service_name)) {
        j_val = json_object_new_string (si_item->s_service_name);
        json_object_object_add_ex (j_res, JS_SERV_NAME, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (si_item->i_width > 0) {
        j_val = json_object_new_int (si_item->i_width);
        json_object_object_add_ex (j_res, JS_WIDTH, j_val,
                                   JSON_C_OBJECT_ADD_KEY_IS_NEW);
    }
    if (si_item->i_height > 0) {
        j_val = json_object_new_int (si_item->i_height);
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
chquery_json_to_searchitem (json_object *j_obj)
{
    SearchItem  *si_item; /* Item to return */
    json_object *j_val;   /* Reading/checking val */

    si_item = searchitem_new ();

    if (json_object_object_get_ex (j_obj, JS_ID, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_id_string (si_item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_AUTH_NAME, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_author_name (si_item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_AUTH_URL, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_author_url (si_item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_FILE_NAME, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_file_name (si_item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_DISP_NAME, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_display_name (si_item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_DISP_MKUP, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_display_markup (si_item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_PAGE_URL, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_page_url (si_item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_IMAGE_URL, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_image_url (si_item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_THUMB_URL, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_thumb_url (si_item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_SERV_NAME, &j_val) &&
        json_object_get_type (j_val) == json_type_string) {

        searchitem_set_service_name (si_item, json_object_get_string (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_WIDTH, &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_width (si_item, json_object_get_int (j_val));
    }
    if (json_object_object_get_ex (j_obj, JS_HEIGHT, &j_val) &&
        json_object_get_type (j_val) == json_type_int) {

        searchitem_set_height (si_item, json_object_get_int (j_val));
    }
#ifdef DEBUG
    searchitem_print (si_item);
#endif
    return si_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert list of SearchItem elements to json array.
 *
 * @param[in] cq_query  CacheQuery item
 * @return    New json_object with converted data
 */
static json_object *
cachequery_searchitems_to_json_array (const CacheQuery *cq_query)
{
    json_object *j_arr;
    json_object *j_val;
    int i = 0;

    j_arr = json_object_new_array ();

    for (i = 0; i < cq_query->i_per_page; ++i) {
        if (cq_query->si_items[i] != NULL) {
            j_val = chquery_searchitem_to_json (cq_query->si_items[i]);
            json_object_array_add (j_arr, j_val);
        }
    }
    return j_arr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free CacheQuery item.
 */
void
cachequery_free (CacheQuery *cq_query)
{
    for (int i = 0; i < cq_query->i_per_page; ++i) {
        if (cq_query->si_items[i] != NULL)
            searchitem_free (cq_query->si_items[i]);
    }
    free (cq_query->si_items);
    free (cq_query->s_file);
    free (cq_query->s_date);
    free (cq_query->s_query);
    free (cq_query);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new CacheQuery item.
 */
CacheQuery *
cachequery_new (const char *s_service_name,
                const char *s_query,
                const int   i_page,
                const int   i_per_page)
{
    CacheQuery *cq_query = NULL;
    struct tm  *tm_time;
    time_t      t_time;
    char        s_date[64];
    int i = 0;

    if ((cq_query = malloc (sizeof (CacheQuery))) == NULL)
        err (EXIT_FAILURE, NULL);

    t_time  = time (NULL);
    tm_time = localtime (&t_time);

    strftime (s_date, 64, "%Y%m%d", tm_time);

    cq_query->s_date      = strdup (s_date);
    cq_query->s_query     = strdup (s_query);
    cq_query->i_page      = i_page;
    cq_query->i_per_page  = i_per_page;
    cq_query->i_found_cnt = 0;

    cq_query->s_file = cfgfile_get_query_path ();
    dir_create_with_subdirs (cq_query->s_file);
    str_append (&cq_query->s_file, "/");
    str_append (&cq_query->s_file, s_service_name);
    str_append (&cq_query->s_file, ".json");

    cq_query->si_items = malloc (
            (size_t)(i_per_page + 1) * sizeof (CacheQuery*));

    if (cq_query->si_items == NULL)
        err (EXIT_FAILURE, NULL);

    for (i = 0; i < i_per_page + 1; ++i)
        cq_query->si_items[i] = NULL;

    return cq_query;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add SearchItem item to list in CacheQuery item.
 */
void
cachequery_add_item (CacheQuery *cq_query,
                     SearchItem *si_item)
{
    for (int i = 0; i < cq_query->i_per_page; ++i) {
        if (cq_query->si_items[i] == NULL) {
            cq_query->si_items[i] = si_item;
            break;
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if there is a cached data for query.
 */
CacheQuery *
cachequery_check_query (const char *s_service_name,
                        const char *s_query,
                        const int   i_page,
                        const int   i_per_page,
                        int        *i_err)
{
    CacheQuery  *cq_query = NULL;  /* Cached query data to return */
    SearchItem  *si_item  = NULL;  /* For image info data */
    size_t       ui_cnt   = 0;     /* Number of elements in array */
    size_t       i        = 0;     /* i */
    int          i_goon   = 1;     /* Go on */
    json_object *j_obj;            /* Json object made from file data */
    json_object *j_date;           /* Date of search */
    json_object *j_query;          /* Query text */
    json_object *j_per_page;       /* Elements per page */
    json_object *j_fnd_cnt;        /* For number of found images */
    json_object *j_items;          /* Array with image info data */
    json_object *j_val;            /* For array tith image info values */
    char s_page[10];               /* String with page number */

    *i_err   = ERR_OK;
    cq_query = cachequery_new (s_service_name, s_query, i_page, i_per_page);
    sprintf (s_page, "%d", cq_query->i_page);

    if ((j_obj = js_open_file (cq_query->s_file, NULL, i_err)) == NULL) {
        cachequery_free (cq_query);
        return NULL;
    }

    if (i_goon) {
        i_goon = (json_object_object_get_ex (j_obj, cq_query->s_date, &j_date)
            && json_object_get_type (j_date) == json_type_object);
    }
    if (i_goon) {
        i_goon = (json_object_object_get_ex (j_date, s_query, &j_query) &&
            json_object_get_type (j_query) == json_type_object);
    }
    if (i_goon) {
        i_goon = 0;
        if (json_object_object_get_ex (j_query, JS_PER_PAGE, &j_per_page)
            && json_object_get_type (j_per_page) == json_type_int) {

            cq_query->i_per_page = json_object_get_int (j_per_page);
            i_goon = (cq_query->i_per_page == i_per_page);
        }
    }
    if (i_goon) {
        i_goon = 0;
        if (json_object_object_get_ex (j_query, JS_FOUND_CNT, &j_fnd_cnt)
            && json_object_get_type (j_fnd_cnt) == json_type_int) {

            cq_query->i_found_cnt = json_object_get_int (j_fnd_cnt);
            i_goon = 1;
        }
    }
    if (i_goon) {
        i_goon = 0;
        if (json_object_object_get_ex (j_query, s_page, &j_items) &&
            json_object_get_type (j_items) == json_type_array) {

            ui_cnt = json_object_array_length (j_items);

            for (i = 0; i < ui_cnt; ++i) {
                j_val = json_object_array_get_idx (j_items, i);
                if (j_val != NULL) {
                    si_item = chquery_json_to_searchitem (j_val);
                    cachequery_add_item (cq_query, si_item);
                }
            }
            i_goon = 1;
        }
    }
    json_object_put (j_obj);
    if (!i_goon) {
        cachequery_free (cq_query);
        cq_query = NULL;
    }
    *i_err = ERR_OK;
    return cq_query;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save cached search data to a service cache file.
 */
int
cachequery_save (CacheQuery *cq_query)
{
    json_object   *j_obj;             /* Json object made from file data */
    json_object   *j_dates;           /* For query date */
    json_object   *j_queries;         /* For queries */
    json_object   *j_val;             /* For getting values */
    json_object   *j_arr;             /* For array with items */
    const char    *s_jbuff = NULL;    /* Json object as string */
    int            i_err   = ERR_OK;  /* For error output */
    char           s_page[10];        /* Page number as string */
    uint_fast32_t  ui_hash = 0;       /* Json data file hash */

    sprintf (s_page, "%d", cq_query->i_page);

    if ((j_obj = js_open_file (cq_query->s_file, &ui_hash, &i_err)) == NULL)
        return i_err;

    if (json_object_object_get_ex (j_obj, cq_query->s_date, &j_val) &&
        json_object_get_type (j_val) == json_type_object) {

        j_dates = j_val;
    }
    else {
        j_dates = json_object_new_object ();
        json_object_object_add (j_obj, cq_query->s_date, j_dates);
    }

    if (json_object_object_get_ex (j_dates, cq_query->s_query, &j_val) &&
        json_object_get_type (j_val) == json_type_object) {

        j_queries = j_val;
    }
    else {
        j_queries = json_object_new_object ();
        json_object_object_add (j_dates, cq_query->s_query, j_queries);
    }

    j_val = json_object_new_int (cq_query->i_found_cnt);
    json_object_object_add (j_queries, JS_FOUND_CNT, j_val);

    j_val = json_object_new_int (cq_query->i_per_page);
    json_object_object_add (j_queries, JS_PER_PAGE, j_val);

    j_arr = cachequery_searchitems_to_json_array (cq_query);
    json_object_object_add (j_queries, s_page, j_arr);
    s_jbuff = json_object_to_json_string (j_obj);

    if (hash (s_jbuff) != ui_hash)
        i_err = save_file_data (cq_query->s_file, s_jbuff);

    json_object_put (j_obj);

    return i_err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check cache query file for entries older that i_days and delete
 *         them.
 */
int
cachequery_delete_older_than (const char *s_service_name,
                              const int   i_days)
{
    json_object  *j_obj;            /* Json item with query info */
    GDate        *gd_date;          /* GDate for date count */
    time_t        t_time;           /* time_t for date count */
    char          s_date_n[64];     /* Temp date string */
    char        **s_dates = NULL;   /* List of dates not to delete */
    char         *s_file  = NULL;   /* Name of file with query info */
    const char   *s_jbuff = NULL;   /* Json file content buffer */
    int           i       = 0;      /* i */
    int           i_ndel  = 0;      /* Item not to delete */
    int           i_err   = ERR_OK; /* For error output */
    uint_fast32_t ui_hash = 0;      /* Json file content hash */

    t_time  = time (NULL);
    gd_date = g_date_new ();

    g_date_set_time_t (gd_date, t_time);

    s_file = cfgfile_get_query_path ();
    dir_create_with_subdirs (s_file);
    str_append (&s_file, "/");
    str_append (&s_file, s_service_name);
    str_append (&s_file, ".json");

    if ((j_obj = js_open_file (s_file, &ui_hash, &i_err)) == NULL) {
        free (s_file);
        return i_err;
    }
    s_dates = malloc ((size_t) i_days * sizeof (char*));

    for (i = 0; i < i_days; ++i) {
        g_date_strftime (s_date_n, 64, "%Y%m%d", gd_date);
        s_dates[i] = strdup (s_date_n);
        g_date_subtract_days (gd_date, 1);
    }
    json_object_object_foreach (j_obj, key, val1) {
        i_ndel = 0;
        for (i = 0; i < i_days; ++i)
            i_ndel |= (strcmp (key, s_dates[i]) == 0);
        if (!i_ndel)
            json_object_object_del (j_obj, key);
    }
    s_jbuff = json_object_to_json_string (j_obj);

    if (hash (s_jbuff) != ui_hash) {
        i_err = save_file_data (s_file, s_jbuff);
    }
    for (i = 0; i < i_days; ++i) {
        free (s_dates[i]);
    }
    free (s_file);
    free (s_dates);
    json_object_put (j_obj);
    g_date_free (gd_date);
    return i_err;
}
/*----------------------------------------------------------------------------*/

