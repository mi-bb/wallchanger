/**
 * @file  image_dialogs.c
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
 * @brief  Image dialogs
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <inttypes.h>
#include <err.h>
#include "settings.h"
#include "webwidget.h"
#include "image_dialogs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Run dialog for getting wallpapers from web
 */
GList *
add_images_from_web_dialog (GtkWindow  *parent,
                            const char *cfg_file)
{
    GtkWidget *dialog;             /* Wallpaper from web dialog */
    GtkWidget *content_box;        /* Dialog's box */
    GtkWidget *box;                /* box */
    GtkWidget *hbox;               /* box */
    GtkWidget *scrolled_window;    /* Scrolled window for icon view */
    WebWidget *web_widget;         /* WebWidget with widgets and data*/
    GList     *result   = nullptr; /* Result list with wallpapers */
    Setting   *settings = nullptr; /* For settings */
    Setting   *sett     = nullptr; /* For particular setting */
    int        err      = 0;       /* Error output */
    int        width    = 0;       /* Dialog width */
    int        height   = 0;       /* Dialog height */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    /* Get settings for dialog dimensions and info for webwidget */
    settings = setts_read (cfg_file, &err);
    settings = setts_check_defaults (settings);
    /* Get dialog width */
    sett = settings_find (settings, get_setting_name (SETT_WEB_DLG_WIDTH));
    if (sett != nullptr) {
        width = (int) setting_get_int (sett);
    }
    /* Get dialog height */
    sett = settings_find (settings, get_setting_name (SETT_WEB_DLG_HEIGHT));
    if (sett != nullptr) {
        height = (int) setting_get_int (sett);
    }
    /* Web widget */
    web_widget = webwidget_create (settings, cfg_file);

    settings_free_all (settings);

    /* Main dialog window */
    dialog = gtk_dialog_new_with_buttons (
                                      "Get wallpapers from web",
                                      parent,
                                      flags,
                                      "_OK",
                                      GTK_RESPONSE_ACCEPT,
                                      "_Cancel",
                                      GTK_RESPONSE_REJECT,
                                      nullptr);
    gtk_window_set_default_size (GTK_WINDOW (dialog), width, height);

    content_box = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    gtk_container_set_border_width (GTK_CONTAINER (content_box), 8);

    /* Scrolled window for wallpeper icon view */
    scrolled_window = gtk_scrolled_window_new (
        gtk_scrollable_get_hadjustment (GTK_SCROLLABLE (web_widget->gw_img_view)),
        gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (web_widget->gw_img_view)));

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER (scrolled_window), web_widget->gw_img_view);

    /* Boxes for result label, nav widget and image info */
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    box  = gtk_box_new (GTK_ORIENTATION_VERTICAL,   4);
    gtk_box_pack_start (GTK_BOX (box),
                        web_widget->gw_count_label,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box),
                        web_widget->gw_nav_box,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox),
                        box,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox),
                        gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox),
                        web_widget->gw_ii_widget,
                        FALSE, FALSE, 4);

    /* Packing dialog widgets */
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new ("Wallpaper search query: "),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        web_widget->gw_search_box,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        scrolled_window,
                        TRUE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        hbox,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        web_widget->gw_selected_box,
                        FALSE, FALSE, 4);

    gtk_widget_show_all (content_box);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
        result = save_selected_wallpapers (dialog, web_widget);
    }
    gtk_window_get_size (GTK_WINDOW (dialog), &width, &height);
    setts_update_web_dlg_size (cfg_file, width, height);

    webwidget_free (web_widget);
    gtk_widget_destroy (dialog);
    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select folder dialog.
 */
char *
add_images_folder_dialog (GtkWindow *parent)
{
    GtkWidget *dialog;          /* Directory choose dialog */
    char      *dir = nullptr;   /* Selected folder name */

    dialog = gtk_file_chooser_dialog_new (
            "Select Folder",
            parent,
            GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
            "_Cancel",
            GTK_RESPONSE_CANCEL,
            "_Open",
            GTK_RESPONSE_ACCEPT,
            nullptr);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
        dir = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    }
    gtk_widget_destroy (dialog);
    return dir;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select images dialog.
 */
GSList *
add_images_dialog (GtkWindow *parent)
{
    GtkFileFilter *filter;          /* File filter */
    GtkWidget     *dialog;          /* File dialog */
    GSList        *files = nullptr; /* Result file list */

    dialog = gtk_file_chooser_dialog_new ("Select Files",
                                          parent,
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          "_Cancel",
                                          GTK_RESPONSE_CANCEL,
                                          "_Open",
                                          GTK_RESPONSE_ACCEPT,
                                          nullptr);
    filter = gtk_file_filter_new ();
    gtk_file_filter_set_name (filter, "Image files");
    gtk_file_filter_add_pixbuf_formats (filter);
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);
    gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), TRUE);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
        files = gtk_file_chooser_get_filenames (
                GTK_FILE_CHOOSER (dialog));
    }
    gtk_widget_destroy (dialog);
    return files;
}
/*----------------------------------------------------------------------------*/

