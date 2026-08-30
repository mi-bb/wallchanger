/**
 * @file  chquery.h
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
#ifndef CHQUERY_H
#define CHQUERY_H
#include <gtk/gtk.h>
#include "searchitem.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct CacheQuery
 *
 * @brief  Structure for storing wallpaper search query data.
 *
 * @var   CacheQuery::si_items
 * @brief List of SearchItem items with image search data.
 *
 * @var   CacheQuery::s_file
 * @brief Name of file with cached data
 *
 * @var   CacheQuery::s_date
 * @brief Date of search query
 *
 * @var   CacheQuery::s_query
 * @brief Query string
 *
 * @var   CacheQuery::s_search_opts
 * @brief String with search options
 *
 * @var   CacheQuery::i_sicnt
 * @brief Number of SearchItem images
 *
 * @var   CacheQuery::i_page
 * @brief Page number
 *
 * @var   CacheQuery::i_found_cnt
 * @brief Number of found images
 */
typedef struct
CacheQuery {
    SearchItem **si_items;
    char        *s_file;
    char        *s_date;
    char        *s_query;
    char        *s_search_opts;
    int          i_sicnt;
    int          i_page;
    int          i_found_cnt;
} CacheQuery;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new CacheQuery item.
 *
 * @param[in] s_service_name  Name of image service.
 * @param[in] s_query         Search query
 * @param[in] s_search_opts   Search options
 * @param[in] i_page          Page number
 * @return    New CacheQuery item, it should be freed using cachequery_free
 */
[[nodiscard]]
CacheQuery * cachequery_new               (const char *s_service_name,
                                           const char *s_query,
                                           const char *s_search_opts,
                                           const int   i_page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free CacheQuery item.
 *
 * @param[out] cq_query  Item to free data
 * @return     none
 */
void         cachequery_free              (CacheQuery *cq_query);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append SearchItem item to list.
 *
 * @param[out] cq_query  CacheQuery item
 * @param[in]  si_item   SearchItem item to add
 * @return     none
 */
void         cachequery_append_item       (CacheQuery *cq_query,
                                           SearchItem *si_item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if there is a cached data for query.
 *
 * @param[in]  s_service_name  Name of image service.
 * @param[in]  s_query         Search query
 * @param[in]  s_search_opts   Search options
 * @param[in]  i_page          Page number
 * @param[out] i_err           Error output
 * @return     CacheQuery item with search results or null if there is no cached
 *             data, it should be freed using cachequery_free
 */
[[nodiscard]]
CacheQuery * cachequery_check_query       (const char *s_service_name,
                                           const char *s_query,
                                           const char *s_search_opts,
                                           const int   i_page,
                                           int        *i_err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save cached search data to a service cache file.
 *
 * @param[in] cq_query  CacheQuery item
 * @return    Process status
 */
int          cachequery_save              (CacheQuery *cq_query);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check cache query file for entries older that i_days and delete
 *         them.
 *
 * @param[in]  s_service_name  Name of image service.
 * @param[in]  i_days          Entries older that this value will be removed
 * @return     Process status
 */
int          cachequery_delete_older_than (const char *s_service_name,
                                           const int   i_days);
/*----------------------------------------------------------------------------*/
#endif

