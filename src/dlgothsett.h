/**
 * @file  dlgothsett.h
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
 * @brief  Other settings dialog
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef DLGOTHSETT_H
#define DLGOTHSETT_H

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with other settings.
 *
 * @param[in] gw_parent  Parent window
 * @param[in] s_cfg_file Path to config file
 * @param[in] s_fn       Path to sample wallpaper file
 * @return    none
 */
void     other_settings_dialog    (GtkWindow  *gw_parent,
                                   const char *s_cfg_file,
                                   const char *s_fn);
/*----------------------------------------------------------------------------*/
#endif

