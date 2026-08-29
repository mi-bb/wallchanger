/**
 * @file  dlgsmsg.h
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
 * @brief  Message dialogs
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef DLGSMSG_H
#define DLGSMSG_H

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Error message dialog.
 *
 * @param[in] gw_parent  Dialog parent widget
 * @param[in] s_message  Message to show
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void     message_dialog_error     (GtkWindow    *gw_parent,
                                   const char   *s_message)
         __attribute__ ((nonnull (2)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Warning message dialog.
 *
 * @param[in] gw_parent  Dialog parent widget
 * @param[in] s_message  Message to show
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void     message_dialog_warning     (GtkWindow  *gw_parent,
                                     const char *s_message)
         __attribute__ ((nonnull (2)));
/*----------------------------------------------------------------------------*/
/**
 *  @brief  Question message dialog.
 *
 * @param[in] gw_parent  Dialog parent widget
 * @param[in] s_message  Message to show
 * @return    1 if Yes pressed 0 if No
 */
int      message_dialog_question    (GtkWindow  *gw_parent,
                                     const char *s_message);
/*----------------------------------------------------------------------------*/

#endif

