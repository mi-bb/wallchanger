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
#include "url_data.h"
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
 * @brief  Get data from given url.
 */
UrlData *
urldata_get_data (const char *url)
{
    UrlData  *url_data = nullptr;
    CURL     *curl;
    CURLcode  res;
    char      errbuf[CURL_ERROR_SIZE];

    url_data = urldata_new ();
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();

    if (curl) {
        curl_easy_setopt (curl, CURLOPT_URL, url);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) url_data);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_write);
        curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, errbuf);
        errbuf[0] = '\0';
#ifdef DEBUG
        curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
#endif
        res = curl_easy_perform (curl);
 
        if (res != CURLE_OK) {
            url_data->errbuf = strdup (strlen (errbuf) ?
                                      errbuf : curl_easy_strerror (res));
            warnx ("libcurl: (%d) %s", res, url_data->errbuf);
        }
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

    if (access (fn, F_OK) == 0) {
        return;
    }
 
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();

    if (curl) {
        curl_easy_setopt (curl, CURLOPT_URL, url);
        curl_easy_setopt (curl, CURLOPT_FAILONERROR, 1L);
#ifdef DEBUG
        curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
#endif
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, errbuf);
        errbuf[0] = '\0';
     
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
        curl_easy_cleanup (curl);
    }
    curl_global_cleanup ();
}
/*----------------------------------------------------------------------------*/
 
 
