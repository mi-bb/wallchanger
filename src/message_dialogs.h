/**
 * @file  message_dialogs.h
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
#ifndef MESSAGE_DIALOGS_H
#define MESSAGE_DIALOGS_H

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Error message dialog.
 *
 * @param[in] parent   Dialog parent widget
 * @param[in] message  Message to show
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void     message_dialog_error     (GtkWindow    *parent,
                                   const char   *message)
         __attribute__ ((nonnull (2)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Warning message dialog.
 *
 * @param[in] parent   Dialog parent widget
 * @param[in] message  Message to show
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void     message_dialog_warning     (GtkWindow  *parent,
                                     const char *message)
         __attribute__ ((nonnull (2)));
/*----------------------------------------------------------------------------*/
/**
 *  @brief  Question message dialog.
 *
 * @param[in] parent   Dialog parent widget
 * @param[in] message  Message to show
 * @return    1 if Yes pressed 0 if No
 */
int      message_dialog_question    (GtkWindow  *parent,
                                     const char *message);
/*----------------------------------------------------------------------------*/

#endif

