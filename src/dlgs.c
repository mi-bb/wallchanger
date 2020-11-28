/**
 * @file  dlgs.c
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
#include <inttypes.h>
#include <err.h>
#include "hashfun.h"
#include "errs.h"
#include "setts.h"
#include "defs.h"
#include "dlgsmsg.h"
#include "webwidget.h"
#include "dlgs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Run dialog for getting wallpapers from web
 */
GList *
add_images_from_web_dilaog (GtkWindow  *gw_parent,
                            const char *s_cfg_file)
{
    GtkWidget *gw_dialog;       /* Wallpaper from web dialog */
    GtkWidget *gw_content_box;  /* Dialog's box */
    GtkWidget *gw_box;          /* box */
    GtkWidget *gw_hbox;         /* box */
    GtkWidget *gw_scrlw;        /* Scrolled window for icon view */
    WebWidget *ww_widget;       /* WebWidget with widgets and data*/
    GList     *gl_res   = NULL; /* Result list with wallpapers */
    Setting   *st_setts = NULL; /* For settings */
    Setting   *st_st    = NULL; /* For particular setting */
    int        i_err    = 0;    /* Error output */
    int        i_w      = 0;    /* Dialog width */
    int        i_h      = 0;    /* Dialog height */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    /* Get settings for dialog dimensions and info for webwidget */
    st_setts = setts_read (s_cfg_file, &i_err);
    setts_check_defaults (st_setts);
    /* Get dialog width */
    st_st = settings_find (st_setts, get_setting_name (SETT_WEB_DLG_WIDTH));
    if (st_st != NULL) {
        i_w = (int) setting_get_int (st_st);
    }
    /* Get dialog height */
    st_st = settings_find (st_setts, get_setting_name (SETT_WEB_DLG_HEIGHT));
    if (st_st != NULL) {
        i_h = (int) setting_get_int (st_st);
    }
    /* Web widget */
    ww_widget = webwidget_create (st_setts, s_cfg_file);

    settings_free_all (st_setts);

    /* Main dialog window */
    gw_dialog = gtk_dialog_new_with_buttons (
                                      "Get wallpapers from web",
                                      gw_parent,
                                      flags,
                                      "_OK",
                                      GTK_RESPONSE_ACCEPT,
                                      "_Cancel",
                                      GTK_RESPONSE_REJECT,
                                      NULL);
    gtk_window_set_default_size (GTK_WINDOW (gw_dialog), i_w, i_h);

    gw_content_box = gtk_dialog_get_content_area (GTK_DIALOG (gw_dialog));
    gtk_container_set_border_width (GTK_CONTAINER (gw_content_box), 8);

    /* Scrolled window for wallpeper icon view */
    gw_scrlw = gtk_scrolled_window_new (
        gtk_scrollable_get_hadjustment (GTK_SCROLLABLE (ww_widget->gw_img_view)),
        gtk_scrollable_get_vadjustment (GTK_SCROLLABLE (ww_widget->gw_img_view)));

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (gw_scrlw),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER (gw_scrlw), ww_widget->gw_img_view);

    /* Boxes for result label, nav widget and image info */
    gw_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gw_box  = gtk_box_new (GTK_ORIENTATION_VERTICAL,   4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        ww_widget->gw_count_label,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box),
                        ww_widget->gw_nav_box,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gw_box,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        ww_widget->gw_ii_widget,
                        FALSE, FALSE, 4);

    /* Packing dialog widgets */
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new ("Wallpaper search query: "),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        ww_widget->gw_search_box,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_scrlw,
                        TRUE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_hbox,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        ww_widget->gw_selected_box,
                        FALSE, FALSE, 4);

    gtk_widget_show_all (gw_content_box);

    if (gtk_dialog_run (GTK_DIALOG (gw_dialog)) == GTK_RESPONSE_ACCEPT) {
        gl_res = save_selected_wallpapers (gw_dialog, ww_widget);
    }
    gtk_window_get_size (GTK_WINDOW (gw_dialog), &i_w, &i_h);
    setts_update_web_dlg_size (s_cfg_file, i_w, i_h);

    webwidget_free (ww_widget);
    gtk_widget_destroy (gw_dialog);
    return gl_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select folder dialog.
 */
char *
add_images_folder_dialog (GtkWindow *gw_parent)
{
    GtkWidget *gw_dialog;    /* Directory choose dialog */
    char      *s_dir = NULL; /* Selected folder name */

    gw_dialog = gtk_file_chooser_dialog_new (
            "Select Folder",
            gw_parent,
            GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
            "_Cancel",
            GTK_RESPONSE_CANCEL,
            "_Open",
            GTK_RESPONSE_ACCEPT,
            NULL);

    if (gtk_dialog_run (GTK_DIALOG (gw_dialog)) == GTK_RESPONSE_ACCEPT) {
        s_dir = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (gw_dialog));
    }
    gtk_widget_destroy (gw_dialog);
    return s_dir;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select images dialog.
 */
GSList *
add_images_dialog (GtkWindow *gw_parent)
{
    GtkFileFilter *gff_filter;
    GtkWidget     *gw_dialog;
    GSList        *gsl_files = NULL; /* Result file list */

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

    if (gtk_dialog_run (GTK_DIALOG (gw_dialog)) == GTK_RESPONSE_ACCEPT) {
        gsl_files = gtk_file_chooser_get_filenames (
                GTK_FILE_CHOOSER (gw_dialog));
    }
    gtk_widget_destroy (gw_dialog);
    return gsl_files;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with information about application.
 */
void
about_app_dialog (GtkWindow *gw_parent)
{
    GtkWidget *gw_dialog;
    const char *s_authors[] = {"Michal Babik <michal.babik@protonmail.com> "
                               "https://init6.pomorze.pl", NULL};
    const char *s_art[]     = {
        "https://fr.wikisource.org/wiki/Le_Tour_de_la_France_par_deux_enfants",
        NULL};

    gw_dialog = gtk_about_dialog_new ();
    gtk_window_set_transient_for (GTK_WINDOW (gw_dialog), gw_parent);

    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (gw_dialog), APP_VER);

    gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (gw_dialog), 
            "Copyright (C) 2019-2020 Michal Babik");

    gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (gw_dialog), APP_DSC);

    gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG (gw_dialog),
                                       GTK_LICENSE_GPL_3_0);

    gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (gw_dialog), APP_WEB);

    gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (gw_dialog), s_authors);
    gtk_about_dialog_set_artists (GTK_ABOUT_DIALOG (gw_dialog), s_art);
    gtk_dialog_run (GTK_DIALOG (gw_dialog));
    gtk_widget_destroy (gw_dialog);
}
/*----------------------------------------------------------------------------*/

