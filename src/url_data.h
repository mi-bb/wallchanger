/**
 * @file  url_data.h
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
 * @brief  Functions for receiving data from url
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef URL_DATA_H
#define URL_DATA_H

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
 * @param[out] url_data  UrlData item to free
 * @return     none
 */
void      urldata_free          (UrlData    *url_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data from given url.
 *
 * @param[in] url  Url to download data
 * @return    UrlData item with downloaded data or error info
 */
UrlData * urldata_get_data      (const char *url);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Download data from url to file.
 *
 * @param[in]  url  Url to download data
 * @param[in]  fn   File where data should be saved
 * @param[out] err  String for error output
 * @return     none
 */
void      urldata_get_to_file   (const char *url,
                                 const char *fn,
                                 char      **err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Pixabay for images.
 *
 * @param[in] query        Search query
 * @param[in] search_opts  Search options
 * @param[in] api_key      Pixabay API key
 * @param[in] page         Results page number
 * @return    UrlData item with downloaded data or error info
 */
UrlData * urldata_search_pixabay (const char *query,
                                  const char *search_opts,
                                  const char *api_key,
                                  const int   page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Wallhaven for images.
 *
 * @param[in] query        Search query
 * @param[in] search_opts  Search options
 * @param[in] api_key      Wallhaven API key
 * @param[in] page         Results page number
 * @return    UrlData item with downloaded data or error info
 */
UrlData * urldata_search_wallhaven (const char *query,
                                    const char *search_opts,
                                    const char *api_key,
                                    const int   page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Wallpaper Abyss for images.
 *
 * @param[in] query        Search query
 * @param[in] search_opts  Search options
 * @param[in] api_key      Wallpaper Abyss API key
 * @param[in] page         Results page number
 * @return    UrlData item with downloaded data or error info
 */
UrlData * urldata_search_wallabyss (const char *query,
                                   const char *search_opts,
                                   const char *api_key,
                                   const int   page);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if UrlData's data buffer is not empty.
 *
 * @param[in] ud  UrlData item to check
 * @return    1 if data buffer is full, 0 if not
 */
static inline int urldata_full (UrlData *ud) {
    return ud->buffer[0] != '\0';
}
/*----------------------------------------------------------------------------*/
#endif
