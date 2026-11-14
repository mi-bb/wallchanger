/**
 * @file  live.c
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
 * @brief  Getting wallpapers from remote live services.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <err.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "defs.h"
#include "settings/cfg_file.h"
#include "settings/json_functions.h"
#include "settings/settings.h"
#include "util/errors.h"
#include "util/file_dir_functions.h"
#include "util/file_io.h"
#include "util/string_functions.h"
#include "web/url_data.h"
#include "web/web_util.h"
#include "live_bing.h"
#include "live_commons.h"
#include "live_wallhaven.h"
#include "live.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   sources
 * @brief Remote live services in the order they are shown and used
 *
 * @var   SOURCE_CNT
 * @brief Number of remote live services
 */
static const LiveSource *const sources[] = {
    &live_bing,
    &live_commons,
    &live_wallhaven,
};
constexpr size_t SOURCE_CNT = sizeof (sources) / sizeof (sources[0]);
/*----------------------------------------------------------------------------*/
/**
 * @def   STATE_LAST_SERVICE
 * @brief State file key with the name of the last used service
 *
 * @def   STATE_CHECK_TIME
 * @brief Suffix of the state file key with the time a daily service was
 *        last asked
 *
 * @def   STATE_FILE
 * @brief Suffix of the state file key with the wallpaper a daily service
 *        last gave
 *
 * @def   PART_EXT
 * @brief Extension of a file being downloaded
 *
 * @def   INFO_JSON_FLAGS
 * @brief Format of the saved image info, the same as for images downloaded
 *        with the search dialog
 */
#define STATE_LAST_SERVICE "Last service"
#define STATE_CHECK_TIME   " check time"
#define STATE_FILE         " file"
#define PART_EXT           ".part"
#ifdef JSON_C_TO_STRING_NOSLASHESCAPE
#define INFO_JSON_FLAGS (JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE)
#else
#define INFO_JSON_FLAGS JSON_C_TO_STRING_PRETTY
#endif
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the number of remote live services.
 */
