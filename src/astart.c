/**
 * @file  astart.c
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
 * @brief  Autostart file set functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include "errs.h"
#include "fdfn.h"
#include "strfun.h"
#include "rwdt.h"
#include "astart.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get path for wchangerd daemon desktop file in user's autostart
 *         directory.
 *
 * @return String with path, it should be freed after use using free
 */
static char *
cfgfile_autostart_get_path (void)
{
    char *s_path = NULL; /* Autostart file path to return */

    s_path = dir_get_home ();
    str_append (&s_path, "/.config/autostart/wchangerd.desktop");

    return s_path;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if wchangerd daemon desktop file exists in user's autostart
 *         directory.
 */
int
autostart_exists (void)
{
    char *s_path = NULL;
    int  i_res   = 0;

    s_path = cfgfile_autostart_get_path ();

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
autostart_create (void)
{
    const char *s_buf = "[Desktop Entry]\n"
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
    char *s_path = NULL;
    int   i_err  = 0;

    s_path = cfgfile_autostart_get_path ();

    if ((i_err = file_create_with_subdirs (s_path)) == ERR_OK) {
        i_err = save_file_data (s_path, s_buf);
    }
    free (s_path);

    return i_err;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Removes wchangerd desktop file from user's autostart directory.
 */
int
autostart_remove (void)
{
    char *s_path = NULL;

    s_path = cfgfile_autostart_get_path ();
    
    if (remove (s_path) != 0) {
        warn ("%s", s_path);
        free (s_path);
        return ERR_FILE_RM;
    }
    free (s_path);
    return ERR_OK;
}
/*----------------------------------------------------------------------------*/

