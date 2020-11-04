/**
 * @file  dlgsmsg.c
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
 * @brief  Message dialogs
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include "dlgsmsg.h"
/*----------------------------------------------------------------------------*/
/**
 *  @brief  Error message dialog.
 */
void
message_dialog_error (GtkWindow  *gw_parent,
                      const char *s_message)
{
    GtkWidget *gw_dialog;

    gw_dialog = gtk_message_dialog_new (gw_parent,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        "%s", s_message);
    gtk_dialog_run (GTK_DIALOG (gw_dialog));
    gtk_widget_destroy (gw_dialog);
}
/*----------------------------------------------------------------------------*/
/**
 *  @brief  Warning message dialog.
 */
void
message_dialog_warning (GtkWindow  *gw_parent,
                        const char *s_message)
{
    GtkWidget *gw_dialog;

    gw_dialog = gtk_message_dialog_new (gw_parent,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_WARNING,
                                        GTK_BUTTONS_CLOSE,
                                        "%s", s_message);
    gtk_dialog_run (GTK_DIALOG (gw_dialog));
    gtk_widget_destroy (gw_dialog);
}
/*----------------------------------------------------------------------------*/
/**
 *  @brief  Question message dialog.
 */
int
message_dialog_question (GtkWindow  *gw_parent,
                         const char *s_message)
{
    GtkWidget *gw_dialog;
    int i_res = 0;

    gw_dialog = gtk_message_dialog_new (gw_parent,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_QUESTION,
                                        GTK_BUTTONS_YES_NO,
                                        "%s", s_message);
    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));
    gtk_widget_destroy (gw_dialog);
    return i_res;
}
/*----------------------------------------------------------------------------*/
