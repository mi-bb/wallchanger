/**
 * @file  cache_query.h
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
#ifndef CACHE_QUERY_H
#define CACHE_QUERY_H
#include <gtk/gtk.h>
#include "search_item.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct CacheQuery
 *
 * @brief  Structure for storing wallpaper search query data.
 *
 * @var   CacheQuery::items
 * @brief List of SearchItem items with image search data.
 *
 * @var   CacheQuery::file
 * @brief Name of file with cached data
 *
 * @var   CacheQuery::date
 * @brief Date of search query
 *
 * @var   CacheQuery::query
 * @brief Query string
 *
 * @var   CacheQuery::search_opts
 * @brief String with search options
 *
 * @var   CacheQuery::count
 * @brief Number of SearchItem images
 *
 * @var   CacheQuery::page
 * @brief Page number
 *
 * @var   CacheQuery::found_cnt
 * @brief Number of found images
 */
typedef struct
CacheQuery {
    SearchItem **items;
    char        *file;
    char        *date;
    char        *query;
    char        *search_opts;
    int          count;
    int          page;
    int          found_cnt;
} CacheQuery;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new CacheQuery item.
 *
 * @param[in] service_name  Name of image service.
 * @param[in] query         Search query
 * @param[in] search_opts   Search options
 * @param[in] page          Page number
 * @return    New CacheQuery item, it should be freed using cache_query_free
 */
[[nodiscard]]
CacheQuery * cache_query_new               (const char *service_name,
                                            const char *query,
                                            const char *search_opts,
                                            const int   page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free CacheQuery item.
 *
 * @param[out] cq_query  Item to free data
 * @return     none
 */
void         cache_query_free              (CacheQuery *cq_query);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append SearchItem item to list.
 *
 * @param[out] cq_query  CacheQuery item
 * @param[in]  item      SearchItem item to add
 * @return     none
 */
void         cache_query_append_item       (CacheQuery *cq_query,
                                            SearchItem *item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if there is a cached data for query.
 *
 * @param[in]  service_name  Name of image service.
 * @param[in]  query         Search query
 * @param[in]  search_opts   Search options
 * @param[in]  page          Page number
 * @param[out] err           Error output
 * @return     CacheQuery item with search results or null if there is no cached
 *             data, it should be freed using cache_query_free
 */
[[nodiscard]]
CacheQuery * cache_query_check_query       (const char *service_name,
                                            const char *query,
                                            const char *search_opts,
                                            const int   page,
                                            int        *err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save cached search data to a service cache file.
 *
 * @param[in] cq_query  CacheQuery item
 * @return    Process status
 */
int          cache_query_save              (CacheQuery *cq_query);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check cache query file for entries older that days and delete
 *         them.
 *
 * @param[in]  service_name  Name of image service.
 * @param[in]  days          Entries older that this value will be removed
 * @return     Process status
 */
int          cache_query_delete_older_than (const char *service_name,
                                            const int   days);
/*----------------------------------------------------------------------------*/
#endif

