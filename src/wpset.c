/**
 * @file  wpset.c
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
 * @brief  Wallpaper setting functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "setts.h"
#include "randomm.h"
#include "strfun.h"
#include "setting.h"
#include "errs.h"
#include "hashfun.h"
#include "wpset.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static void wallpaper_set_file (const char *s_cmd,
 *                                      const char *s_wall)
 *
 * @brief  Set given file as a wallpaper. 
 *
 * @param[in]  s_cmd   Wallpaper set command
 * @param[in]  s_wall  Wallpaper file
 * @return     none
 *
 * @fn  static const char * wallpaper_set_random (Setting *st_settings,
 *                                                RandMem *rm_rand)
 *
 * @brief  Set random image from list as a wallpaper.
 *
 * @param[in,out] st_settings  Program settings
 * @param[in,out] rm_rand      Raddom memory structure
 * @return        Wallpaper file path
 *
 * @fn  static const char * wallpaper_set_next_in_list (Setting *st_settings)
 *
 * @brief  Set next wallpaper from list.
 *
 * @param[in,out] st_settings  Program settings
 * @return        Wallpaper file path
 */
/*----------------------------------------------------------------------------*/
static void         wallpaper_set_file         (const char *s_cmd,
                                                const char *s_wall)
                    __attribute__ ((nonnull (1, 2)));

static const char * wallpaper_set_random       (Setting    *st_settings,
                                                RandMem    *rm_rand);

static const char * wallpaper_set_next_in_list (Setting    *st_settings);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set given file as a wallpaper. 
 */