size_t
live_source_count (void)
{
    return SOURCE_CNT;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get a remote live service.
 */
const LiveSource *
live_source_get (const size_t index)
{
    return index < SOURCE_CNT ? sources[index] : nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find a remote live service by its name.
 */
const LiveSource *
live_source_find (const char *name)
{
    if (name == nullptr)
        return nullptr;

    for (size_t i = 0; i < SOURCE_CNT; ++i) {
        if (strcmp (sources[i]->name, name) == 0)
            return sources[i];
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the remote live services enabled in settings.
 *
 * @param[in]  settings  Program settings
 * @param[out] enabled   Array for SOURCE_CNT services
 * @return     Number of enabled services
 */
static size_t
live_enabled_list (Setting           *settings,
                   const LiveSource **enabled)
{
    Setting *names = nullptr; /* Enabled service names */
    size_t   count = 0;

    names = setting_get_child (
            settings_find (settings, get_setting_name (SETT_LIVE_SERVICES)));

    /* Keep the order of the sources table, not the one in the settings */
    for (size_t i = 0; i < SOURCE_CNT; ++i) {
        for (Setting *item = names; item != nullptr; item = item->next) {
            const char *name = setting_get_string (item);

            if (name != nullptr && strcmp (name, sources[i]->name) == 0) {
                enabled[count++] = sources[i];
                break;
            }
        }
    }
    return count;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count the remote live services enabled in settings.
 */
size_t
live_enabled_count (Setting *settings)
{
    const LiveSource *enabled[SOURCE_CNT];

    return live_enabled_list (settings, enabled);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the path of the info file of a wallpaper.
 *
 * @param[in] file_name  Wallpaper file name without directory
 * @return    New string with path, free it with free()
 */
static char *
live_info_path (const char *file_name)
{
    char *path = cfg_file_get_image_info_path ();

    str_append (&path, PTH_SEP);
    str_append (&path, file_name);
    str_append (&path, ".json");

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add a string to a json object when it is not empty.
 *
 * @param[out] obj  Json object
 * @param[in]  key  Member name
 * @param[in]  val  Value, can be null
 * @return     none
 */
static void
json_add_string (json_object *obj,
                 const char  *key,
                 const char  *val)
{
    if (!str_is_empty (val))
        json_object_object_add (obj, key, json_object_new_string (val));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save the source, author and license of a downloaded wallpaper.
 *
 * Uses the same "<image file name>.json" format as images downloaded with
 * the search dialog.
 *
 * @param[in] source  Service that gave the image
 * @param[in] image   Image data
 * @return    none
 */
static void
live_info_write (const LiveSource *source,
                 const LiveImage  *image)
{
    json_object *obj  = nullptr;
    char        *dir  = cfg_file_get_image_info_path ();
    char        *path = nullptr;

    if (dir_create_with_subdirs (dir) == ERR_OK) {
        obj = json_object_new_object ();
        json_add_string (obj, "service",     source->name);
        json_add_string (obj, "title",       image->title);
        json_add_string (obj, "author",      image->author);
        json_add_string (obj, "license",     image->license);
        json_add_string (obj, "license_url", image->license_url);
        json_add_string (obj, "page_url",    image->page_url);
        json_add_string (obj, "image_url",   image->image_url);

        path = live_info_path (image->file_name);
        save_file_data (path, json_object_to_json_string_ext (
                    obj, INFO_JSON_FLAGS));
        json_object_put (obj);
        free (path);
    }
    free (dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read the saved title, author and license of a wallpaper.
 */
bool
live_info_read (const char *wall_file,
                LiveImage  *image)
{
    const char  *name = nullptr; /* File name without directory */
    char        *path = nullptr; /* Info file path */
    char        *buff = nullptr; /* Info file content */
    json_object *obj  = nullptr;
    int          err  = ERR_OK;

    *image = (LiveImage) {};

    if (wall_file == nullptr)
        return false;

    name = strrchr (wall_file, '/');
    name = name != nullptr ? name + 1 : wall_file;
    path = live_info_path (name);

    if (access (path, R_OK) == 0)
        buff = read_file_data (path, &err, nullptr);

    if (buff != nullptr && err == ERR_OK)
        obj = web_json_parse (buff);

    if (obj != nullptr) {
        live_image_set (&image->title,       web_json_str (obj, "title"));
        live_image_set (&image->author,      web_json_str (obj, "author"));
        live_image_set (&image->license,     web_json_str (obj, "license"));
        live_image_set (&image->license_url, web_json_str (obj,
                                                           "license_url"));
        live_image_set (&image->page_url,    web_json_str (obj, "page_url"));
        live_image_set (&image->image_url,   web_json_str (obj, "image_url"));
        live_image_set (&image->file_name,   name);
        json_object_put (obj);
    }
    free (buff);
    free (path);

    return obj != nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Ask a service for a wallpaper and download it to the cache.
 *
 * An image already in the cache is not downloaded again. The image is
 * downloaded to a ".part" file renamed when the download is complete, so an
 * interrupted download never leaves a broken image in the cache.
 *
 * @param[in] source  Service to ask
 * @param[in] params  Wallpaper parameters
 * @param[in] dir     Cache directory
 * @return    Path of the wallpaper or null, free it with free()
 */
static char *
live_download (const LiveSource *source,
               const LiveParams *params,
               const char       *dir)
{
    LiveImage    image   = {};
    UrlData     *data    = nullptr; /* Service response */
    char        *url     = nullptr; /* Request url */
    char        *path    = nullptr; /* Wallpaper path to return */
    char        *part    = nullptr; /* Path of the file being downloaded */
    char        *err_msg = nullptr; /* Download error */
    struct stat  st;

    url  = source->request_url (params);
    data = urldata_get_data (url);
    free (url);

    if (data->errbuf != nullptr || !urldata_full (data) ||
        !source->parse (data->buffer, params, &image) ||
        image.file_name == nullptr || image.image_url == nullptr) {

        warnx ("%s: no wallpaper received", source->label);
        urldata_free (data);
        live_image_clear (&image);
        return nullptr;
    }
    urldata_free (data);

    path = str_comb (dir, PTH_SEP);
    str_append (&path, image.file_name);

    if (access (path, F_OK) == 0) {
        /* Already in the cache, mark it as recently used */
        utimensat (AT_FDCWD, path, nullptr, 0);
    }
    else {
        part = str_comb (path, PART_EXT);
        unlink (part);
        urldata_get_to_file (image.image_url, part, &err_msg);

        if (err_msg == nullptr && stat (part, &st) == 0 && st.st_size > 0 &&
            rename (part, path) == 0) {
            live_info_write (source, &image);
        }
        else {
            warnx ("%s: could not download %s", source->label,
                   image.image_url);
            unlink (part);
            free (path);
            path = nullptr;
        }
        free (err_msg);
        free (part);
    }
    live_image_clear (&image);

    return path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get a wallpaper from a service.
 *
 * A daily service asked less than LIVE_DAILY_CHECK_SEC ago gives the file it
 * gave then, without any request.
 *
 * @param[in]     source   Service to ask
 * @param[in]     params   Wallpaper parameters
 * @param[in]     dir      Cache directory
 * @param[in]     state    Saved state
 * @param[in,out] changes  State changes to save
 * @return        Path of the wallpaper or null, free it with free()
 */
static char *
live_fetch (const LiveSource *source,
            const LiveParams *params,
            const char       *dir,
            Setting          *state,
            Setting         **changes)
{
    char       *key_time  = str_comb (source->name, STATE_CHECK_TIME);
    char       *key_file  = str_comb (source->name, STATE_FILE);
    const char *last_file = nullptr; /* Wallpaper given the last time */
    int64_t     last_time = 0;       /* Time the service was last asked */
    int64_t     now       = (int64_t) time (nullptr);
    char       *result    = nullptr;

    last_file = setting_get_string (settings_find (state, key_file));
    last_time = setting_get_int (settings_find (state, key_time));

    if (source->daily && last_file != nullptr &&
        now >= last_time && now - last_time < LIVE_DAILY_CHECK_SEC &&
        access (last_file, R_OK) == 0) {

        result = str_comb (last_file, "");
        utimensat (AT_FDCWD, result, nullptr, 0);
    }
    else {
        result = live_download (source, params, dir);

        if (result != nullptr && source->daily) {
            *changes = settings_append (*changes,
                                        setting_new_int (key_time, now));
            *changes = settings_append (*changes,
                                        setting_new_string (key_file, result));
        }
    }
    free (key_time);
    free (key_file);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Cached file name and modification time.
 */
typedef struct CacheFile {
    char   *path;
    time_t  mtime;
} CacheFile;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare cache files, newer first.
 */
static int
cache_file_cmp (const void *a,
                const void *b)
{
    const CacheFile *first  = a;
    const CacheFile *second = b;

    return (first->mtime < second->mtime) - (first->mtime > second->mtime);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  List the wallpapers in the cache directory, newest first.
 *
 * Files being downloaded are skipped.
 *
 * @param[in]  dir    Cache directory
 * @param[out] count  Number of files
 * @return     Array of files or null, free it with cache_files_free()
 */
static CacheFile *
cache_files_list (const char *dir,
                  size_t     *count)
{
    DIR           *handle = opendir (dir);
    struct dirent *entry  = nullptr;
    struct stat    st;
    CacheFile     *files  = nullptr;
    CacheFile     *resized = nullptr;
    size_t         alloc  = 0;
    size_t         len    = 0;
    char          *path   = nullptr;

    *count = 0;

    if (handle == nullptr)
        return nullptr;

    while ((entry = readdir (handle)) != nullptr) {
        len = strlen (entry->d_name);

        if (entry->d_name[0] == '.' ||
            (len > strlen (PART_EXT) &&
             strcmp (entry->d_name + len - strlen (PART_EXT), PART_EXT) == 0)) {
            continue;
        }
        path = str_comb (dir, PTH_SEP);
        str_append (&path, entry->d_name);

        if (stat (path, &st) != 0 || !S_ISREG (st.st_mode)) {
            free (path);
            continue;
        }
        if (*count == alloc) {
            alloc = alloc ? alloc * 2 : 64;
            if ((resized = realloc (files, alloc * sizeof (*files))) == nullptr)
                err (EXIT_FAILURE, nullptr);
            files = resized;
        }
        files[*count].path  = path;
        files[*count].mtime = st.st_mtime;
        ++*count;
    }
    closedir (handle);

    if (files != nullptr)
        qsort (files, *count, sizeof (*files), cache_file_cmp);

    return files;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free a list of cache files.
 */
static void
cache_files_free (CacheFile    *files,
                  const size_t  count)
{
    for (size_t i = 0; i < count; ++i)
        free (files[i].path);
    free (files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete the oldest wallpapers when there are more than
 *         LIVE_CACHE_MAX in the cache.
 *
 * Info files are deleted with their wallpapers, except when an image of the
 * same name downloaded with the search dialog still uses the info.
 *
 * @param[in] dir   Cache directory
 * @param[in] keep  Wallpaper never to delete
 * @return    none
 */
static void
live_prune (const char *dir,
            const char *keep)
{
    CacheFile  *files     = nullptr;
    size_t      count     = 0;
    char       *data_dir  = cfg_file_get_app_wallpapers_path ();
    char       *path      = nullptr;
    const char *name      = nullptr;

    files = cache_files_list (dir, &count);

    for (size_t i = LIVE_CACHE_MAX; i < count; ++i) {
        if (strcmp (files[i].path, keep) == 0 || unlink (files[i].path) != 0)
            continue;

        name = strrchr (files[i].path, '/') + 1;
        path = str_comb (data_dir, PTH_SEP);
        str_append (&path, name);

        if (access (path, F_OK) != 0) {
            free (path);
            path = live_info_path (name);
            unlink (path);
        }
        free (path);
    }
    cache_files_free (files, count);
    free (data_dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the most recently downloaded remote live wallpaper.
 */
char *
live_newest_cached (void)
{
    char      *dir    = cfg_file_get_live_wallpapers_path ();
    CacheFile *files  = nullptr;
    size_t     count  = 0;
    char      *result = nullptr;

    files = cache_files_list (dir, &count);

    if (count > 0)
        result = str_comb (files[0].path, "");

    cache_files_free (files, count);
    free (dir);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get an integer setting, or a default when it is not positive.
 */
static int
live_setting_int (Setting   *settings,
                  const int  id,
                  const int  def)
{
    int64_t val = setting_get_int (settings_find (settings,
                                                  get_setting_name (id)));

    return (val > 0 && val < 100000) ? (int) val : def;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get a wallpaper from the enabled remote live services.
 */
int
live_wallpaper_get (Setting  *settings,
                    RandMem  *rand_mem,
                    char    **file)
{
    const LiveSource *enabled[SOURCE_CNT];
    const LiveSource *source  = nullptr;
    LiveParams        params  = {};
    Setting          *state   = nullptr; /* Saved state */
    Setting          *changes = nullptr; /* State changes to save */
    char             *dir     = nullptr; /* Cache directory */
    char             *path    = nullptr; /* State file path */
    const char       *last    = nullptr; /* Last used service name */
    long              last_id = -1;      /* Last used service position */
    size_t            count   = 0;       /* Enabled services */
    size_t            first   = 0;       /* Position of first service to ask */
    bool              random  = false;
    int               err_code = ERR_OK;
    char              date[16];
    struct tm         tm_now;
    time_t            now     = time (nullptr);

    *file = nullptr;

    if ((count = live_enabled_list (settings, enabled)) == 0)
        return ERR_LIVE_NOSRC;

    dir = cfg_file_get_live_wallpapers_path ();
    if ((err_code = dir_create_with_subdirs (dir)) != ERR_OK) {
        free (dir);
        return err_code;
    }
    /* Start with an empty state, reading a missing file prints a warning */
    path = cfg_file_get_live_state_path ();
    if (access (path, F_OK) != 0)
        save_file_data (path, "{}");

    state = json_settings_read (path, &err_code);

    last = setting_get_string (settings_find (state, STATE_LAST_SERVICE));
    for (size_t i = 0; last != nullptr && i < count; ++i) {
        if (strcmp (enabled[i]->name, last) == 0)
            last_id = (long) i;
    }
    random = setting_get_int (settings_find (
                settings, get_setting_name (SETT_RANDOM_OPT))) != 0;

    if (random && rand_mem->range != count)
        randomm_set_range (rand_mem, count);

    first = live_pick_first (count, last_id, random, rand_mem);

    gmtime_r (&now, &tm_now);
    strftime (date, sizeof (date), "%Y-%m-%d", &tm_now);

    params.date          = date;
    params.screen_width  = live_setting_int (settings, SETT_SCREEN_WIDTH,
                                             DEFAULT_SCREEN_WIDTH);
    params.screen_height = live_setting_int (settings, SETT_SCREEN_HEIGHT,
                                             DEFAULT_SCREEN_HEIGHT);

    /* Ask the chosen service, then the others when it fails */
    for (size_t i = 0; i < count && *file == nullptr; ++i) {
        source = enabled[(first + i) % count];
        params.keywords = source->keywords_setting == nullptr ? nullptr :
                setting_get_string (settings_find (settings,
                                                   source->keywords_setting));

        if ((*file = live_fetch (source, &params, dir, state,
                                 &changes)) != nullptr) {
            changes = settings_append (changes,
                    setting_new_string (STATE_LAST_SERVICE, source->name));
        }
    }
    if (changes != nullptr)
        json_settings_check_update_file (changes, path);

    if (*file != nullptr)
        live_prune (dir, *file);

    settings_free_all (changes);
    settings_free_all (state);
    free (path);
    free (dir);

    return *file != nullptr ? ERR_OK : ERR_LIVE_NET;
}
/*----------------------------------------------------------------------------*/
