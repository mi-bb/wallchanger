/**
 * @file  main_window.h
 * @copyright Copyright (C) 2019-2026 Michał Bąbik
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @brief  Main settings window of wchangercfg
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>
#include "dialog_data.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Build and show the main settings window.
 *
 * Finds the config file, creates the application directories, loads the
 * settings into the window widgets and starts the wchangerd presence
 * monitor. Quits the application if the directories or settings cannot
 * be set up.
 *
 * @param[in,out] app   Pointer to GtkApplication
 * @param[in,out] data  DialogData object with settings and widget data
 * @return        none
 */
void main_window_create (GtkApplication *app,
                         DialogData     *data);
/*----------------------------------------------------------------------------*/
#endif
