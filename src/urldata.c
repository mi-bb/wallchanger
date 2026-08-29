/**
 * @file  urldata.c
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
#include "strfun.h"
#include "urldata.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new UrlData item.
 */
UrlData *
urldata_new (void)
{
    UrlData *ud_data = nullptr;

    if ((ud_data = malloc (sizeof (UrlData))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    ud_data->size = 0;
    ud_data->errbuf = nullptr;

    if ((ud_data->buffer = malloc (sizeof (char))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    ud_data->buffer[0] = '\0';

    return ud_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free UrlData item.
 */
void
urldata_free (UrlData *ud_data)
{
    if (ud_data == nullptr)
        return;

    free (ud_data->buffer);
    free (ud_data->errbuf);
    free (ud_data);
}
/*----------------------------------------------------------------------------*/
static size_t
url_write (void   *data,
           size_t  size,
           size_t  nmemb,
           void   *userp)
{
    UrlData *u_data  = nullptr;
    char    *s_buff  = nullptr;
    size_t   ui_size = 0;
 
    ui_size = size * nmemb;
    u_data  = (UrlData *) userp;
    if ((s_buff = realloc (u_data->buffer, u_data->size + ui_size + 1)) == nullptr)
        err (EXIT_FAILURE, nullptr);
 
    u_data->buffer = s_buff;
    memcpy (& (u_data->buffer[u_data->size]), data, ui_size);
    u_data->size += ui_size;
    u_data->buffer[u_data->size] = '\0';

    return ui_size;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get data from given url.
 */
UrlData *
urldata_get_data (const char *s_url)
{
    UrlData  *ud_data = nullptr;
    CURL     *curl;
    CURLcode  res;
    char      errbuf[CURL_ERROR_SIZE];

    ud_data = urldata_new ();
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();

    if (curl) {
        curl_easy_setopt (curl, CURLOPT_URL, s_url);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) ud_data);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_write);
        curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, errbuf);
        errbuf[0] = '\0';
#ifdef DEBUG
        curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
#endif
        res = curl_easy_perform (curl);
 
        if (res != CURLE_OK) {
            ud_data->errbuf = strdup (strlen (errbuf) ?
                                      errbuf : curl_easy_strerror (res));
            warnx ("libcurl: (%d) %s", res, ud_data->errbuf);
        }
        curl_easy_cleanup (curl);
    }
    curl_global_cleanup ();

    return ud_data;
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
urldata_get_to_file (const char *s_url,
                     const char *s_fn,
                     char      **s_err)
{
    CURL     *curl;
    FILE     *f_file;
    CURLcode  res;
    char      errbuf[CURL_ERROR_SIZE];

    if (access (s_fn, F_OK) == 0) {
        return;
    }
 
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();

    if (curl) {
        curl_easy_setopt (curl, CURLOPT_URL, s_url);
        curl_easy_setopt (curl, CURLOPT_FAILONERROR, 1L);
#ifdef DEBUG
        curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
#endif
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, errbuf);
        errbuf[0] = '\0';
     
        f_file = fopen (s_fn, "wb");
        if (f_file) {
            curl_easy_setopt (curl, CURLOPT_WRITEDATA, f_file);
            res = curl_easy_perform (curl);
            fclose (f_file);
    
            if (res != CURLE_OK) {
                *s_err = strdup (strlen (errbuf) ?
                                 errbuf : curl_easy_strerror (res));
                warnx ("libcurl: (%d) %s", res, *s_err);
                unlink (s_fn);
            }
        }
        curl_easy_cleanup (curl);
    }
    curl_global_cleanup ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Pixabay for images.
 */
UrlData *
urldata_search_pixabay (const char *s_query,
                        const char *s_search_opts,
                        const char *s_api_key,
                        const int   i_page)
{
    UrlData  *ud_data = nullptr;
    char     *s_url   = nullptr;
    char      s_page[64];
    CURL     *curl;
    CURLcode  res;
    char      errbuf[CURL_ERROR_SIZE];

    ud_data = urldata_new ();
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();

    if (curl) {
        sprintf (s_page,     "%d", i_page);
        s_url = str_comb ("https://pixabay.com/api/?key=", s_api_key);
        str_append (&s_url, "&q=");
        str_append (&s_url, s_query);
        str_append (&s_url, "&page=");
        str_append (&s_url, s_page);
        str_append (&s_url, "&image_type=photo");
        str_append (&s_url, str_is_empty (s_search_opts) ?
                            "&per_page=12" : s_search_opts);

        curl_easy_setopt (curl, CURLOPT_URL, s_url);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) ud_data);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_write);
        curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, errbuf);
        errbuf[0] = '\0';
#ifdef DEBUG
        curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
#endif
        res = curl_easy_perform (curl);
 
        if (res != CURLE_OK) {
            ud_data->errbuf = strdup (strlen (errbuf) ?
                              errbuf : curl_easy_strerror (res));
            warnx ("libcurl: (%d) %s", res, ud_data->errbuf);
        }
 
        curl_easy_cleanup (curl);
    }
    curl_global_cleanup ();

    return ud_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Wallhaven for images.
 */
