/**
 * @file  deffiles.c
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
 * @brief  Default files create / delete ... functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include "errs.h"
#include "cfgfile.h"
#include "defs.h"
#include "fdfn.h"
#include "strfun.h"
#include "rwdt.h"
#include "deffiles.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Template for autostart desktop file with wchangerd daemon.
 */
static const char *s_default_autostart = 
    "[Desktop Entry]\n"
    "Type=Application\n"
    "Name=Wall Changer\n"
    "GenericName=Wallpaper Changer\n"
    "NoDisplay=true\n"
    "Comment=Automatic wallpaper changer\n"
    "Icon=wchangercfg\n"
    "Exec=wchangerd --restart\n"
    "Terminal=false\n"
    "StartupNotify=false\n"
    "Categories=Utility;DesktopSettings;\n";
/*----------------------------------------------------------------------------*/
/**
 * @brief  Template for config file with window manager info.
 */
static const char *s_default_wm_json = "{"
"\"Unknown\": {"
    "\"Proc\": [\"\"],"
    "\"Command\": \"feh --bg-fill \\\"[F]\\\"\""
    "}, "
"\"MATE\": {"
    "\"Proc\": [\"mate-session\"], "
    "\"Command\": \"gsettings set org.mate.background picture-filename "
                  "\\\"[F]\\\"\""
    "}, "
"\"Xfce\": {"
    "\"Proc\": [\"xfce4-session\"], "
    "\"Command\": \"\""
    "}, "
"\"Gnome\": {"
    "\"Proc\": [\"gnome-session-b\", \"gnome-session-binary\"], "
    "\"Command\": \"gsettings set org.gnome.desktop.background picture-uri "
                   "\\\"file://[F]\\\"\""
    "}, "
"\"Cinnamon\": {"
    "\"Proc\": [\"cinnamon-sessio\", \"cinnamon-session\", \"cinnamon\"], "
    "\"Command\": \"gsettings set org.cinnamon.desktop.background picture-uri "
                   "\\\"file://[F]\\\"\""
    "}, "
"\"KDE Plasma\": {"
    "\"Proc\": [\"plasma_session\"], "
    "\"Command\": \"qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell"
    ".evaluateScript 'var allDesktops = desktops();print (allDesktops);for (i="
    "0;i<allDesktops.length;i++) {d = allDesktops[i];d.wallpaperPlugin = \\\"o"
    "rg.kde.image\\\";d.currentConfigGroup = Array(\\\"Wallpaper\\\", \\\"org."
    "kde.image\\\", \\\"General\\\");d.writeConfig(\\\"Image\\\", \\\"file://["
    "F]\\\")}'\""
    "}, "
"\"LXDE\": {"
    "\"Proc\": [\"lxsession\"], "
    "\"Command\": \"pcmanfm --wallpaper-mode crop --set-wallpaper "
                  "\\\"[F]\\\"\""
    "}, "
"\"FVWM\": {"
    "\"Proc\": [\"fvwm2\"],"
    "\"Command\": \"feh --bg-fill \\\"[F]\\\"\""
    "}, "
"\"i3\": {"
    "\"Proc\": [\"i3\"],"
    "\"Command\": \"feh --bg-fill \\\"[F]\\\"\""
    "}, "
"\"dwm\": {"
    "\"Proc\": [\"dwm\"],"
    "\"Command\": \"feh --bg-fill \\\"[F]\\\"\""
    "}, "
"\"spectrwm\": {"
    "\"Proc\": [\"spectrwm\"],"
    "\"Command\": \"feh --bg-fill \\\"[F]\\\"\""
    "}, "
"\"Openbox\": {"
    "\"Proc\": [\"openbox\"],"
    "\"Command\": \"feh --bg-fill \\\"[F]\\\"\""
    "}, "
"\"Fluxbox\": {"
    "\"Proc\": [\"fluxbox\"],"
    "\"Command\": \"feh --bg-fill \\\"[F]\\\"\""
    "}, "
"\"Window Maker\": {"
    "\"Proc\": [\"wmaker\"],"
    "\"Command\": \"feh --bg-fill \\\"[F]\\\"\""
    "} "
"}";
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd daemon desktop file exists in user's autostart
 *         directory.
 */
int
deffiles_autostart_exists (void)
{
    char *s_path = NULL;
    int  i_res   = 0;

    s_path = cfgfile_get_autostart_file_path ();

    if (file_check_permissions (s_path) == ERR_OK) {
        i_res = 1;
    }
    free (s_path);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates wchangerd desktop file in user's autostart directory.
 */
int
deffiles_autostart_create (void)
{
    char *s_path = NULL;
    int   i_err  = 0;

    s_path = cfgfile_get_autostart_file_path ();

    if ((i_err = file_create_with_subdirs (s_path)) == ERR_OK) {
        i_err = save_file_data (s_path, s_default_autostart);
    }
    free (s_path);

    return i_err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Removes wchangerd desktop file from user's autostart directory.
 */
int
deffiles_autostart_remove (void)
{
    char *s_path = NULL;

    s_path = cfgfile_get_autostart_file_path ();
    
    if (remove (s_path) != 0) {
        warn ("%s", s_path);
        free (s_path);
        return ERR_FILE_RM;
    }
    free (s_path);
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check existence of json config file with window manager info,
 *         creates it if there is no such file.
 */
int
deffiles_wm_check_create (void)
{
    char *s_path = NULL;
    int   i_err  = 0;

    s_path = cfgfile_get_wm_info_file_path ();

    i_err = file_check_permissions (s_path);
    if (i_err == ERR_FILE_EX) {
        i_err = file_create_with_subdirs (s_path);
        if (i_err == ERR_OK) {
            i_err = save_file_data (s_path, s_default_wm_json);
        }
    }
    free (s_path);

    return i_err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get copy of string with window manager info (config file template).
 */
char *
deffiles_wm_get_buff_copy (void)
{
    return strdup (s_default_wm_json);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get string with window manager info (config file template).
 */
const char *
deffiles_wm_get_buff (void)
{
    return s_default_wm_json;
}
/*----------------------------------------------------------------------------*/