static void
wallpaper_set_file (const char *s_cmd,
                    const char *s_wall)
{
    char *s_cmdn = NULL;                      /* Wallpaper set command */
    int   i_res __attribute__ ((unused)) = 0; /* Result of system command */

    s_cmdn = str_set_up_wallpaper_command (s_cmd, s_wall, "[F]");

    if (s_cmdn == NULL)
        return;

    #ifdef DEBUG
    printf ("Setting wallpaper command %s\n", s_cmdn);
    #endif

    i_res = system (s_cmdn);

    free (s_cmdn);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set random image from list as a wallpaper.
 */
static const char *
wallpaper_set_random (Setting *st_settings,
                      RandMem *rm_rand)
{
    const char *s_file   = NULL; /* Wallpaper file name */
    Setting    *st_walls = NULL; /* Setting with wallpaper */
    Setting    *st_sett  = NULL; /* For concrete setting */
    uint32_t    i_pos    = 0;    /* Random wallpaper position */

    /* Get wallaper list setting */
    st_walls = settings_find (setting_get_child (st_settings),
                              get_setting_name (SETTING_WALL_ARRAY));
    /* Exit function if there is no wallpapers in an array */
    if ((st_walls = setting_get_child (st_walls)) == NULL)
        return NULL;

    /* Get random number */
    i_pos = randomm_get_number (rm_rand);

    /* Get the file name at the random position */
    if ((st_sett = setting_get_at_pos (st_walls, i_pos)) != NULL)
        s_file = setting_get_string (st_sett);

    if (s_file != NULL) {
        /* Save wallpaper as last used in settings */
        st_sett = setting_new_string (get_setting_name (SETTING_LAST_USED_STR),
                                       s_file);
        settings_append_or_replace (setting_get_child (st_settings), st_sett);
        /* Set wallpaper */
        st_sett = settings_find (setting_get_child (st_settings), 
                                 get_setting_name (SETTING_BG_CMD));
        if (st_sett != NULL) {
            wallpaper_set_file (setting_get_string (st_sett), s_file);
        }
    }
    return s_file;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set next wallpaper from list.
 */
static const char *
wallpaper_set_next_in_list (Setting *st_settings)
{
    Setting       *st_walls = NULL; /* Setting with wallpaper */
    Setting       *st_sett  = NULL; /* For setting operations */
    Setting       *st_item  = NULL; /* For iteration */
    const char    *s_file   = NULL; /* File path */
    uint_fast32_t  ui_hash  = 0;    /* Last used wall name hash */

    /* Get wallaper list setting */
    st_walls = settings_find (setting_get_child (st_settings),
                              get_setting_name (SETTING_WALL_ARRAY));
    /* Exit function if there is no wallpapers in an array */
    if ((st_walls = setting_get_child (st_walls)) == NULL)
        return NULL;
    /* Get last used wallpaper setting and file path */
    st_sett = settings_find (setting_get_child (st_settings),
                             get_setting_name (SETTING_LAST_USED_STR));
    if (st_sett != NULL) {
        ui_hash = hash (setting_get_string (st_sett));
    }
    st_sett = st_walls;
    /* Find file in wallpapers array */
    st_item = st_walls;
    while (st_item != NULL) {
        /*if (strcmp (setting_get_string (st_item), s_file) == 0) {*/
        if (hash (setting_get_string (st_item)) == ui_hash) {
            st_sett = st_item;
            break;
        }
        st_item = st_item->next;
    }
    /* Get next wallpaper or first if list ended */
    s_file = st_sett->next != NULL ? setting_get_string (st_sett->next) :
                                     setting_get_string (st_walls);
    /* Save wallpaper as last used in settings */
    st_sett = setting_new_string (get_setting_name (SETTING_LAST_USED_STR),
                                  s_file);
    settings_append_or_replace (setting_get_child (st_settings), st_sett);
    /* Set wallpaper */
    st_sett = settings_find (setting_get_child (st_settings), 
                             get_setting_name (SETTING_BG_CMD));
    if (st_sett != NULL) {
        wallpaper_set_file (setting_get_string (st_sett), s_file);
    }
    return s_file;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Wallpaper change during program work.
 */
int
wpset_change (Setting    *st_settings,
              RandMem    *rm_rand,
              const char *s_cfg_file)
{
    Setting    *st_sett = NULL;
    const char *s_lastu = NULL;

    st_sett = settings_find (setting_get_child (st_settings),
                             get_setting_name (SETTING_RANDOM_OPT));

    s_lastu = (st_sett != NULL && setting_get_int (st_sett)) ?
              wallpaper_set_random (st_settings, rm_rand) : 
              wallpaper_set_next_in_list (st_settings);

    if (s_lastu != NULL)
        return setts_update_last_used (s_cfg_file, s_lastu);
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting wallpaper image at program startup.
 */
int
wpset_startup_set (Setting    *st_settings,
                   RandMem    *rm_rand,
                   const char *s_cfg_file)
{
    Setting *st_sett = NULL;
    Setting *st_opt  = NULL;
    Setting *st_cmd  = NULL;

    st_opt  = settings_find (setting_get_child (st_settings),
                             get_setting_name (SETTING_LAST_USED_OPT));
    st_sett = settings_find (setting_get_child (st_settings),
                             get_setting_name (SETTING_LAST_USED_STR));
    st_cmd  = settings_find (setting_get_child (st_settings),
                             get_setting_name (SETTING_BG_CMD));

    if (st_opt != NULL && st_sett != NULL && st_cmd != NULL &&
        setting_get_int (st_opt)) {
    
        wallpaper_set_file (setting_get_string (st_cmd),
                            setting_get_string (st_sett));
        return ERR_OK;
    }
    return wpset_change (st_settings, rm_rand, s_cfg_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting wallpaper with configuration dialog.
 */
void
wallpaper_test_set (const char *s_cmd,
                    const char *s_file)
{
    wallpaper_set_file (s_cmd, s_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting wallpaper with settings dialog.
 */
int
wallpaper_dialog_set (const char *s_cmd,
                      const char *s_file,
                      const char *s_cfg_file)
{
    wallpaper_set_file (s_cmd, s_file);
    return setts_update_last_used (s_cfg_file, s_file);
}
/*----------------------------------------------------------------------------*/

