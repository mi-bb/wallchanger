/**
 * @file  other_settings_dialog.c
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
 * @brief  Other settings dialog
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include "message_dialogs.h"
#include "dir_content.h"
#include "daemon/daemon.h"
#include "settings/setting_item.h"
#include "settings/settings.h"
#include "settings/cfg_file.h"
#include "util/string_functions.h"
#include "other_settings_dialog.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Enum for config dialog widgets.
 */
enum
e_sett_dial_widgets {
    GW_SETT_IMG_O,            /**< Thumbnail preview */
    GW_SETT_IMG_P,            /**< Thumbnail JPG preview */
    GW_SETT_JPG_SPIN,         /**< Spinbutton for jpg quality */
    GW_SETT_QUERY_PATH_LABEL, /**< Query directory label */
    GW_SETT_QUERY_SIZE_LABEL, /**< Query size label */
    GW_SETT_THUMB_PATH_LABEL, /**< Thumbnails directory label */
    GW_SETT_THUMB_SIZE_LABEL, /**< Thumbnails size label */
    GW_SETT_CONF_PATH_LABEL,  /**< Configuration dir label */
    GW_SETT_CONF_SIZE_LABEL,  /**< Configuration size */
    GW_SETT_WALL_PATH_LABEL,  /**< Wallpapers dir label */
    GW_SETT_WALL_SIZE_LABEL,  /**< Wallpapers dir size */
    GW_SETT_CNT               /**< Number of Widgets */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Refresh JPG preview image using new value of JPG quality.
 *
 * @param[in,out] widgets  Array with widgets
 * @return        none
 */
static void
refresh_preview (GtkWidget **widgets)
{
    GdkPixbuf *source      = nullptr; /* Original pixbuf */
    GdkPixbuf *jpg         = nullptr; /* JPG pixbuf */
    char      *buffer      = nullptr; /* Data buffer */
    gsize      buffer_size = 0;       /* Size of data buffer */
    int        quality     = 0;       /* JPG quality value */
    char       quality_str[8];        /* JPG quality value as a string */
    GInputStream *stream;             /* Stream for passing data to pixbuf */

    if (widgets[GW_SETT_IMG_O] == nullptr)
        return;

    quality = gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON (widgets[GW_SETT_JPG_SPIN]));

    source = gtk_image_get_pixbuf (GTK_IMAGE (widgets[GW_SETT_IMG_O]));

    if (source != nullptr) {
        sprintf (quality_str, "%d", quality);
        if (gdk_pixbuf_save_to_buffer (source, &buffer, &buffer_size,
                                       "jpeg", nullptr,
                                       "quality", quality_str, nullptr)) {
            stream = g_memory_input_stream_new ();
            g_memory_input_stream_add_data (G_MEMORY_INPUT_STREAM (stream),
                                            buffer, (gssize) buffer_size,
                                            nullptr);
            jpg = gdk_pixbuf_new_from_stream (stream, nullptr, nullptr);
            free (buffer);
            g_input_stream_close (stream, nullptr, nullptr);
            g_object_unref (stream);
            if (jpg != nullptr) {
                gtk_image_clear (GTK_IMAGE (widgets[GW_SETT_IMG_P]));
                gtk_image_set_from_pixbuf (GTK_IMAGE (widgets[GW_SETT_IMG_P]),
                                           jpg);
                g_object_unref (jpg);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Deleting query cache files.
 *
 * @param[in,out] widgets  Array with widgets
 * @return        none
 */
static void
event_delete_query_data (GtkWidget **widgets)
{
    char       *size = nullptr;   /* String with size of dir after delete */
    const char *path = nullptr;   /* Directory path */

    path = gtk_label_get_text (
            GTK_LABEL (widgets[GW_SETT_QUERY_PATH_LABEL]));
    delete_dir_content (path);
    size = g_format_size ((guint64) get_dir_size (path));
    gtk_label_set_text (GTK_LABEL (widgets[GW_SETT_QUERY_SIZE_LABEL]), size);
    free (size);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Deleting thumbnails.
 *
 * @param[in,out] widgets  Array with widgets
 * @return        none
 */
static void
event_delete_thumb_data (GtkWidget **widgets)
{
    char       *size = nullptr;   /* String with size of dir after delete */
    const char *path = nullptr;   /* Directory path */

    path = gtk_label_get_text (
            GTK_LABEL (widgets[GW_SETT_THUMB_PATH_LABEL]));
    delete_dir_content (path);
    size = g_format_size ((guint64) get_dir_size (path));
    gtk_label_set_text (GTK_LABEL (widgets[GW_SETT_THUMB_SIZE_LABEL]), size);
    free (size);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Deleting downloaded wallpapers.
 *
 * @param[in,out] widgets  Array with widgets
 * @return        none
 */
static void
event_delete_wallpapers (GtkWidget **widgets)
{
    char       *size = nullptr;   /* String with size of dir after delete */
    const char *path = nullptr;   /* Directory path */

    if (message_dialog_question (nullptr, "Do you really want to delete all "
                "downloaded wallpapers ?")) {

        path = gtk_label_get_text (
                GTK_LABEL (widgets[GW_SETT_WALL_PATH_LABEL]));
        delete_dir_content (path);
        size = g_format_size ((guint64) get_dir_size (path));
        gtk_label_set_text (GTK_LABEL (widgets[GW_SETT_WALL_SIZE_LABEL]),
                            size);
        free (size);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Deleting configuration files.
 *
 * @param[in,out] widgets  Array with widgets
 * @return        none
 */
static void
event_delete_config_files (GtkWidget **widgets)
{
    char       *size = nullptr;   /* String with size of dir after delete */
    const char *path = nullptr;   /* Directory path */

    if (message_dialog_question (nullptr, "Do you really want to delete all "
                "applicatoin configuration files ?\nThis will result in the"
                "termination of the application.")) {

        path = gtk_label_get_text (
                GTK_LABEL (widgets[GW_SETT_CONF_PATH_LABEL]));
        delete_dir_content (path);
        size = g_format_size ((guint64) get_dir_size (path));
        gtk_label_set_text (GTK_LABEL (widgets[GW_SETT_CONF_SIZE_LABEL]),
                            size);
        free (size);
        daemon_kill ();
        exit (EXIT_SUCCESS);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Open directory stored in given label
 *
 * @param[in] label  Label with directory path
 * @return        none
 */
static void
event_open_dir_from_label (GtkLabel *label)
{
    [[maybe_unused]] int result = 0; /* Result of system command for
                                       compiler complain */
    char *cmd = str_comb ("xdg-open ", gtk_label_get_text (label));
    result = system(cmd);
    free (cmd);
}
/*----------------------------------------------------------------------------*/

/**
 * @brief  Dialog with other settings.
 */
void
other_settings_dialog (GtkWindow  *parent,
                       const char *cfg_file,
                       const char *sample_file)
{
    GtkWidget *widgets[GW_SETT_CNT]; /* Array for app widgets */
    GtkWidget *dialog;              /* Dialog window */
    GtkWidget *content_box;         /* Dialog's box */
    GtkWidget *hbox;                /* box */
    GtkWidget *grid;                /* grid */
    GtkWidget *thumb_open_btn;      /* Thumbnails open button */
    GtkWidget *query_open_btn;      /* Query open button */
    GtkWidget *conf_open_btn;       /* Config open button */
    GtkWidget *wall_open_btn;       /* Wallpapers open button */
    GtkWidget *thumb_clear_btn;     /* Thumbnails clear button */
    GtkWidget *query_clear_btn;     /* Query clear button */
    GtkWidget *conf_clear_btn;      /* Config delete button */
    GtkWidget *wall_clear_btn;      /* Wallpapers delete button */
    GtkWidget *jpg_spin;            /* Spinbuton for jpg quality */
    GtkWidget *orig_image  = nullptr;  /* Original thumb image */
    GtkWidget *jpg_image   = nullptr;  /* JPG thumb image */
    GdkPixbuf *pixbuf      = nullptr;  /* Pixbuf */
    GError    *error       = nullptr;  /* For error output */
    Setting   *settings    = nullptr;  /* For setting */
    Setting   *setting     = nullptr;  /* For particular setting */
    char      *thumb_path  = nullptr;  /* Thumbnails directory */
    char      *thumb_size  = nullptr;  /* Size of thumbnails dir */
    char      *query_path  = nullptr;  /* Query info directory */
    char      *query_size  = nullptr;  /* Size of query dir; */
    char      *conf_path   = nullptr;  /* Config directory */
    char      *conf_size   = nullptr;  /* Size of config dir */
    char      *wall_path   = nullptr;  /* Wallpapers directory */
    char      *wall_size   = nullptr;  /* Size of wallpapers dir */
    int        error_code  = 0;     /* Error output */
    int        jpg_quality = 96;    /* JPG quality val */
    GtkAdjustment *adjustment;      /* Adjustment for jpg quality */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    /* Get settings for dialog dimensions and info for webwidget */
    settings = setts_read (cfg_file, &error_code);
    settings = setts_check_defaults (settings);

    if ((setting = settings_find (
                    settings, get_setting_name (SETT_THUMB_QUALITY))) != nullptr) {
        jpg_quality = (int) setting_get_int (setting);
    }
    settings_free_all (settings);

    /* Main dialog window */
    dialog = gtk_dialog_new_with_buttons (
                                      "Other settings",
                                      parent,
                                      flags,
                                      "_OK",
                                      GTK_RESPONSE_ACCEPT,
                                      "_Cancel",
                                      GTK_RESPONSE_REJECT,
                                      nullptr);

    content_box = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    gtk_container_set_border_width (GTK_CONTAINER (content_box), 8);

    thumb_path = cfg_file_get_app_thumbnails_path ();
    query_path = cfg_file_get_query_path ();
    conf_path  = cfg_file_get_app_config_path ();
    wall_path  = cfg_file_get_app_wallpapers_path ();
    thumb_size = g_format_size ((guint64) get_dir_size (thumb_path));
    query_size = g_format_size ((guint64) get_dir_size (query_path));
    conf_size  = g_format_size ((guint64) get_dir_size (conf_path));
    wall_size  = g_format_size ((guint64) get_dir_size (wall_path));
    widgets[GW_SETT_THUMB_PATH_LABEL] = gtk_label_new (thumb_path);
    widgets[GW_SETT_THUMB_SIZE_LABEL] = gtk_label_new (thumb_size);
    widgets[GW_SETT_QUERY_PATH_LABEL] = gtk_label_new (query_path);
    widgets[GW_SETT_QUERY_SIZE_LABEL] = gtk_label_new (query_size);
    widgets[GW_SETT_CONF_PATH_LABEL]  = gtk_label_new (conf_path);
    widgets[GW_SETT_CONF_SIZE_LABEL]  = gtk_label_new (conf_size);
    widgets[GW_SETT_WALL_PATH_LABEL]  = gtk_label_new (wall_path);
    widgets[GW_SETT_WALL_SIZE_LABEL]  = gtk_label_new (wall_size);
    thumb_open_btn  = gtk_button_new_with_label ("Open");
    query_open_btn  = gtk_button_new_with_label ("Open");
    conf_open_btn   = gtk_button_new_with_label ("Open");
    wall_open_btn   = gtk_button_new_with_label ("Open");
    thumb_clear_btn = gtk_button_new_with_label ("Clear");
    query_clear_btn = gtk_button_new_with_label ("Clear");
    conf_clear_btn  = gtk_button_new_with_label ("Delete");
    wall_clear_btn  = gtk_button_new_with_label ("Delete");
    gtk_widget_set_tooltip_text (thumb_open_btn,
                                 "Open thumbnail cache directory");
    gtk_widget_set_tooltip_text (query_open_btn,
                                 "Open query info cache directory");
    gtk_widget_set_tooltip_text (conf_open_btn,
                                 "Open configuration files directory");
    gtk_widget_set_tooltip_text (wall_open_btn,
                                 "Open downloaded wallpapers directory");
    gtk_widget_set_tooltip_text (thumb_clear_btn, "Clear thumbnail cache");
    gtk_widget_set_tooltip_text (query_clear_btn, "Clear query info cache");
    gtk_widget_set_tooltip_text (conf_clear_btn, "Delete configuration files");
    gtk_widget_set_tooltip_text (wall_clear_btn,
                                 "Delete downloaded wallpapers");
    free (thumb_path);
    free (query_path);
    free (thumb_size);
    free (query_size);
    free (conf_path);
    free (conf_size);
    free (wall_path);
    free (wall_size);

    g_signal_connect_swapped (thumb_open_btn, "clicked",
                              G_CALLBACK (event_open_dir_from_label),
                                  GTK_LABEL (
                                      widgets[GW_SETT_THUMB_PATH_LABEL]));
    g_signal_connect_swapped (query_open_btn, "clicked",
                              G_CALLBACK (event_open_dir_from_label),
                                  GTK_LABEL (
                                      widgets[GW_SETT_QUERY_PATH_LABEL]));
    g_signal_connect_swapped (conf_open_btn, "clicked",
                              G_CALLBACK (event_open_dir_from_label),
                                  GTK_LABEL (
                                      widgets[GW_SETT_CONF_PATH_LABEL]));
    g_signal_connect_swapped (wall_open_btn, "clicked",
                              G_CALLBACK (event_open_dir_from_label),
                                  GTK_LABEL (
                                      widgets[GW_SETT_WALL_PATH_LABEL]));

    g_signal_connect_swapped (thumb_clear_btn, "clicked",
                              G_CALLBACK (event_delete_thumb_data),
                              widgets);
    g_signal_connect_swapped (query_clear_btn, "clicked",
                              G_CALLBACK (event_delete_query_data),
                              widgets);
    g_signal_connect_swapped (wall_clear_btn, "clicked",
                              G_CALLBACK (event_delete_wallpapers),
                              widgets);
    g_signal_connect_swapped (conf_clear_btn, "clicked",
                              G_CALLBACK (event_delete_config_files),
                              widgets);

    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    adjustment = gtk_adjustment_new ((gdouble) jpg_quality,
                                     0.0, 100.0, 1.0, 10.0, 0.0);
    jpg_spin = gtk_spin_button_new (adjustment, 1.0, 0);
    gtk_box_pack_start (GTK_BOX (hbox),
                        gtk_label_new ("Cached thumbnail jpg quality:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (hbox),
                        jpg_spin,
                        FALSE, FALSE, 4);
    gtk_widget_set_halign (hbox, GTK_ALIGN_CENTER);

    gtk_box_pack_start (GTK_BOX (content_box),
                        hbox,
                        FALSE, FALSE, 4);

    if (sample_file != nullptr) {
        pixbuf = gdk_pixbuf_new_from_file_at_size (sample_file, 250, 250,
                                                   &error);
        g_clear_error (&error);
        if (pixbuf != nullptr) {
            orig_image = gtk_image_new_from_pixbuf (pixbuf);
            jpg_image  = gtk_image_new_from_pixbuf (pixbuf);
            g_object_unref (pixbuf);

            grid = gtk_grid_new ();
            gtk_grid_set_row_spacing    (GTK_GRID (grid), 8);
            gtk_grid_set_column_spacing (GTK_GRID (grid), 8);
            gtk_widget_set_halign (grid, GTK_ALIGN_CENTER);
            gtk_grid_attach (GTK_GRID (grid),
                             gtk_label_new ("Original:"),
                             0,0,1,1);
            gtk_grid_attach (GTK_GRID (grid),
                             gtk_label_new ("JPG Preview:"),
                             1,0,1,1);
            gtk_grid_attach (GTK_GRID (grid),
                             orig_image,
                             0,1,1,1);
            gtk_grid_attach (GTK_GRID (grid),
                             jpg_image,
                             1,1,1,1);
            gtk_box_pack_start (GTK_BOX (content_box),
                                grid,
                                FALSE, FALSE, 4);
        }
        gtk_box_pack_start (GTK_BOX (content_box),
                            gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                            FALSE, FALSE, 4);
    }

    /* Cache directories paths and sized */
    grid = gtk_grid_new ();
    gtk_widget_set_halign (grid, GTK_ALIGN_CENTER);
    gtk_grid_set_row_spacing    (GTK_GRID (grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 8);
    /* Title */
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Application directories"),
                     0,0,4,1);
    /* Query cache */
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Query cache:"),
                     0,2,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     widgets[GW_SETT_QUERY_PATH_LABEL],
                     1,2,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     widgets[GW_SETT_QUERY_SIZE_LABEL],
                     2,2,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     query_open_btn,
                     3,2,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     query_clear_btn,
                     4,2,1,1);
    /* Thumbnail cache */
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Thumbnails:"),
                     0,3,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     widgets[GW_SETT_THUMB_PATH_LABEL],
                     1,3,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     widgets[GW_SETT_THUMB_SIZE_LABEL],
                     2,3,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     thumb_open_btn,
                     3,3,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     thumb_clear_btn,
                     4,3,1,1);
    /* Wallpapers */
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                     //gtk_label_new ("Application data directories"),
                     0,4,4,1);
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Wallpapers:"),
                     0,5,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     widgets[GW_SETT_WALL_PATH_LABEL],
                     1,5,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     widgets[GW_SETT_WALL_SIZE_LABEL],
                     2,5,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     wall_open_btn,
                     3,5,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     wall_clear_btn,
                     4,5,1,1);
    /* Config */
    gtk_grid_attach (GTK_GRID (grid),
                     gtk_label_new ("Config files:"),
                     0,6,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     widgets[GW_SETT_CONF_PATH_LABEL],
                     1,6,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     widgets[GW_SETT_CONF_SIZE_LABEL],
                     2,6,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     conf_open_btn,
                     3,6,1,1);
    gtk_grid_attach (GTK_GRID (grid),
                     conf_clear_btn,
                     4,6,1,1);

    gtk_box_pack_start (GTK_BOX (content_box),
                        grid,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    widgets[GW_SETT_IMG_O]    = orig_image;
    widgets[GW_SETT_IMG_P]    = jpg_image;
    widgets[GW_SETT_JPG_SPIN] = jpg_spin;

    refresh_preview (widgets);

    g_signal_connect_swapped (jpg_spin, "value-changed",
                              G_CALLBACK (refresh_preview),
                              widgets);

    gtk_widget_show_all (content_box);

    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
        jpg_quality = gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON (jpg_spin));
        settings = setting_new_int (get_setting_name (SETT_THUMB_QUALITY),
                                    jpg_quality);
        setts_check_update_file (cfg_file, settings);
        settings_free_all (settings);
    }
    gtk_widget_destroy (dialog);
}
/*----------------------------------------------------------------------------*/
