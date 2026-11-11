/**
 * @file  web_provider.h
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
 * @brief  Description of an image search service.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef WEB_PROVIDER_H
#define WEB_PROVIDER_H

#include "gui/icons.h"
#include "search_item.h"
#include "webwidget_struct.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   WEB_PAGE_SIZE
 * @brief Number of images requested per results page from the services that
 *        let the client choose it.
 */
constexpr int WEB_PAGE_SIZE = 20;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Function receiving the images found by WebProvider::parse.
 *
 * @param[in] item       Found image, the function takes its ownership
 * @param[in] user_data  Data passed to WebProvider::parse
 */
typedef void (*WebItemFunc) (SearchItem *item,
                             void       *user_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Kinds of search options shown by the generic options dialog.
 */
typedef enum WebOptType : int {
    WEB_OPT_INT,   /**< Number from 0 to WebOption::max, 0 means not set */
    WEB_OPT_CHOICE /**< One of WebOption::choices */
} WebOptType;
/*----------------------------------------------------------------------------*/
/**
 * @struct WebChoice
 *
 * @brief  One value of a WEB_OPT_CHOICE search option.
 *
 * @var   WebChoice::value
 * @brief Value saved in settings; an empty value means the option is not set
 *
 * @var   WebChoice::label
 * @brief Text shown to the user
 */
typedef struct WebChoice {
    const char *value;
    const char *label;
} WebChoice;
/*----------------------------------------------------------------------------*/
/**
 * @struct WebOption
 *
 * @brief  Search option shown by the generic options dialog.
 *
 * Options that are set are saved under WebProvider::opts_name and passed to
 * WebProvider::search_url as a "&key=value&key=value" string.
 *
 * @var   WebOption::key
 * @brief Name of the option, null in the item ending an option list
 *
 * @var   WebOption::label
 * @brief Text shown next to the option's widget
 *
 * @var   WebOption::tooltip
 * @brief Tooltip of the option's widget
 *
 * @var   WebOption::type
 * @brief Kind of option
 *
 * @var   WebOption::max
 * @brief Largest value of a WEB_OPT_INT option
 *
 * @var   WebOption::choices
 * @brief Values of a WEB_OPT_CHOICE option, ended by an item with a null
 *        value. The first one is the default.
 */
typedef struct WebOption {
    const char      *key;
    const char      *label;
    const char      *tooltip;
    WebOptType       type;
    int              max;
    const WebChoice *choices;
} WebOption;
/*----------------------------------------------------------------------------*/
/**
 * @struct WebProvider
 *
 * @brief  Description of an image search service.
 *
 * A service is searched by getting WebProvider::search_url and passing the
 * response to WebProvider::parse. Services whose search response holds only
 * image ids also set WebProvider::detail_url and WebProvider::parse_detail,
 * which are then used once for every found image.
 *
 * @var   WebProvider::name
 * @brief Short name without spaces. It is the prefix of the downloaded image
 *        and thumbnail file names and the name of the query cache file.
 *
 * @var   WebProvider::label
 * @brief Name shown to the user
 *
 * @var   WebProvider::opts_name
 * @brief Name of the setting holding the search options
 *
 * @var   WebProvider::api_key_name
 * @brief Name of the setting holding the API key, null when the service does
 *        not use one
 *
 * @var   WebProvider::api_key_info
 * @brief Text for the API key dialog saying what the key is for and how to
 *        get it
 *
 * @var   WebProvider::api_key_url
 * @brief Address of the page where the user gets an API key
 *
 * @var   WebProvider::logo
 * @brief Logo of the service, W_IMG_COUNT when there is none
 *
 * @var   WebProvider::options
 * @brief Options for the generic options dialog ended by an item with a null
 *        key, null when the service has none or sets WebProvider::opts_dialog
 *
 * @var   WebProvider::search_url
 * @brief Build the search request url. Gets the query as typed by the user,
 *        the options string, the API key (can be empty) and the page number
 *        starting from 1. Returns a new string to free with free(), or null
 *        when the service does not serve that page of results.
 *
 * @var   WebProvider::parse
 * @brief Parse the search response. Sets the number of found images and
 *        calls the WebItemFunc for every image that can be used. Returns
 *        false when the response is not a search result.
 *
 * @var   WebProvider::detail_url
 * @brief Build the url of the request for the data of the image with the
 *        given id. Returns a new string to free with free(). Null when the
 *        search response already holds the image data.
 *
 * @var   WebProvider::parse_detail
 * @brief Parse the response of the WebProvider::detail_url request into
 *        the item. Returns false when the image can not be used.
 *
 * @var   WebProvider::opts_dialog
 * @brief Service's own search options dialog. Returns the new options string
 *        to free with free(), or null when the dialog was cancelled.
 */
typedef struct WebProvider {
    const char      *name;
    const char      *label;
    const char      *opts_name;
    const char      *api_key_name;
    const char      *api_key_info;
    const char      *api_key_url;
    IconImg          logo;
    const WebOption *options;

    char * (*search_url)   (const char  *query,
                            const char  *search_opts,
                            const char  *api_key,
                            const int    page);

    bool   (*parse)        (const char  *json,
                            int         *found_cnt,
                            WebItemFunc  add_item,
                            void        *user_data);

    char * (*detail_url)   (const char  *id);

    bool   (*parse_detail) (const char  *json,
                            SearchItem  *item);

    char * (*opts_dialog)  (WebWidget   *widget);
} WebProvider;
/*----------------------------------------------------------------------------*/
#endif
