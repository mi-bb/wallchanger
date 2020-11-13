/**
 * @file  wmsfn.c
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
 * @brief  Wallpaper set command functions.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "procfn.h"
#include "cfgfile.h"
#include "setts.h"
#include "strfun.h"
#include "fdfn.h"
#include "defs.h"
#include "errs.h"
#include "wmsfn.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update window manager info file with new settings.
 *
 * @param[in]  st_wms  List of Setting items
 * @return     Updating result
 */
static int
wms_update_wm_config (Setting *st_wms)
{
    char *s_path = NULL;   /* Config file path */
    int   i_res  = ERR_OK; /* Error output */

    s_path = cfgfile_get_wm_info_home_file_path ();
    i_res  = setts_check_update_file (s_path, st_wms);
    free (s_path);
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of Xfce displays possible to set wallpaper.
 */
char **
wms_get_xfce_display_list (void)
{
    char   **s_ret    = NULL; /* Null terminated list of displays to return */
    char   **s_tmp    = NULL; /* Temp for realloc */
    char    *s_nl     = NULL; /* Newline pos char */
    size_t   ui_alloc = 0;    /* Number of strings to alloc */
    size_t   i        = 0;    /* i */
    char     s_buff[1024];    /* Buffer for fgets */
    FILE    *f_file;          /* File */

    if ((s_ret = malloc ((ui_alloc + 1) * sizeof (char*))) == NULL)
        err (EXIT_FAILURE, NULL);

    /* Get output for xfce backdrop query */
    f_file = popen ("xfconf-query -c xfce4-desktop -p /backdrop -l", "r");

    if (f_file == NULL) {
        warnx ("Checking displays failed");
    }
    else {
        while (fgets (s_buff, sizeof (s_buff), f_file) != NULL) {
            if (strstr (s_buff, "last-image") != NULL) {
                ++ui_alloc;
                if ((s_nl = strchr (s_buff, '\n')) != NULL) {
                    *s_nl = '\0';
                }
                s_tmp = realloc (s_ret, (ui_alloc + 1) * sizeof (char*));
                if (s_tmp == NULL) {
                    for (i = 0; i < ui_alloc-1; ++i)
                        free (s_ret[i]);
                    free (s_ret);
                    err (EXIT_FAILURE, NULL);
                }
                s_ret = s_tmp;
                s_ret[ui_alloc-1] = strdup (s_buff);
            }
        }
        pclose (f_file);
    }
    s_ret[ui_alloc] = NULL;
    return s_ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free null terminated list of strings.
 */
void
wms_free_xfce_display_list (char **s_list)
{
    char **s_it = NULL;

    for (s_it = s_list; *s_it != NULL; ++s_it)
        free (*s_it);
    free (s_list);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create Xfce wallpaper set command with display name given in s_disp.
 */
char *
wms_get_xfce_command (const char *s_disp)
{
    char *s_res = NULL;

    s_res = str_comb ("xfconf-query --channel xfce4-desktop --property ",
                      s_disp);
    str_append (&s_res, " --set \"[F]\"");

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from user's local config file.
 */
Setting *
wms_get_wm_info_home (int *i_err)
{
    Setting *st_wms = NULL; /* Setting list to return */
    char    *s_path = NULL; /* Config file path */

    *i_err = 0;

    s_path = cfgfile_get_wm_info_home_file_path ();
    st_wms = setts_read (s_path, i_err);

    free (s_path);

    return st_wms;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from default config file.
 */
Setting *
wms_get_wm_info_data (int *i_err)
{
    Setting *st_wms = NULL; /* Setting list to return */
    char    *s_path = NULL; /* Config file path */

    *i_err = 0;

    if ((s_path = cfgfile_get_wm_info_data_file_path (i_err)) != NULL) {
        st_wms = setts_read (s_path, i_err);
        free (s_path);
    }

    return st_wms;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get window manager info from user's config file and if it is not
 *         present, get data from default config file.
 */
Setting *
wms_get_wm_info (int *i_err)
{
    Setting *st_settings = NULL; /* Setting list to return */

    *i_err = 0;

    /* Load user's wm info data */
    st_settings = wms_get_wm_info_home (i_err);

    if (*i_err == ERR_OK) {
        return st_settings;
    }
    settings_free_all (st_settings);

    warnx ("Loading default window manager info.");

    /* Load app default wm info data */
    st_settings = wms_get_wm_info_data (i_err);

    return st_settings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find window manager that is currently in use.
 */
Setting *
wms_get_current_wm (Setting *st_wmsl)
{
    const char *s_name  = NULL; /* Window manager name string */
    const char *s_proc  = NULL; /* Window manager process name string */
    Setting    *st_proc = NULL; /* Setting with process name info */
    Setting    *st_wm   = NULL; /* Setting with window manager info */
    Setting    *st_unkn = NULL; /* Setting for unknown window manager */

#ifdef DEBUG
    printf ("Finding wm\n");
#endif
    st_wm = st_wmsl;

    while (st_wm != NULL) {
        s_name = setting_get_name (st_wm);
        if (strcmp (s_name, "Unknown") == 0) {
            st_unkn = st_wm;
            st_wm = st_wm->next;
            continue;
        }
#ifdef DEBUG
        printf ("%s", s_name);
#endif
        st_proc = setting_get_child (setting_find_child (st_wm, "Proc"));
        while (st_proc != NULL) {
            s_proc = setting_get_string (st_proc);
#ifdef DEBUG
            printf (" %s", s_proc);
#endif
            if (process_exists_b (s_proc)) {
#ifdef DEBUG
                printf (" YES!\n");
#endif
                return st_wm;
            }
            st_proc = st_proc->next;
        }
#ifdef DEBUG
        printf (" nope\n");
#endif
        st_wm = st_wm->next;
    }
    return st_unkn;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Update wallpaper set command in window manager info config file.
 */
int
wms_update_wm_command (const char *s_wm_name,
                       const char *s_command)
{
    Setting *st_wms  = NULL;   /* Window manager info list */
    Setting *st_item = NULL;   /* For checking wm name and command */
    char    *s_path  = NULL;   /* Config file path */
    int      i_err   = ERR_OK; /* Error output */

    /* Load settings from config file */
    s_path = cfgfile_get_wm_info_home_file_path ();
    st_wms = setts_read (s_path, &i_err);
    if (i_err != ERR_OK) {
        /* err (EXIT_FAILURE, NULL); */
        free (s_path);
        settings_free_all (st_wms);
        return i_err;
    }
#ifdef DEBUG
    printf ("WM : %s\nCM : %s\n", s_wm_name, s_command);
#endif
    if ((st_item = settings_find (st_wms, s_wm_name)) != NULL) {
        if ((st_item = setting_find_child (st_item, "Command")) != NULL) {
            setting_set_string (st_item, s_command);
        }
    }
    i_err = setts_check_update_file (s_path, setting_get_child (st_wms));
    free (s_path);
#ifdef DEBUG
    settings_print (st_wms);
#endif
    settings_free_all (st_wms);
    return i_err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare previously used window manager with present one, set
 *         wallpaper change command.
 */
char *
wms_get_wallpaper_command (const char *s_cfg_file,
                           Setting    *st_settings,
                           Setting    *st_wmlist,
                           int        *i_err)
{
    Setting    *st_wm        = NULL; /* Wm list setting */
    Setting    *st_cu_wm     = NULL; /* Current used wm setting */
    Setting    *st_lu_wm     = NULL; /* Last used wm setting */
    Setting    *st_item      = NULL; /* Temp setting */
    Setting    *st_bgcmd     = NULL; /* Wallpaper set command setting */
    const char *s_setts_cmd  = NULL; /* Command from settings */
    const char *s_result_cmd = NULL; /* Result command */
    const char *s_lu_wm      = NULL; /* Last used wm string */
    const char *s_cu_wm      = NULL; /* Current used wm string */
    const char *s_cu_wm_cmd  = NULL; /* Current used wm command string */

    *i_err = ERR_OK;

    enum wm_enum_state {
        WM_STATE_END = 0,                /* End of work */
        WM_STATE_SET_SAVED_BG_CMD,       /* Set saved in cfg command */
        WM_STATE_SET_CURR_USED_WM_CMD,   /* Find setting with command */
        WM_STATE_CHECK_CURR_USED_WM_CMD, /* Check current command string */
        WM_STATE_SET_MAIN_DEFAULT,       /* Set main default command */
        WM_STATE_CHECK_WM_INFO           /* Check if wm changed */
    };

    enum wm_enum_state e_state = WM_STATE_CHECK_WM_INFO;
    st_wm = st_wmlist;

    /* Getting saved wallpaper set command setting and string */
    st_bgcmd = settings_find (st_settings, get_setting_name (SETT_BG_CMD));
    if (st_bgcmd != NULL) {
        s_setts_cmd = setting_get_string (st_bgcmd);
    }
    /* Getting last used window manager setting and name string */
    st_lu_wm = settings_find (st_settings,
                              get_setting_name (SETT_LAST_USED_WM));
    if (st_lu_wm != NULL) {
        s_lu_wm = setting_get_string (st_lu_wm);
    }
    /* Getting currently used window manager setting and name string */
    if ((st_cu_wm = wms_get_current_wm (st_wm)) != NULL) {
        s_cu_wm = setting_get_name (st_cu_wm);
    }
#ifdef DEBUG
    printf ("WM last used: %s, current: %s\n", s_lu_wm, s_cu_wm);
#endif
    /* Compare last and currently used window manager, set wallpaper
     * change command for returning */
    while (e_state) {
        switch (e_state) {
            case WM_STATE_CHECK_WM_INFO:
#ifdef DEBUG
                puts (" Checking wm info");
#endif
                if (s_cu_wm == NULL && s_lu_wm == NULL) {
#ifdef DEBUG
                    puts ("  All nulls");
#endif
                    e_state = WM_STATE_SET_MAIN_DEFAULT;
                }
                else if (s_lu_wm == NULL || s_cu_wm == NULL || 
                         strcmp (s_cu_wm, s_lu_wm) != 0) {
#ifdef DEBUG
                    puts ("  Current wm differs last used");
#endif
                    *i_err = setts_update_last_used_wm (s_cfg_file, s_cu_wm);
                    e_state = WM_STATE_SET_CURR_USED_WM_CMD;
                }
                else {
#ifdef DEBUG
                    puts ("  Current wm equals last used");
#endif
                    e_state = WM_STATE_SET_SAVED_BG_CMD;
                }
                break;

            case WM_STATE_SET_CURR_USED_WM_CMD:
#ifdef DEBUG
                puts (" Setting current used wm cmd string");
#endif
                st_item = setting_find_child (st_cu_wm, "Command");
                if (st_item != NULL) {
#ifdef DEBUG
                    puts ("  Current used wm item ok");
#endif
                    s_cu_wm_cmd = setting_get_string (st_item);
                    e_state     = WM_STATE_CHECK_CURR_USED_WM_CMD;
                }
                else {
#ifdef DEBUG
                    puts ("  Current used wm item null");
#endif
                    e_state = WM_STATE_SET_SAVED_BG_CMD;
                }
                break;

            case WM_STATE_CHECK_CURR_USED_WM_CMD:
#ifdef DEBUG
                puts (" Checking current used wm cmd string");
#endif
                if (s_cu_wm_cmd != NULL) {
#ifdef DEBUG
                    puts ("  Current used wm command ok");
#endif
                    s_result_cmd = s_cu_wm_cmd;
                    e_state      = WM_STATE_END;
                }
                else {
#ifdef DEBUG
                    puts ("  Current used wm command null");
#endif
                    e_state = WM_STATE_SET_SAVED_BG_CMD;
                }
                break;

            case WM_STATE_SET_SAVED_BG_CMD:
#ifdef DEBUG
                puts (" Checking saved wm cmd string");
#endif
                if (s_setts_cmd != NULL) {
#ifdef DEBUG
                    puts ("  Saved wm command ok");
#endif
                    s_result_cmd = s_setts_cmd;
                    e_state      = WM_STATE_END;
                }
                else {
#ifdef DEBUG
                    puts ("  Saved wm command null");
#endif
                    e_state = WM_STATE_SET_MAIN_DEFAULT;
                }
                break;

            case WM_STATE_SET_MAIN_DEFAULT:
#ifdef DEBUG
                puts (" Setting main default cmd string");
#endif
                s_result_cmd = DEFAULT_BG_CMD;
                e_state      = WM_STATE_END;
                break;

            case WM_STATE_END:
                break;

            default:
                break;
        }
    }
    return strdup (s_result_cmd);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check setting with wm info, remove from st_defaults all wms present
 *         in st_settings list.
 */
int
wms_check_for_new_wms (void)
{
    Setting *st_home      = NULL;   /* For user's wm info */
    Setting *st_nxt       = NULL;   /* Next setting in list */
    Setting *st_default   = NULL;   /* For app default wm info */
    Setting *st_item_home = NULL;   /* Temp for saved data item */
    Setting *st_item_def  = NULL;   /* Temp for default data item */
    int      i_err        = ERR_OK; /* Error output */

    /* Load user's wm info data */
    st_home = wms_get_wm_info_home (&i_err);
    /* Load app default wm info data */
    st_default = wms_get_wm_info_data (&i_err);
    if (i_err != ERR_OK) {
        settings_free_all (st_default);
        settings_free_all (st_home);
        return i_err;
    }
    /* Iterate and remove from app default data wms present in user's info */
    st_item_home = st_home;

    while (st_item_home != NULL) {

        st_item_def = st_default;

        while (st_item_def != NULL) {

            st_nxt = st_item_def->next;

            if (settings_equal_names (st_item_home, st_item_def)) {
#ifdef DEBUG
                printf ("Removing %s\n", st_item_def->s_name);
#endif
                st_default = setting_remove (st_item_def);
                break;
            }
            st_item_def = st_nxt;
        }
        st_item_home = st_item_home->next;
    }
    /* If there are some window managers left in app default wm data,
     * update user's wm data with this window managers */
    if (setting_count_children (st_default) > 0) {
        i_err = wms_update_wm_config (st_default);
    }
    settings_free_all (st_default);
    settings_free_all (st_home);
    return i_err;
}
/*----------------------------------------------------------------------------*/

