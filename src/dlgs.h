/**
 * @file  dlgs.h
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
 * @brief  Application dialogs
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef DLGS_H
#define DLGS_H

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Run wallpaper set command configuration dialog.
 *
 * @param[in] gw_parent     Window widget for setting dialog modal
 * @param[in] s_current_cmd Current wallpaper set command
 * @param[in] gsl_iinfo     List with wallpapers
 * @return    String with wallpaper set command or null. It should be
 *            freed after use using free.
 */
/*----------------------------------------------------------------------------*/
char *   cmddialog_run            (GtkWindow    *gw_parent,
                                   const char   *s_current_cmd,
                                   const GSList *gsl_iinfo);
/*----------------------------------------------------------------------------*/
/**
 * @fn  char * add_images_folder_dialog (GtkWindow *gw_parent)
 *
 * @brief  Select folder dialog.
 *
 * @param[in] gw_parent  Dialog parent widget
 * @return    Foler path or NULL.
 *
 * @fn  GSList * add_images_dialog (GtkWindow *gw_parent)
 *
 * @brief  Select images dialog.
 *
 * @param[in] gw_parent  Dialog parent widget
 * @return    List with images.
 */
/*----------------------------------------------------------------------------*/
char   * add_images_folder_dialog (GtkWindow    *gw_parent);

GSList * add_images_dialog        (GtkWindow    *gw_parent);
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
 * @brief  Dialog with information about application.
 *
 * @param[in] data  Unused pointer for event call
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void     about_app_dialog         (gpointer     data);
/*----------------------------------------------------------------------------*/
#endif

