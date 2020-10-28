/**
 * @file  urldata.c
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
    UrlData *ud_data = NULL;

    if ((ud_data = malloc (sizeof (UrlData))) == NULL)
        err (EXIT_FAILURE, NULL);

    ud_data->size = 0;
    ud_data->errbuf = NULL;

    if ((ud_data->buffer = malloc (sizeof (char))) == NULL)
        err (EXIT_FAILURE, NULL);

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
    if (ud_data == NULL)
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
    UrlData *u_data  = NULL;
    char    *s_buff  = NULL;
    size_t   ui_size = 0;
 
    ui_size = size * nmemb;
    u_data  = (UrlData *) userp;
    if ((s_buff = realloc (u_data->buffer, u_data->size + ui_size + 1)) == NULL)
        err (EXIT_FAILURE, NULL);
 
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
    UrlData  *ud_data = NULL;
    CURL     *curl;
    CURLcode  res;

    ud_data = urldata_new ();
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();

    if (curl) {
        curl_easy_setopt (curl, CURLOPT_URL, s_url);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) ud_data);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_write);
#ifdef DEBUG
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
 
        res = curl_easy_perform (curl);
 
        if (res != CURLE_OK) {
            ud_data->errbuf = malloc (CURL_ERROR_SIZE * sizeof (char));
            if (ud_data->errbuf == NULL)
                err (EXIT_FAILURE, NULL);

            snprintf (ud_data->errbuf, CURL_ERROR_SIZE, "%s",
                      curl_easy_strerror (res));
            fprintf(stderr, "curl failed: %s\n",
                    ud_data->errbuf);
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
 * @brief  Donwload data from url to file.
 */
void
urldata_get_to_file (const char *s_url,
                     const char *s_fn,
                     char      **s_err)
{
    CURL     *curl;
    FILE     *f_file;
    CURLcode  res;

    if (access (s_fn, F_OK) == 0) {
        return;
    }
 
    curl_global_init (CURL_GLOBAL_ALL);
    curl= curl_easy_init ();

    curl_easy_setopt (curl, CURLOPT_URL, s_url);
 
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
#ifdef DEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
 
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_data);
 
    f_file = fopen (s_fn, "wb");
    if (f_file) {
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, f_file);
        res = curl_easy_perform (curl);
        fclose (f_file);

        if (res != CURLE_OK) {
            *s_err = malloc (CURL_ERROR_SIZE * sizeof (char));
            if (*s_err == NULL)
                err (EXIT_FAILURE, NULL);

            snprintf (*s_err, CURL_ERROR_SIZE, "%s",
                      curl_easy_strerror (res));
            fprintf (stderr, "curl failed: %s\n", curl_easy_strerror (res));
            unlink (s_fn);
        }
    }
    curl_easy_cleanup (curl);
    curl_global_cleanup ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Pexels for images.
 */
UrlData *
urldata_search_pexels (const char *s_query,
                       const char *s_api_key,
                       const int   i_page,
                       const int   i_per_page)
{
    UrlData  *ud_data = NULL;
    char     *s_api   = NULL;
    char     *s_url   = NULL;
    char      s_page[64];
    char      s_per_page[64];
    CURL     *curl;
    CURLcode  res;
    struct curl_slist *list = NULL;

    ud_data = urldata_new ();
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();

    if (curl) {
        sprintf (s_page, "%d", i_page);
        sprintf (s_per_page, "%d", i_per_page);
        s_api = str_comb ("Authorization: ", s_api_key);
        s_url = str_comb ("https://api.pexels.com/v1/search?query=", s_query);
        str_append (&s_url, "&page=");
        str_append (&s_url, s_page);
        str_append (&s_url, "&per_page=");
        str_append (&s_url, s_per_page);

        list = curl_slist_append (list, s_api);

        curl_easy_setopt (curl, CURLOPT_URL, s_url);
        curl_easy_setopt (curl, CURLOPT_HTTPHEADER, list);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) ud_data);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_write);
