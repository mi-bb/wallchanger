/**
 * @file  web_util.h
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
 * @brief  Helpers shared by the image search services.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef WEB_UTIL_H
#define WEB_UTIL_H

#include "config.h"
#ifdef HAVE_JSON_C_JSON_H
#include <json-c/json.h>
#else
#include <json.h>
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse a json response whose top level value is an object.
 *
 * @param[in] json  String with json data
 * @return    Json object to release with json_object_put, or null when the
 *            data is not a valid json object
 */
[[nodiscard]] json_object * web_json_parse (const char *json);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get a member of a json object.
 *
 * @param[in] obj   Json object, can be null
 * @param[in] key   Member name
 * @param[in] type  Required type of the member
 * @return    Member, or null when obj is not an object or has no member of
 *            that name and type
 */
[[nodiscard]] json_object * web_json_get (json_object    *obj,
                                          const char     *key,
                                          const json_type type);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get a string member of a json object.
 *
 * @param[in] obj  Json object, can be null
 * @param[in] key  Member name
 * @return    Member value owned by obj, or null when there is no not empty
 *            string member of that name
 */
[[nodiscard]] const char * web_json_str (json_object *obj,
                                         const char  *key);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get an integer member of a json object.
 *
 * @param[in] obj  Json object, can be null
 * @param[in] key  Member name
 * @return    Member value, or 0 when there is no integer member of that name
 */
[[nodiscard]] int web_json_int (json_object *obj,
                                const char  *key);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the element of a json array.
 *
 * @param[in] array  Json array, can be null
 * @param[in] index  Element index
 * @return    Element, or null when array is not an array or is too short
 */
[[nodiscard]] json_object * web_json_idx (json_object *array,
                                          const size_t index);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the value of an integer option from a search options string.
 *
 * @param[in] search_opts  Options in "&key=value&key=value" format, can be
 *                         null
 * @param[in] key          Option name
 * @return    Option value, or 0 when it is not set
 */
[[nodiscard]] int web_opts_get_int (const char *search_opts,
                                    const char *key);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if an option is present in a search options string.
 *
 * @param[in] search_opts  Options in "&key=value&key=value" format, can be
 *                         null
 * @param[in] key          Option name
 * @return    true if the option is set
 */
[[nodiscard]] bool web_opts_has (const char *search_opts,
                                 const char *key);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Turn a piece of html into plain text.
 *
 * Tags are removed, the common character entities are decoded and runs of
 * white space are replaced with a single space.
 *
 * @param[in] html  String with html, can be null
 * @return    New string, null when html is null or holds no text;
 *            free with free()
 */
[[nodiscard]] char * web_strip_html (const char *html);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the file extension of an image with the given media type.
 *
 * @param[in] mime  Media type, e.g. "image/jpeg", can be null
 * @return    Extension with the dot, or null for a type that is not one of
 *            the image formats usable as a wallpaper (jpeg, png, webp)
 */
[[nodiscard]] const char * web_ext_from_mime (const char *mime);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the file extension of an image from its url.
 *
 * The query and fragment parts of the url are ignored.
 *
 * @param[in] url  Image url, can be null
 * @return    Extension with the dot, or null when the url does not end with
 *            the extension of one of the image formats usable as a wallpaper
 *            (jpeg, png, webp)
 */
[[nodiscard]] const char * web_ext_from_url (const char *url);
/*----------------------------------------------------------------------------*/
#endif
