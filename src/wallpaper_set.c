/**
 * @file  wallpaper_set.c
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
 * @brief  Wallpaper setting functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "settings.h"
#include "randomm.h"
#include "string_functions.h"
#include "setting_item.h"
#include "errors.h"
#include "hash_djb2.h"
#include "wallpaper_set.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static void wallpaper_set_file (const char *cmd,
 *                                      const char *wall)
 *
 * @brief  Set given file as a wallpaper.
 *
 * @param[in]  cmd   Wallpaper set command
 * @param[in]  wall  Wallpaper file
 * @return     none
 *
 * @fn  static const char * wallpaper_set_random (Setting *settings,
 *                                                RandMem *rand)
 *
 * @brief  Set random image from list as a wallpaper.
 *
 * @param[in,out] settings  Program settings
 * @param[in,out] rand      Raddom memory structure
 * @return        Wallpaper file path
 *
 * @fn  static const char * wallpaper_set_next_in_list (Setting *settings)
 *
 * @brief  Set next wallpaper from list.
 *
 * @param[in,out] settings  Program settings
 * @return        Wallpaper file path
 */
/*----------------------------------------------------------------------------*/
static void         wallpaper_set_file         (const char *cmd,
                                                const char *wall)
                    __attribute__ ((nonnull (1, 2)));

static const char * wallpaper_set_random       (Setting    *settings,
                                                RandMem    *rand);

static const char * wallpaper_set_next_in_list (Setting    *settings);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set given file as a wallpaper. 
 */
static void
wallpaper_set_file (const char *cmd,
                    const char *wall)
{
    char *command = nullptr;             /* Wallpaper set command */
    [[maybe_unused]] int res = 0;        /* Result of system command */

    command = str_set_up_wallpaper_command (cmd, wall, "[F]");

    if (command == nullptr)
        return;

    #ifdef DEBUG
    printf ("Setting wallpaper command %s\n", command);
    #endif

    res = system (command);

    free (command);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random image from list as a wallpaper.
 */
static const char *
wallpaper_set_random (Setting *settings,
                      RandMem *rand)
{
    const char *file   = nullptr; /* Wallpaper file name */
    Setting    *walls  = nullptr; /* Setting with wallpaper */
    Setting    *sett   = nullptr; /* For concrete setting */
    size_t      pos    = 0;       /* Random wallpaper position */

    /* Get wallaper list setting */
    walls = settings_find (settings, get_setting_name (SETT_WALL_ARRAY));
    /* Exit function if there is no wallpapers in an array */
    if ((walls = setting_get_child (walls)) == nullptr)
        return nullptr;

    /* Get random number */
    pos = randomm_get_number (rand);

    /* Get the file name at the random position */
    if ((sett = setting_get_at_pos (walls, pos)) != nullptr)
        file = setting_get_string (sett);

    if (file != nullptr) {
        /* Save wallpaper as last used in settings */
        sett = setting_new_string (get_setting_name (SETT_LAST_USED_STR),
                                   file);
        //settings_append_or_replace (setting_get_child (settings), sett);
        settings = settings_append_or_replace (settings, sett);
        /* Set wallpaper */
        sett = settings_find (settings, get_setting_name (SETT_BG_CMD));
        if (sett != nullptr) {
            wallpaper_set_file (setting_get_string (sett), file);
        }
    }
    return file;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set next wallpaper from list.
 */
static const char *
wallpaper_set_next_in_list (Setting *settings)
{
    Setting       *walls     = nullptr; /* Setting with wallpaper */
    Setting       *sett      = nullptr; /* For setting operations */
    Setting       *item      = nullptr; /* For iteration */
    const char    *file      = nullptr; /* File path */
    uint_fast32_t  last_hash = 0;       /* Last used wall name hash */

    /* Get wallaper list setting */
    walls = settings_find (settings, get_setting_name (SETT_WALL_ARRAY));
    /* Exit function if there is no wallpapers in an array */
    if ((walls = setting_get_child (walls)) == nullptr)
        return nullptr;
    /* Get last used wallpaper setting and file path */
    sett = settings_find (settings, get_setting_name (SETT_LAST_USED_STR));
    if (sett != nullptr) {
        last_hash = hash (setting_get_string (sett));
    }
    sett = walls;
    /* Find file in wallpapers array */
    item = walls;
    while (item != nullptr) {
        /*if (strcmp (setting_get_string (item), file) == 0) {*/
        if (hash (setting_get_string (item)) == last_hash) {
            sett = item;
            break;
        }
        item = item->next;
    }
    /* Get next wallpaper or first if list ended */
    file = sett->next != nullptr ? setting_get_string (sett->next) :
                                   setting_get_string (walls);
    /* Save wallpaper as last used in settings */
    sett = setting_new_string (get_setting_name (SETT_LAST_USED_STR),
                               file);
    settings = settings_append_or_replace (settings, sett);
    /* Set wallpaper */
    sett = settings_find (settings, get_setting_name (SETT_BG_CMD));
    if (sett != nullptr) {
        wallpaper_set_file (setting_get_string (sett), file);
    }
    return file;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Wallpaper change during program work.
 */
int
wallpaper_set_change (Setting    *settings,
                      RandMem    *rand,
                      const char *cfg_file)
{
    Setting    *sett      = nullptr; /* For setting with random opt */
    const char *last_used = nullptr; /* For file name of last used wallpaper */

    sett = settings_find (settings, get_setting_name (SETT_RANDOM_OPT));

    last_used = (sett != nullptr && setting_get_int (sett)) ?
                wallpaper_set_random (settings, rand) :
                wallpaper_set_next_in_list (settings);

    return last_used != nullptr ? setts_update_last_used (cfg_file, last_used) :
                                  ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting wallpaper image at program startup.
 */
int
wallpaper_set_startup_set (Setting    *settings,
                           RandMem    *rand,
                           const char *cfg_file)
{
    Setting *opt       = nullptr; /* For setting with last used option */
    Setting *last_used = nullptr; /* For setting with last used wallpaper name */
    Setting *cmd       = nullptr; /* For wallpaper set command */

    opt       = settings_find (settings, get_setting_name (SETT_LAST_USED_OPT));
    last_used = settings_find (settings, get_setting_name (SETT_LAST_USED_STR));
    cmd       = settings_find (settings, get_setting_name (SETT_BG_CMD));

    if (opt != nullptr && last_used != nullptr && cmd != nullptr &&
        setting_get_int (opt)) {

        wallpaper_set_file (setting_get_string (cmd),
                            setting_get_string (last_used));
        return ERR_OK;
    }
    return wallpaper_set_change (settings, rand, cfg_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting wallpaper with configuration dialog.
 */
void
wallpaper_test_set (const char *cmd,
                    const char *file)
{
    wallpaper_set_file (cmd, file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting wallpaper with settings dialog.
 */
int
wallpaper_dialog_set (const char *cmd,
                      const char *file,
                      const char *cfg_file)
{
    wallpaper_set_file (cmd, file);
    return setts_update_last_used (cfg_file, file);
}
/*----------------------------------------------------------------------------*/

