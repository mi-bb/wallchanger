/**
 * @file  dlgs.c
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
#include "dlgs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select folder dialog.
 */
char *
add_images_folder_dialog (GtkWindow *gw_parent)
{
    GtkWidget  *gw_dialog;        /* Directory choose dialog */
    char       *s_folder  = NULL; /* Result selected folder string */
    int         res       = 0;    /* Dialog run response */

    gw_dialog = gtk_file_chooser_dialog_new ("Select Folder",
                                          gw_parent,
                                          GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                          "_Cancel",
                                          GTK_RESPONSE_CANCEL,
                                          "_Open",
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        s_folder = gtk_file_chooser_get_filename (
                GTK_FILE_CHOOSER (gw_dialog));
    }
    gtk_widget_destroy (gw_dialog);

    return s_folder;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select images dialog.
 */
GSList *
add_images_dialog (GtkWindow *gw_parent)
{
    GSList        *gsl_files = NULL; /* Result file list */
    GtkFileFilter *gff_filter;
    GtkWidget     *gw_dialog;
    int            res       = 0;    /* Dialog run response */

    gw_dialog = gtk_file_chooser_dialog_new ("Select Files",
                                             gw_parent,
                                             GTK_FILE_CHOOSER_ACTION_OPEN,
                                             "_Cancel",
                                             GTK_RESPONSE_CANCEL,
                                             "_Open",
                                             GTK_RESPONSE_ACCEPT,
                                             NULL);
    gff_filter = gtk_file_filter_new ();
    gtk_file_filter_set_name (gff_filter, "Image files");
    gtk_file_filter_add_pixbuf_formats (gff_filter);
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (gw_dialog), gff_filter);
    gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (gw_dialog), TRUE);

    res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        gsl_files = gtk_file_chooser_get_filenames (
                GTK_FILE_CHOOSER (gw_dialog));
      }

    gtk_widget_destroy (gw_dialog);

    return gsl_files;
}
/*----------------------------------------------------------------------------*/
/**
 *  @brief  Error message dialog.
 */
void
message_dialog_error (GtkWindow  *gw_parent,
                      const char *s_message)
{
    GtkWidget *gw_dialog;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    gw_dialog = gtk_message_dialog_new (gw_parent,
                                        flags,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        "%s", s_message);
    gtk_dialog_run (GTK_DIALOG (gw_dialog));
    gtk_widget_destroy (gw_dialog);
}
/*----------------------------------------------------------------------------*/

