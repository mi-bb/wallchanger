/**
 * @file  window_manager_info.c
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
 * @brief  Window manager info getting functions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process/process.h"
#include "cfg_file.h"
#include "settings.h"
#include "string_functions.h"
#include "defs.h"
#include "errors.h"
#include "window_manager_info.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update window manager info file with new settings.
 *
 * @param[in]  wms  List of Setting items
 * @return     Updating result
 */
static int
wms_update_wm_config (const Setting *wms)
{
    char *path = nullptr;  /* Config file path */
    int   res  = ERR_OK;   /* Error output */

    path = cfg_file_get_wm_info_home_file_path ();
    res  = setts_check_update_file (path, wms);
    free (path);

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of Xfce displays possible to set wallpaper.
 */
char **
wms_get_xfce_display_list (void)
{
    char   **ret   = nullptr; /* Null terminated list of displays to return */
    char   **tmp   = nullptr; /* Temp for realloc */
    char    *nl    = nullptr; /* Newline pos char */
    size_t   alloc = 0;       /* Number of strings to alloc */
    size_t   i     = 0;       /* i */
    char     buff[1024];      /* Buffer for fgets */
    FILE    *file;            /* File */

    if ((ret = malloc ((alloc + 1) * sizeof (char*))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    /* Get output for xfce backdrop query */
    file = popen ("xfconf-query -c xfce4-desktop -p /backdrop -l", "r");

    if (file == nullptr) {
        warnx ("Checking displays failed");
    }
    else {
        while (fgets (buff, sizeof (buff), file) != nullptr) {
            if (strstr (buff, "last-image") != nullptr) {
                ++alloc;
                if ((nl = strchr (buff, '\n')) != nullptr) {
                    *nl = '\0';
                }
                tmp = realloc (ret, (alloc + 1) * sizeof (char*));
                if (tmp == nullptr) {
                    for (i = 0; i < alloc-1; ++i)
                        free (ret[i]);
                    free (ret);
                    err (EXIT_FAILURE, nullptr);
                }
                ret = tmp;
                ret[alloc-1] = strdup (buff);
            }
        }
        pclose (file);
    }
    ret[alloc] = nullptr;

    return ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free null terminated list of strings.
 */
void
wms_free_xfce_display_list (char **list)
{
    char **it = nullptr;

    for (it = list; *it != nullptr; ++it)
        free (*it);
    free (list);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create Xfce wallpaper set command with display name given in disp.
 */
char *
wms_get_xfce_command (const char *disp)
{
    char *res = nullptr;

    res = str_comb ("xfconf-query --channel xfce4-desktop --property ",
                    disp);
    str_append (&res, " --set \"[F]\"");

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from user's local config file.
 */
Setting *
wms_get_wm_info_home (int *err)
{
    Setting *wms  = nullptr; /* Setting list to return */
    char    *path = nullptr; /* Config file path */

    *err = 0;

    path = cfg_file_get_wm_info_home_file_path ();
    wms  = setts_read (path, err);

    free (path);

    return wms;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from default config file.
 */
Setting *
wms_get_wm_info_data (int *err)
{
    Setting *wms  = nullptr; /* Setting list to return */
    char    *path = nullptr; /* Config file path */

    *err = 0;

    if ((path = cfg_file_get_wm_info_data_file_path (err)) != nullptr) {
        wms = setts_read (path, err);
        free (path);
    }
    return wms;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from user's config file and if it is not
 *         present, get data from default config file.
 */
Setting *
wms_get_wm_info (int *err)
{
    Setting *settings = nullptr; /* Setting list to return */

    *err = 0;

    /* Load user's wm info data */
    settings = wms_get_wm_info_home (err);

    if (*err == ERR_OK) {
        return settings;
    }
    settings_free_all (settings);

    warnx ("Loading default window manager info.");

    /* Load app default wm info data */
    settings = wms_get_wm_info_data (err);

    return settings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find window manager that is currently in use.
 */
const Setting *
wms_get_current_wm (const Setting *wmlist)
{
    const char    *name       = nullptr; /* Window manager name string */
    const char    *proc_name  = nullptr; /* Window manager process name string */
    const Setting *proc_item  = nullptr; /* Setting with process name info */
    const Setting *wm         = nullptr; /* Setting with window manager info */
    const Setting *unknown_wm = nullptr; /* Setting for unknown window manager */

#ifdef DEBUG
    printf ("Finding wm\n");
#endif
    wm = wmlist;

    while (wm != nullptr) {
        name = setting_get_name (wm);
#ifdef DEBUG
        printf ("%s\n", name);
#endif
        if (strcmp (name, "Unknown") == 0) {
            unknown_wm = wm;
            wm = wm->next;
            continue;
        }
        proc_item = setting_get_child (setting_find_child (wm, "Proc"));
        while (proc_item != nullptr) {
            proc_name = setting_get_string (proc_item);
#ifdef DEBUG
            printf (" %s", proc_name);
#endif
            if (process_exists_b (proc_name)) {
#ifdef DEBUG
                printf (" YES!\n");
#endif
                return wm;
            }
            proc_item = proc_item->next;
        }
#ifdef DEBUG
        printf (" nope\n");
#endif
        wm = wm->next;
    }
    return unknown_wm;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update wallpaper set command in window manager info config file.
 */
int
wms_update_wm_command (const char *wm_name,
                       const char *command)
{
    Setting *wms  = nullptr;  /* Window manager info list */
    Setting *item = nullptr;  /* For checking wm name and command */
    char    *path = nullptr;  /* Config file path */
    int      err  = ERR_OK;   /* Error output */

    /* Load settings from config file */
    path = cfg_file_get_wm_info_home_file_path ();
    wms  = setts_read (path, &err);
    if (err != ERR_OK) {
        /* err (EXIT_FAILURE, NULL); */
        free (path);
        settings_free_all (wms);
        return err;
    }
#ifdef DEBUG
    printf ("WM : %s\nCM : %s\n", wm_name, command);
#endif
    if ((item = settings_find (wms, wm_name)) != nullptr) {
        if ((item = setting_find_child (item, "Command")) != nullptr) {
            setting_set_string (item, command);
        }
    }
    err = setts_check_update_file (path, setting_get_child (wms));
    free (path);
#ifdef DEBUG
    settings_print (wms);
#endif
    settings_free_all (wms);

    return err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare previously used window manager with present one, set
 *         wallpaper change command.
 */
char *
wms_get_wallpaper_command (const char *cfg_file,
                           Setting    *setts,
                           Setting    *wmlist,
                           int        *err)
{
    const Setting *current_wm      = nullptr; /* Current used wm setting */
    const char    *cmd             = nullptr; /* Command from settings */
    const char    *last_wm_name    = nullptr; /* Last used wm string */
    const char    *current_wm_name = nullptr; /* Current used wm string */

    *err = ERR_OK;

    /* Getting last used window manager name string */
    last_wm_name = setting_get_string (
            settings_find (setts, get_setting_name (SETT_LAST_USED_WM)));
    /* Getting currently used window manager setting and name string */
    current_wm = wms_get_current_wm (wmlist);
    current_wm_name = setting_get_name (current_wm);
#ifdef DEBUG
    printf ("WM last used: %s, current: %s\n", last_wm_name, current_wm_name);
#endif

#ifdef DEBUG
    puts (" Checking wm info");
#endif
    if (current_wm_name == nullptr && last_wm_name == nullptr) {
#ifdef DEBUG
        puts ("  All nulls, settinf default");
#endif
        return strdup (DEFAULT_BG_CMD);
    }
    else if (last_wm_name == nullptr || current_wm_name == nullptr ||
        strcmp (current_wm_name, last_wm_name) != 0) {
#ifdef DEBUG
        puts ("  Current wm differs last used");
#endif
        cmd = setting_get_string (
                setting_find_child (current_wm, "Command"));
        *err = setts_update_last_used_wm (cfg_file, current_wm_name);
    }
    else {
#ifdef DEBUG
        puts ("  Current wm equals last used");
#endif
        cmd = setting_get_string (
                settings_find (setts, get_setting_name (SETT_BG_CMD)));
    }
    return strdup (cmd != nullptr ? cmd : DEFAULT_BG_CMD);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check setting with wm info, remove from defaults all wms present
 *         in home list.
 */
int
wms_check_for_new_wms (void)
{
    Setting *home      = nullptr;  /* For user's wm info */
    Setting *next_item = nullptr;  /* Next setting in list */
    Setting *defaults  = nullptr;  /* For app default wm info */
    Setting *home_item = nullptr;  /* Temp for saved data item */
    Setting *def_item  = nullptr;  /* Temp for default data item */
    int      err       = ERR_OK;   /* Error output */

    /* Load user's wm info data */
    home = wms_get_wm_info_home (&err);
    /* Load app default wm info data */
    defaults = wms_get_wm_info_data (&err);
    if (err != ERR_OK) {
        settings_free_all (defaults);
        settings_free_all (home);
        return err;
    }
    /* Iterate and remove from app default data wms present in user's info */
    home_item = home;

    while (home_item != nullptr) {

        def_item = defaults;

        while (def_item != nullptr) {

            next_item = def_item->next;

            if (settings_equal_names (home_item, def_item)) {
#ifdef DEBUG
                printf ("Removing %s\n", def_item->s_name);
#endif
                defaults = setting_remove (def_item);
                break;
            }
            def_item = next_item;
        }
        home_item = home_item->next;
    }
    /* If there are some window managers left in app default wm data,
     * update user's wm data with this window managers */
    if (setting_count_children (defaults) > 0) {
        err = wms_update_wm_config (defaults);
    }
    settings_free_all (defaults);
    settings_free_all (home);

    return err;
}
/*----------------------------------------------------------------------------*/
