/**
 * @file  live_source.h
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
 * @brief  Description of a remote live wallpaper service.
 *
 * Remote live services give a wallpaper without the user choosing an image:
 * an image of the day or a random one. Unlike the search services in web/
 * this code is linked into the daemon, so it must not use GTK or GLib.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef LIVE_SOURCE_H
#define LIVE_SOURCE_H

#include <stddef.h>
#include "util/randomm.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct LiveParams
 *
 * @brief  Data a service needs to choose a wallpaper.
 *
 * @var   LiveParams::screen_width
 * @brief Screen width in pixels
 *
 * @var   LiveParams::screen_height
 * @brief Screen height in pixels
 *
 * @var   LiveParams::keywords
 * @brief Keywords typed by the user, can be null or empty
 *
 * @var   LiveParams::date
 * @brief Present UTC date in "YYYY-MM-DD" format
 */
typedef struct LiveParams {
    int         screen_width;
    int         screen_height;
    const char *keywords;
    const char *date;
} LiveParams;
/*----------------------------------------------------------------------------*/
/**
 * @struct LiveImage
 *
 * @brief  Wallpaper chosen by a service. All strings are owned by the item,
 *         every one of them can be null.
 *
 * @var   LiveImage::image_url
 * @brief Address of the image file to download
 *
 * @var   LiveImage::file_name
 * @brief Name of the file in the cache directory. The same image always gets
 *        the same name, so an image already in the cache is not downloaded
 *        again.
 *
 * @var   LiveImage::title
 * @brief Image title
 *
 * @var   LiveImage::author
 * @brief Image author
 *
 * @var   LiveImage::license
 * @brief Name of the image license
 *
 * @var   LiveImage::license_url
 * @brief Address of the license text
 *
 * @var   LiveImage::page_url
 * @brief Address of the image page on the service
 */
typedef struct LiveImage {
    char *image_url;
    char *file_name;
    char *title;
    char *author;
    char *license;
    char *license_url;
    char *page_url;
} LiveImage;
/*----------------------------------------------------------------------------*/
/**
 * @struct LiveSource
 *
 * @brief  Description of a remote live wallpaper service.
 *
 * A wallpaper is chosen by getting LiveSource::request_url and passing the
 * response to LiveSource::parse.
 *
 * @var   LiveSource::name
 * @brief Short name without spaces. It is saved in the settings and is the
 *        prefix of the cached file names.
 *
 * @var   LiveSource::label
 * @brief Name shown to the user
 *
 * @var   LiveSource::info
 * @brief Description shown as a tooltip
 *
 * @var   LiveSource::daily
 * @brief true when the service changes its image once a day, so it is asked
 *        again only after LIVE_DAILY_CHECK_SEC
 *
 * @var   LiveSource::keywords_setting
 * @brief Name of the setting with keywords typed by the user, null when the
 *        service does not use keywords
 *
 * @var   LiveSource::request_url
 * @brief Build the request url. Returns a new string to free with free().
 *
 * @var   LiveSource::parse
 * @brief Parse the response and fill the image. Returns false when the
 *        response holds no image that can be used as a wallpaper.
 */
typedef struct LiveSource {
    const char *name;
    const char *label;
    const char *info;
    bool        daily;
    const char *keywords_setting;

    char * (*request_url) (const LiveParams *params);

    bool   (*parse)       (const char       *json,
                           const LiveParams *params,
                           LiveImage        *image);
} LiveSource;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free the strings of an image and set them to null.
 *
 * @param[in,out] image  Image to clear
 * @return        none
 */
void live_image_clear (LiveImage *image);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set a string member of an image to a copy of a value.
 *
 * @param[in,out] member  Member to set, its old value is freed
 * @param[in]     value   Value to copy, can be null
 * @return        none
 */
void live_image_set (char       **member,
                     const char  *value);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build a cache file name from parts.
 *
 * The parts are joined with '_' after the service name, the extension is
 * appended and every character that is not a letter, digit, '.', '-' or '_'
 * is replaced with '_'.
 *
 * @param[in] service  Service name
 * @param[in] id       Image id
 * @param[in] ext      Extension with the dot, can be null
 * @return    New string to free with free()
 */
[[nodiscard]] char * live_file_name (const char *service,
                                     const char *id,
                                     const char *ext);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Choose the service to ask first.
 *
 * @param[in]     count     Number of enabled services
 * @param[in]     last      Position of the last used service, or -1
 * @param[in]     random    Choose randomly instead of the next in order
 * @param[in,out] rand_mem  Random memory with range set to count, used only
 *                          when random is true
 * @return        Position of the service, 0 when count is 0
 */
[[nodiscard]] size_t live_pick_first (const size_t count,
                                      const long   last,
                                      const bool   random,
                                      RandMem     *rand_mem);
/*----------------------------------------------------------------------------*/
#endif
