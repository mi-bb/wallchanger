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
 *
 * @var   UrlData::http_status
 * @brief HTTP status of the response, 0 when there was no response
 */
typedef struct
UrlData {
    char   *buffer;
    size_t  size;
    char   *errbuf;
    long    http_status;
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
 * Redirects are followed and a HTTP status of 400 or more is reported as an
 * error in UrlData::errbuf.
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
 * @brief  Percent-encode a string for use as a url query value.
 *
 * @param[in] str  String to encode
 * @return    New encoded string, empty on error; free with free()
 */
[[nodiscard]] char * urldata_escape (const char *str);
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
