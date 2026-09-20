/**
 * @file  dialogs_other.h
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
 * @brief  Other dialogs
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef DIALOGS_OTHER_H
#define DIALOGS_OTHER_H

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with information about application.
 *
 * @param[in] gw_parent  Parent window
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void     about_app_dialog           (GtkWindow  *gw_parent);
/*----------------------------------------------------------------------------*/
#endif

