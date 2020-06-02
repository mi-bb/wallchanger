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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <err.h>
#include "procfn.h"
#include "wmsfn.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   wms
 * @brief Array with window managers.
 */
static Wms wms[] = {
    {1, WM_ID_MATE, "mate-session", "MATE",
     "gsettings set org.mate.background picture-filename \"[F]\""},
    {2, WM_ID_XFCE, "xfce4-session", "Xfce", ""},
    {3, WM_ID_GNOME, "gnome-session-b", "Gnome",
     "gsettings set org.gnome.desktop.background picture-uri \"file://[F]\""},
    {4, WM_ID_GNOME, "gnome-session-binary", "Gnome",
     "gsettings set org.gnome.desktop.background picture-uri \"file://[F]\""},
    {5, WM_ID_CINNAMON, "cinnamon-sessio", "Cinnamon",
     "gsettings set org.cinnamon.desktop.background picture-uri \"file://[F]\""},
    {6, WM_ID_CINNAMON, "cinnamon-session", "Cinnamon",
     "gsettings set org.cinnamon.desktop.background picture-uri \"file://[F]\""},
    {7, WM_ID_CINNAMON, "cinnamon", "Cinnamon",
     "gsettings set org.cinnamon.desktop.background picture-uri \"file://[F]\""},
    {8, WM_ID_PLASMA, "plasma_session", "KDE Plasma",
     "qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScri"
     "pt 'var allDesktops = desktops();print (allDesktops);for (i=0;i<allDesk"
     "tops.length;i++) {d = allDesktops[i];d.wallpaperPlugin = \"org.kde.imag"
     "e\";d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"Ge"
     "neral\");d.writeConfig(\"Image\", \"file://[F]\")}'"},
    {9, WM_ID_LXDE, "lxsession", "LXDE",
     "pcmanfm --wallpaper-mode crop --set-wallpaper \"[F]\""},
    {10, WM_ID_FVWM, "fvwm2", "FVWM",
     "feh --bg-fill \"[F]\""},
    {11, WM_ID_I3, "i3", "i3",
     "feh --bg-fill \"[F]\""},
    {12, WM_ID_SPECTRWM, "spectrwm", "spectrwm",
     "feh --bg-fill \"[F]\""},
    {13, WM_ID_OPENBOX, "openbox", "Openbox",
     "feh --bg-fill \"[F]\""},
    {14, WM_ID_FLUXBOX, "fluxbox", "Fluxbox",
     "feh --bg-fill \"[F]\""},
    {15, WM_ID_WMAKER, "wmaker", "Window Maker",
     "feh --bg-fill \"[F]\""},
    {16, WM_ID_END, "", "", ""}
    };
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create a copy of Wms item.
 *
 * @param[in]  wms_item  Wms item to copy
 * @return     New Wms item
 */
static Wms *
wms_copy (Wms *wms_item)
{
    Wms *wm_new = NULL;

    if ((wm_new = malloc (sizeof (Wms))) == NULL)
        err (EXIT_FAILURE, NULL);

    wm_new->id    = wms_item->id;
    wm_new->wm_id = wms_item->wm_id;
    strcpy (wm_new->name,    wms_item->name);
    strcpy (wm_new->process, wms_item->process);
    strcpy (wm_new->command, wms_item->command);

    return wm_new;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get null terminated list of window manager's data.
 */
Wms **
wms_list_get (void)
{
    Wms   **wms_list = NULL; /* List of window managers to return */
    Wms   **wms_temp = NULL; /* Temp for realloc */
    Wms    *wm_list  = NULL; /* Input source of window manager list */
    size_t  i        = 0;    /* i */
    size_t  ui_alloc = 0;    /* Number of data to alloc */

    wm_list  = wms;
    wms_list = malloc ((ui_alloc + 1) * sizeof (Wms*));

    while (wm_list->wm_id != WM_ID_END) {
        ++ui_alloc;
        wms_temp = realloc (wms_list, (ui_alloc + 1) * sizeof (Wms*));
        if (wms_temp == NULL) {
            for (i = 0; i < ui_alloc-1; ++i)
                free (wms_list[i]);
            free (wms_list);
            err (EXIT_FAILURE, NULL);
        }
        wms_list = wms_temp;
        wms_list[ui_alloc-1] = wms_copy (wm_list++);
    }
    wms_list[ui_alloc] = NULL;
    return wms_list;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free list of window manager's data.
 */
void
wms_list_free (Wms **wms_list)
{
    Wms **wms_it = NULL;

    for (wms_it = wms_list; *wms_it != NULL; ++wms_it)
        free (*wms_it);
    free (wms_list);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of Xfce displays possible to set wallpaper.
 */
char **
get_xfce_display_list (void)
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

    f_file = popen ("xfconf-query -c xfce4-desktop -p /backdrop -l", "r");

    if (f_file == NULL) {
        warnx ("Failed to run checking displays");
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
 * @brief  Find window manager that is currently in use.
 */
Wms *
find_window_magager (void)
{
    Wms *wms_ret = NULL; /* Wms to return */
    Wms *wm_list = NULL;

    wm_list = wms;

    while (wm_list->wm_id != WM_ID_END) {
        if (process_exists_b (wm_list->process)) {
            return wms_copy (wm_list);
        }
        wm_list++;
    }
    return wms_ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find command for current window manager.
 */
char *
cmddialog_find_command (void)
{
    char *s_ret  = NULL; /* Command to return */
    Wms  *wms_wm = NULL; /* Window manager info */

    if ((wms_wm = find_window_magager ()) != NULL) {
        s_ret = strdup (wms_wm->command);
        free (wms_wm);
    }
    return s_ret;
}
/*----------------------------------------------------------------------------*/

