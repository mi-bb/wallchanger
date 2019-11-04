/**
 * @file  dlgs.h
 * @copyright Copyright (C) 2019 Michał Bąbik
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
 * @brief  Application dialogs
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef DLGS_H
#define DLGS_H
#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
char *add_images_folder_dialog (GtkWindow *gw_parent);
/*----------------------------------------------------------------------------*/
GSList *add_images_dialog (GtkWindow *gw_parent);
/*----------------------------------------------------------------------------*/
#endif