#ifdef DEBUG
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
 
        res = curl_easy_perform (curl);
 
        if (res != CURLE_OK) {
            ud_data->errbuf = malloc (CURL_ERROR_SIZE * sizeof (char));
            if (ud_data->errbuf == NULL)
                err (EXIT_FAILURE, NULL);

            snprintf (ud_data->errbuf, CURL_ERROR_SIZE, "%s",
                      curl_easy_strerror (res));
            fprintf(stderr, "curl failed: %s\n", ud_data->errbuf);
        }
        curl_slist_free_all (list);
 
        curl_easy_cleanup (curl);
        free (s_api);
    }
    curl_global_cleanup ();

    return ud_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search Pixbay for images.
 */
UrlData *
urldata_search_pixbay (const char *s_query,
                       const char *s_api_key,
                       const int   i_page,
                       const int   i_per_page)
{
    UrlData  *ud_data = NULL;
    char     *s_url   = NULL;
    char      s_page[64];
    char      s_per_page[64];
    CURL     *curl;
    CURLcode  res;

    ud_data = urldata_new ();
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();

    if (curl) {
        sprintf (s_page, "%d", i_page);
        sprintf (s_per_page, "%d", i_per_page);
        s_url = str_comb ("https://pixabay.com/api/?key=", s_api_key);
        str_append (&s_url, "&q=");
        str_append (&s_url, s_query);
        str_append (&s_url, "&page=");
        str_append (&s_url, s_page);
        str_append (&s_url, "&per_page=");
        str_append (&s_url, s_per_page);
        str_append (&s_url, "&image_type=photo");

        curl_easy_setopt (curl, CURLOPT_URL, s_url);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) ud_data);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_write);
#ifdef DEBUG
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
        res = curl_easy_perform (curl);
 
        if (res != CURLE_OK) {
            ud_data->errbuf = malloc (CURL_ERROR_SIZE * sizeof (char));
            if (ud_data->errbuf == NULL)
                err (EXIT_FAILURE, NULL);

            snprintf (ud_data->errbuf, CURL_ERROR_SIZE, "%s",
                      curl_easy_strerror (res));
            fprintf(stderr, "curl failed: %s\n", ud_data->errbuf);
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
                          const char *s_api_key,
                          const int   i_page,
                          const int   i_per_page)
{
    UrlData  *ud_data = NULL;
    char     *s_url   = NULL;
    char      s_page[64];
    char      s_per_page[64];
    CURL     *curl;
    CURLcode  res;
    struct curl_slist *list = NULL;

    ud_data = urldata_new ();
    curl_global_init (CURL_GLOBAL_ALL);
    curl = curl_easy_init ();
    if (curl) {
        sprintf (s_page, "%d", i_page);
        sprintf (s_per_page, "%d", i_per_page);
        s_url = str_comb ("https://wallhaven.cc/api/v1/search?apikey=",
                          s_api_key);
        str_append (&s_url, "&q=");
        str_append (&s_url, s_query);
        str_append (&s_url, "&page=");
        str_append (&s_url, s_page);
        str_append (&s_url, "&sorting=relevance");

        curl_easy_setopt (curl, CURLOPT_URL, s_url);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) ud_data);
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, url_write);
#ifdef DEBUG
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif
 
        res = curl_easy_perform (curl);
 
        if (res != CURLE_OK) {
            ud_data->errbuf = malloc (CURL_ERROR_SIZE * sizeof (char));
            if (ud_data->errbuf == NULL)
                err (EXIT_FAILURE, NULL);

            snprintf (ud_data->errbuf, CURL_ERROR_SIZE, "%s",
                      curl_easy_strerror (res));
            fprintf(stderr, "curl failed: %s\n", ud_data->errbuf);
        }
        curl_slist_free_all (list);
 
        curl_easy_cleanup (curl);
    }
    curl_global_cleanup ();

    return ud_data;
}
/*----------------------------------------------------------------------------*/
 
