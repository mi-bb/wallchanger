/**
 * @file  url_data.c
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
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <curl/curl.h>
#include "config.h"
#include "url_data.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   URL_USER_AGENT
 * @brief Value of the User-Agent header sent with every request. The program
 *        always identifies itself, never as a web browser.
 *
 * @var   URL_MAX_REDIRECTS
 * @brief Number of redirects to follow before giving up.
 *
 * @var   URL_CONNECT_TIMEOUT
 * @brief Seconds to wait for a connection to the server.
 *
 * @var   URL_STALL_TIME
 * @brief Seconds without any received data after which a transfer is
 *        aborted, so a stalled connection can not block the program forever.
 */
#define URL_USER_AGENT PACKAGE "/" PACKAGE_VERSION " (+" PACKAGE_URL ")"
constexpr long URL_MAX_REDIRECTS   = 5;
constexpr long URL_CONNECT_TIMEOUT = 20;
constexpr long URL_STALL_TIME      = 60;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new UrlData item.
 */
UrlData *
urldata_new (void)
{
    UrlData *ud = nullptr;

    if ((ud = malloc (sizeof (UrlData))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    ud->size = 0;
    ud->errbuf = nullptr;
    ud->http_status = 0;

    if ((ud->buffer = malloc (sizeof (char))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    ud->buffer[0] = '\0';

    return ud;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free UrlData item.
 */
void
urldata_free (UrlData *url_data)
{
    if (url_data == nullptr)
        return;

    free (url_data->buffer);
    free (url_data->errbuf);
    free (url_data);
}
/*----------------------------------------------------------------------------*/
static size_t
url_write (void   *data,
           size_t  size,
           size_t  nmemb,
           void   *userp)
{
    UrlData *url_data = nullptr;
    char    *buff     = nullptr;
    size_t   bytes    = 0;
 
    bytes = size * nmemb;
    url_data = (UrlData *) userp;
    if ((buff = realloc (url_data->buffer, url_data->size + bytes + 1)) == nullptr)
        err (EXIT_FAILURE, nullptr);
 
    url_data->buffer = buff;
    memcpy (& (url_data->buffer[url_data->size]), data, bytes);
    url_data->size += bytes;
    url_data->buffer[url_data->size] = '\0';

    return bytes;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the options shared by every request.
 *
 * @param[in,out] curl    Curl handle to set up
 * @param[in]     url     Request url
 * @param[out]    errbuf  Buffer of CURL_ERROR_SIZE chars for error text
 * @return        List of extra request headers or null. It has to stay alive
 *                until the transfer ends, then be freed with
 *                curl_slist_free_all.
 */
[[nodiscard]] static struct curl_slist *
url_set_common_opts (CURL       *curl,
                     const char *url,
                     char       *errbuf)
{
    struct curl_slist *headers = nullptr;

    errbuf[0] = '\0';
    curl_easy_setopt (curl, CURLOPT_URL, url);
    curl_easy_setopt (curl, CURLOPT_USERAGENT, URL_USER_AGENT);
    curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt (curl, CURLOPT_MAXREDIRS, URL_MAX_REDIRECTS);
    curl_easy_setopt (curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, errbuf);
    curl_easy_setopt (curl, CURLOPT_CONNECTTIMEOUT, URL_CONNECT_TIMEOUT);
    curl_easy_setopt (curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
    curl_easy_setopt (curl, CURLOPT_LOW_SPEED_TIME, URL_STALL_TIME);
    curl_easy_setopt (curl, CURLOPT_NOSIGNAL, 1L);
#ifdef DEBUG
    curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
#endif
    /* The Art Institute of Chicago asks API clients to identify themselves
     * with this header; its image server rejects requests without it. */
    if (strstr (url, "://api.artic.edu/") != nullptr ||
        strstr (url, "://www.artic.edu/") != nullptr) {

        headers = curl_slist_append (headers,
                                     "AIC-User-Agent: " URL_USER_AGENT);
        curl_easy_setopt (curl, CURLOPT_HTTPHEADER, headers);
    }
    return headers;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data from given url.
 */
UrlData *
urldata_get_data (const char *url)
{
    UrlData  *url_data = nullptr;
    CURL     *curl;
    CURLcode  res;
    char      errbuf[CURL_ERROR_SIZE];
    struct curl_slist *headers = nullptr;

    url_data = urldata_new ();
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();

    if (curl) {
        headers = url_set_common_opts (curl, url, errbuf);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) url_data);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_write);

        res = curl_easy_perform (curl);
        curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE,
                           &url_data->http_status);
 
        if (res != CURLE_OK) {
            url_data->errbuf = strdup (strlen (errbuf) ?
                                      errbuf : curl_easy_strerror (res));
            warnx ("libcurl: (%d) %s", res, url_data->errbuf);
        }
        curl_slist_free_all (headers);
        curl_easy_cleanup (curl);
    }
    curl_global_cleanup ();

    return url_data;
}
/*----------------------------------------------------------------------------*/
static size_t
write_data (void   *ptr,
            size_t  size,
            size_t  nmemb,
            void   *stream)
{
  return fwrite (ptr, size, nmemb, (FILE *) stream);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Download data from url to file.
 */
void
urldata_get_to_file (const char *url,
                     const char *fn,
                     char      **err)
{
    CURL     *curl;
    FILE     *file;
    CURLcode  res;
    char      errbuf[CURL_ERROR_SIZE];
    struct curl_slist *headers = nullptr;

    if (access (fn, F_OK) == 0) {
        return;
    }
 
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();

    if (curl) {
        headers = url_set_common_opts (curl, url, errbuf);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);
     
        file = fopen (fn, "wb");
        if (file) {
            curl_easy_setopt (curl, CURLOPT_WRITEDATA, file);
            res = curl_easy_perform (curl);
            fclose (file);
    
            if (res != CURLE_OK) {
                *err = strdup (strlen (errbuf) ?
                                 errbuf : curl_easy_strerror (res));
                warnx ("libcurl: (%d) %s", res, *err);
                unlink (fn);
            }
        }
        curl_slist_free_all (headers);
        curl_easy_cleanup (curl);
    }
    curl_global_cleanup ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Percent-encode a string for use as a url query value.
 */
char *
urldata_escape (const char *str)
{
    CURL *curl    = nullptr; /* Handle, required by older libcurl versions */
    char *escaped = nullptr; /* String allocated by libcurl */
    char *result  = nullptr; /* Copy to return */

    curl = curl_easy_init ();
    if (curl != nullptr) {
        escaped = curl_easy_escape (curl, str, 0);
        curl_easy_cleanup (curl);
    }
    if ((result = strdup (escaped != nullptr ? escaped : "")) == nullptr)
        err (EXIT_FAILURE, nullptr);

    curl_free (escaped);

    return result;
}
/*----------------------------------------------------------------------------*/
