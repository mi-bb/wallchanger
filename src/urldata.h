/**
 * @file  urldata.h
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
 * @brief  Functions for receiving data from url
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef URLDATA_H
#define URLDATA_H

#include <stddef.h>
/*----------------------------------------------------------------------------*/
/**
 * @struct UrlData
 *
 * @brief  Structure for handling download data using curl.
 *
 * @var   UrlData::buffer
 * @brief Buffer for downloaded data
 *
 * @var   UrlData::size
 * @brief Size of buffer
 *
 * @var   UrlData::errbuf
 * @brief Buffer for error information
 */
typedef struct
UrlData {
    char   *buffer;
    size_t  size;
    char   *errbuf;
} UrlData;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new UrlData item.
 *
 * @return New UrlData item
 */
UrlData * urldata_new           (void);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free UrlData item.
 *
 * @param[out] ud_data  UrlData item to free
 * @return     none
 */
void      urldata_free          (UrlData    *ud_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data from given url.
 *
 * @param[in] s_url  Url to download data
 * @return    UrlData item with downloaded data or error info
 */
UrlData * urldata_get_data      (const char *s_url);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Donwload data from url to file.
 *
 * @param[in]  s_url  Url to download data
 * @param[in]  s_fn   File where data should be saved
 * @param[out] s_err  String for error output
 * @return     none
 */
void      urldata_get_to_file   (const char *s_url,
                                 const char *s_fn,
                                 char      **s_err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Pexels for images.
 *
 * @param[in] s_query        Search query
 * @param[in] s_search_opts  Search options
 * @param[in] s_api_key      Pexels API key
 * @param[in] i_page         Results page number
 * @return    UrlData item with downloaded data or error info
 */
UrlData * urldata_search_pexels (const char *s_query,
                                 const char *s_search_opts,
                                 const char *s_api_key,
                                 const int   i_page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Pixbay for images.
 *
 * @param[in] s_query        Search query
 * @param[in] s_search_opts  Search options
 * @param[in] s_api_key      Pixbay API key
 * @param[in] i_page         Results page number
 * @return    UrlData item with downloaded data or error info
 */
UrlData * urldata_search_pixbay (const char *s_query,
                                 const char *s_search_opts,
                                 const char *s_api_key,
                                 const int   i_page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Wallhaven for images.
 *
 * @param[in] s_query        Search query
 * @param[in] s_search_opts  Search options
 * @param[in] s_api_key      Wallhaven API key
 * @param[in] i_page         Results page number
 * @return    UrlData item with downloaded data or error info
 */
UrlData * urldata_search_wallhaven (const char *s_query,
                                    const char *s_search_opts,
                                    const char *s_api_key,
                                    const int   i_page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Wallpaper Abyss for images.
 *
 * @param[in] s_query        Search query
 * @param[in] s_search_opts  Search options
 * @param[in] s_api_key      Wallhaven API key
 * @param[in] i_page         Results page number
 * @return    UrlData item with downloaded data or error info
 */
UrlData * urldata_search_wallabyss (const char *s_query,
                                    const char *s_search_opts,
                                    const char *s_api_key,
                                    const int   i_page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if UrlData's data buffer is not empty.
 *
 * @param[in] ud_data  UrlData item to check
 * @return    1 if data buffer is full, 0 if not
 */
static inline int urldata_full (UrlData *ud_data) {
    return ud_data->buffer[0] != '\0';
}
/*----------------------------------------------------------------------------*/
#endif