UrlData *
urldata_search_wallhaven (const char *s_query,
                          const char *s_search_opts,
                          const char *s_api_key,
                          const int   i_page)
{
    UrlData  *ud_data = nullptr;
    char     *s_url   = nullptr;
    char      s_page[64];
    CURL     *curl;
    CURLcode  res;
    char      errbuf[CURL_ERROR_SIZE];

    ud_data = urldata_new ();
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();
    if (curl) {
        sprintf (s_page, "%d", i_page);
        s_url = str_comb ("https://wallhaven.cc/api/v1/search?apikey=",
                          s_api_key);
        str_append (&s_url, "&q=");
        str_append (&s_url, s_query);
        str_append (&s_url, "&page=");
        str_append (&s_url, s_page);
        str_append (&s_url, str_is_empty (s_search_opts) ?
                            "&sorting=relevance" : s_search_opts);

        curl_easy_setopt (curl, CURLOPT_URL, s_url);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) ud_data);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_write);
        curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, errbuf);
        errbuf[0] = '\0';
#ifdef DEBUG
        curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
#endif
 
        res = curl_easy_perform (curl);
 
        if (res != CURLE_OK) {
            ud_data->errbuf = strdup (strlen (errbuf) ?
                              errbuf : curl_easy_strerror (res));
            warnx ("libcurl: (%d) %s", res, ud_data->errbuf);
        }
 
        curl_easy_cleanup (curl);
    }
    curl_global_cleanup ();

    return ud_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Wallpaper Abyss for images.
 */
UrlData *
urldata_search_wallabyss (const char *s_query,
                          const char *s_search_opts,
                          const char *s_api_key,
                          const int   i_page)
{
    UrlData  *ud_data = nullptr;
    char     *s_url   = nullptr;
    char      s_page[64];
    CURL     *curl;
    CURLcode  res;
    char      errbuf[CURL_ERROR_SIZE];

    ud_data = urldata_new ();
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();
    if (curl) {
        sprintf (s_page, "%d", i_page);
        s_url = str_comb ("https://wall.alphacoders.com/api2.0/get.php?auth=",
                          s_api_key);
        str_append (&s_url, "&method=search&term=");
        str_append (&s_url, s_query);
        str_append (&s_url, "&info_level=2&page=");
        str_append (&s_url, s_page);
        if (!str_is_empty (s_search_opts))
            str_append (&s_url, s_search_opts);

        curl_easy_setopt (curl, CURLOPT_URL, s_url);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) ud_data);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_write);
        curl_easy_setopt (curl, CURLOPT_ERRORBUFFER, errbuf);
        errbuf[0] = '\0';
#ifdef DEBUG
        curl_easy_setopt (curl, CURLOPT_VERBOSE, 1L);
#endif
        res = curl_easy_perform (curl);
 
        if (res != CURLE_OK) {
            ud_data->errbuf = strdup (strlen (errbuf) ?
                              errbuf : curl_easy_strerror (res));
            warnx ("libcurl: (%d) %s", res, ud_data->errbuf);
        }
        curl_easy_cleanup (curl);
    }
    curl_global_cleanup ();

    return ud_data;
}
/*----------------------------------------------------------------------------*/
 
 
