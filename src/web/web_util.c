/**
 * @file  web_util.c
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
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <err.h>
#include "web_util.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse a json response whose top level value is an object.
 */
json_object *
web_json_parse (const char *json)
{
    json_object *obj = nullptr;
    enum json_tokener_error json_err = json_tokener_success;

    if (json == nullptr)
        return nullptr;

    obj = json_tokener_parse_verbose (json, &json_err);

    if (json_err != json_tokener_success ||
        json_object_get_type (obj) != json_type_object) {

        if (obj != nullptr)
            json_object_put (obj);
        return nullptr;
    }
    return obj;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get a member of a json object.
 */
json_object *
web_json_get (json_object    *obj,
              const char     *key,
              const json_type type)
{
    json_object *val = nullptr;

    if (obj != nullptr &&
        json_object_get_type (obj) == json_type_object &&
        json_object_object_get_ex (obj, key, &val) &&
        json_object_get_type (val) == type) {

        return val;
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get a string member of a json object.
 */
const char *
web_json_str (json_object *obj,
              const char  *key)
{
    json_object *val = web_json_get (obj, key, json_type_string);
    const char  *str = nullptr;

    if (val != nullptr)
        str = json_object_get_string (val);

    return (str != nullptr && str[0] != '\0') ? str : nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get an integer member of a json object.
 */
int
web_json_int (json_object *obj,
              const char  *key)
{
    json_object *val = web_json_get (obj, key, json_type_int);

    return val != nullptr ? json_object_get_int (val) : 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the element of a json array.
 */
json_object *
web_json_idx (json_object *array,
              const size_t index)
{
    if (array == nullptr ||
        json_object_get_type (array) != json_type_array ||
        index >= (size_t) json_object_array_length (array)) {

        return nullptr;
    }
    return json_object_array_get_idx (array, index);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find the value of an option in a search options string.
 *
 * @param[in] search_opts  Options in "&key=value&key=value" format, can be
 *                         null
 * @param[in] key          Option name
 * @return    Pointer to the first char of the value inside search_opts, or
 *            null when the option is not set
 */
static const char *
web_opts_find (const char *search_opts,
               const char *key)
{
    const char  *pos     = search_opts;
    const size_t key_len = strlen (key);

    while (pos != nullptr && (pos = strchr (pos, '&')) != nullptr) {
        ++pos;
        if (strncmp (pos, key, key_len) == 0 && pos[key_len] == '=')
            return pos + key_len + 1;
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the value of an integer option from a search options string.
 */
int
web_opts_get_int (const char *search_opts,
                  const char *key)
{
    const char *val = web_opts_find (search_opts, key);
    long        num = 0;

    if (val != nullptr)
        num = strtol (val, nullptr, 10);

    return (num > 0 && num <= 1000000) ? (int) num : 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if an option is present in a search options string.
 */
bool
web_opts_has (const char *search_opts,
              const char *key)
{
    return web_opts_find (search_opts, key) != nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Decode the character entity at the start of a string.
 *
 * @param[in]  str  String starting with '&'
 * @param[out] chr  Decoded character
 * @return     Length of the entity, 0 when it is not a known one
 */
static size_t
web_decode_entity (const char *str,
                   char       *chr)
{
    static const struct {
        const char *name;
        char        chr;
    } entities[] = {
        {"&amp;",  '&'}, {"&lt;",   '<'},  {"&gt;",   '>'}, {"&quot;", '"'},
        {"&#039;", '\''}, {"&#39;", '\''}, {"&apos;", '\''}, {"&nbsp;", ' '},
        {"&#160;", ' '}
    };
    size_t len = 0;

    for (size_t i = 0; i < sizeof (entities) / sizeof (entities[0]); ++i) {
        len = strlen (entities[i].name);
        if (strncmp (str, entities[i].name, len) == 0) {
            *chr = entities[i].chr;
            return len;
        }
    }
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Turn a piece of html into plain text.
 */
char *
web_strip_html (const char *html)
{
    char  *text   = nullptr; /* Result text */
    size_t len    = 0;       /* Length of result text */
    size_t ent    = 0;       /* Length of a character entity */
    bool   in_tag = false;   /* Inside of a html tag */
    char   chr    = '\0';    /* Char to add to result */

    if (html == nullptr)
        return nullptr;

    /* Plain text is never longer than the html it comes from */
    if ((text = malloc (strlen (html) + 1)) == nullptr)
        err (EXIT_FAILURE, nullptr);

    while (*html != '\0') {
        chr = *html;
        ent = 1;

        if (in_tag) {
            in_tag = (chr != '>');
            ++html;
            continue;
        }
        if (chr == '<') {
            /* A tag separates words like a space does */
            in_tag = true;
            chr = ' ';
        }
        else if (chr == '&' && (ent = web_decode_entity (html, &chr)) == 0) {
            ent = 1;
        }
        html += ent;

        if (isspace ((unsigned char) chr)) {
            if (len == 0 || text[len - 1] == ' ')
                continue;
            chr = ' ';
        }
        text[len++] = chr;
    }
    while (len > 0 && text[len - 1] == ' ')
        --len;

    text[len] = '\0';

    if (len == 0) {
        free (text);
        text = nullptr;
    }
    return text;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the file extension of an image with the given media type.
 */
const char *
web_ext_from_mime (const char *mime)
{
    if (mime == nullptr)
        return nullptr;
    if (strcasecmp (mime, "image/jpeg") == 0)
        return ".jpg";
    if (strcasecmp (mime, "image/png") == 0)
        return ".png";
    if (strcasecmp (mime, "image/webp") == 0)
        return ".webp";

    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the file extension of an image from its url.
 */
const char *
web_ext_from_url (const char *url)
{
    static const struct {
        const char *url_ext;
        const char *ext;
    } exts[] = {
        {".jpg", ".jpg"}, {".jpeg", ".jpg"}, {".png", ".png"},
        {".webp", ".webp"}
    };
    size_t url_len = 0; /* Url length without query and fragment */
    size_t ext_len = 0; /* Length of extension to compare */

    if (url == nullptr)
        return nullptr;

    url_len = strcspn (url, "?#");

    for (size_t i = 0; i < sizeof (exts) / sizeof (exts[0]); ++i) {
        ext_len = strlen (exts[i].url_ext);
        if (url_len > ext_len &&
            strncasecmp (url + url_len - ext_len,
                         exts[i].url_ext, ext_len) == 0) {
            return exts[i].ext;
        }
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
