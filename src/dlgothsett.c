/**
 * @file  dlgothsett.c
 * @copyright Copyright (C) 2019-2021 Michał Bąbik
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
 * @author Michal Babik <michal.babik@pm.me>
 */
#include "dlgsmsg.h"
#include "dirlist.h"
#include "dmfn.h"
#include "setting.h"
#include "setts.h"
#include "cfgfile.h"
#include "strfun.h"
#include "dlgothsett.h"
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
 * @param[in,out] gw_array  Array with widgets
 * @return        none
 */
static void
refresh_preview (GtkWidget **gw_array)
{
    GdkPixbuf *gp_src    = NULL; /* Original pixbuf */
    GdkPixbuf *gp_jpg    = NULL; /* JPG pixbuf */
    char      *s_buffer  = NULL; /* Data buffer */
    gsize      ui_bfsize = 0;    /* Size of data buffer */
    int        i_val     = 0;    /* JPG quality value */
    char       s_qual[8];        /* JPG quality value as a string */
    GInputStream *stream;        /* Stream for passing data to pixbuf */

    if (gw_array[GW_SETT_IMG_O] == NULL)
        return;

    i_val = gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON (gw_array[GW_SETT_JPG_SPIN]));

    gp_src = gtk_image_get_pixbuf (GTK_IMAGE (gw_array[GW_SETT_IMG_O]));

    if (gp_src != NULL) {
        sprintf (s_qual, "%d", i_val);
        if (gdk_pixbuf_save_to_buffer (gp_src, &s_buffer, &ui_bfsize,
                                       "jpeg", NULL,
                                       "quality", s_qual, NULL)) {
            stream = g_memory_input_stream_new ();
            g_memory_input_stream_add_data (G_MEMORY_INPUT_STREAM (stream),
                                            s_buffer, (gssize) ui_bfsize, NULL);
            gp_jpg = gdk_pixbuf_new_from_stream (stream, NULL, NULL);
            free (s_buffer);
            g_input_stream_close (stream, NULL, NULL);
            g_object_unref (stream);
            if (gp_jpg != NULL) {
                gtk_image_clear (GTK_IMAGE (gw_array[GW_SETT_IMG_P]));
                gtk_image_set_from_pixbuf (GTK_IMAGE (gw_array[GW_SETT_IMG_P]),
                                           gp_jpg);
                g_object_unref (gp_jpg);
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Deleting query cache files.
 *
 * @param[in,out] gw_array  Array with widgets
 * @return        none
 */
static void
event_delete_query_data (GtkWidget **gw_array)
{
    char       *s_size = NULL; /* String with size of dir after delete */
    const char *s_path = NULL; /* Directory path */

    s_path = gtk_label_get_text (
            GTK_LABEL (gw_array[GW_SETT_QUERY_PATH_LABEL]));
    dirlist_delete_dir_content (s_path);
    s_size = g_format_size ((guint64) dirlist_get_dir_size (s_path));
    gtk_label_set_text (GTK_LABEL (gw_array[GW_SETT_QUERY_SIZE_LABEL]), s_size);
    free (s_size);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Deleting thumbnails.
 *
 * @param[in,out] gw_array  Array with widgets
 * @return        none
 */
static void
event_delete_thumb_data (GtkWidget **gw_array)
{
    char       *s_size = NULL; /* String with size of dir after delete */
    const char *s_path = NULL; /* Directory path */

    s_path = gtk_label_get_text (
            GTK_LABEL (gw_array[GW_SETT_THUMB_PATH_LABEL]));
    dirlist_delete_dir_content (s_path);
    s_size = g_format_size ((guint64) dirlist_get_dir_size (s_path));
    gtk_label_set_text (GTK_LABEL (gw_array[GW_SETT_THUMB_SIZE_LABEL]), s_size);
    free (s_size);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Deleting downloaded wallpapers.
 *
 * @param[in,out] gw_array  Array with widgets
 * @return        none
 */
static void
event_delete_wallpapers (GtkWidget **gw_array)
{
    char       *s_size = NULL; /* String with size of dir after delete */
    const char *s_path = NULL; /* Directory path */

    if (message_dialog_question (NULL, "Do you really want to delete all "
                "downloaded wallpapers ?")) {

        s_path = gtk_label_get_text (
                GTK_LABEL (gw_array[GW_SETT_WALL_PATH_LABEL]));
        dirlist_delete_dir_content (s_path);
        s_size = g_format_size ((guint64) dirlist_get_dir_size (s_path));
        gtk_label_set_text (GTK_LABEL (gw_array[GW_SETT_WALL_SIZE_LABEL]),
                            s_size);
        free (s_size);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Deleting configuration files.
 *
 * @param[in,out] gw_array  Array with widgets
 * @return        none
 */
static void
event_delete_config_files (GtkWidget **gw_array)
{
    char       *s_size = NULL; /* String with size of dir after delete */
    const char *s_path = NULL; /* Directory path */

    if (message_dialog_question (NULL, "Do you really want to delete all "
                "applicatoin configuration files ?\nThis will result in the"
                "termination of the application.")) {

        s_path = gtk_label_get_text (
                GTK_LABEL (gw_array[GW_SETT_CONF_PATH_LABEL]));
        dirlist_delete_dir_content (s_path);
        s_size = g_format_size ((guint64) dirlist_get_dir_size (s_path));
        gtk_label_set_text (GTK_LABEL (gw_array[GW_SETT_CONF_SIZE_LABEL]),
                            s_size);
        free (s_size);
        dmfn_kill ();
        exit (EXIT_SUCCESS);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Open directory stored in given label
 *
 * @param[in] gl_label  Label with directory path
 * @return        none
 */
static void
event_open_dir_from_label (GtkLabel *gl_label)
{
    int i_res __attribute__ ((unused)) = 0; /* Result of system command for
                                               compiler complain */
    char *s_cmd = str_comb ("xdg-open ", gtk_label_get_text (gl_label));
    i_res = system(s_cmd);
    free (s_cmd);
}
/*----------------------------------------------------------------------------*/

/**
 * @brief  Dialog with other settings.
 */
void
other_settings_dialog (GtkWindow  *gw_parent,
                       const char *s_cfg_file,
                       const char *s_fn)
{
    GtkWidget *gw_array[GW_SETT_CNT]; /* Array for app widgets */
    GtkWidget *gw_dialog;           /* Dialog window */
    GtkWidget *gw_content_box;      /* Dialog's box */
    GtkWidget *gw_hbox;             /* box */
    GtkWidget *gw_grid;             /* grid */
    GtkWidget *gw_thumb_opn_btn;    /* Thumbnails open button */
    GtkWidget *gw_query_opn_btn;    /* Query open button */
    GtkWidget *gw_conf_opn_btn;     /* Config open button */
    GtkWidget *gw_wall_opn_btn;     /* Wallpapers open button */
    GtkWidget *gw_thumb_clr_btn;    /* Thumbnails clear button */
    GtkWidget *gw_query_clr_btn;    /* Query clear button */
    GtkWidget *gw_conf_clr_btn;     /* Config delete button */
    GtkWidget *gw_wall_clr_btn;     /* Wallpapers delete button */
    GtkWidget *gw_jpg_spin;         /* Spinbuton for jpg quality */
    GtkWidget *gw_image_o   = NULL; /* Original thumb image */
    GtkWidget *gw_image_p   = NULL; /* JPG thumb image */
    GdkPixbuf *gp_pbuf      = NULL; /* Pixbuf */
    GError    *g_error      = NULL; /* For error output */
    Setting   *st_sett      = NULL; /* For setting */
    Setting   *st_st        = NULL; /* For particular setting */
    char      *s_thumb_path = NULL; /* Thumbnails directory */
    char      *s_thumb_size = NULL; /* Size of thumbnails dir */
    char      *s_query_path = NULL; /* Query info directory */
    char      *s_query_size = NULL; /* Size of query dir; */
    char      *s_conf_path  = NULL; /* Config directory */
    char      *s_conf_size  = NULL; /* Size of config dir */
    char      *s_wall_path  = NULL; /* Wallpapers directory */
    char      *s_wall_size  = NULL; /* Size of wallpapers dir */
    int        i_err        = 0;    /* Error output */
    int        i_jpg_qual   = 96;   /* JPG quality val */
    GtkAdjustment *ga_adjust;       /* Adjustment for jpg quality */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    /* Get settings for dialog dimensions and info for webwidget */
    st_sett = setts_read (s_cfg_file, &i_err);
    setts_check_defaults (st_sett);

    if ((st_st = settings_find (
                    st_sett, get_setting_name (SETT_THUMB_QUALITY))) != NULL) {
        i_jpg_qual = (int) setting_get_int (st_st);
    }
    settings_free_all (st_sett);

    /* Main dialog window */
    gw_dialog = gtk_dialog_new_with_buttons (
                                      "Other settings",
                                      gw_parent,
                                      flags,
                                      "_OK",
                                      GTK_RESPONSE_ACCEPT,
                                      "_Cancel",
                                      GTK_RESPONSE_REJECT,
                                      NULL);

    gw_content_box = gtk_dialog_get_content_area (GTK_DIALOG (gw_dialog));
    gtk_container_set_border_width (GTK_CONTAINER (gw_content_box), 8);

    s_thumb_path = cfgfile_get_app_thumbnails_path ();
    s_query_path = cfgfile_get_query_path ();
    s_conf_path  = cfgfile_get_app_config_path ();
    s_wall_path  = cfgfile_get_app_wallpapers_path ();
    s_thumb_size = g_format_size ((guint64) dirlist_get_dir_size (s_thumb_path));
    s_query_size = g_format_size ((guint64) dirlist_get_dir_size (s_query_path));
    s_conf_size  = g_format_size ((guint64) dirlist_get_dir_size (s_conf_path));
    s_wall_size  = g_format_size ((guint64) dirlist_get_dir_size (s_wall_path));
    gw_array[GW_SETT_THUMB_PATH_LABEL] = gtk_label_new (s_thumb_path);
    gw_array[GW_SETT_THUMB_SIZE_LABEL] = gtk_label_new (s_thumb_size);
    gw_array[GW_SETT_QUERY_PATH_LABEL] = gtk_label_new (s_query_path);
    gw_array[GW_SETT_QUERY_SIZE_LABEL] = gtk_label_new (s_query_size);
    gw_array[GW_SETT_CONF_PATH_LABEL]  = gtk_label_new (s_conf_path);
    gw_array[GW_SETT_CONF_SIZE_LABEL]  = gtk_label_new (s_conf_size);
    gw_array[GW_SETT_WALL_PATH_LABEL]  = gtk_label_new (s_wall_path);
    gw_array[GW_SETT_WALL_SIZE_LABEL]  = gtk_label_new (s_wall_size);
    gw_thumb_opn_btn = gtk_button_new_with_label ("Open");
    gw_query_opn_btn = gtk_button_new_with_label ("Open");
    gw_conf_opn_btn  = gtk_button_new_with_label ("Open");
    gw_wall_opn_btn  = gtk_button_new_with_label ("Open");
    gw_thumb_clr_btn = gtk_button_new_with_label ("Clear");
    gw_query_clr_btn = gtk_button_new_with_label ("Clear");
    gw_conf_clr_btn  = gtk_button_new_with_label ("Delete");
    gw_wall_clr_btn  = gtk_button_new_with_label ("Delete");
    gtk_widget_set_tooltip_text (gw_thumb_opn_btn,
                                 "Open thumbnail cache directory");
    gtk_widget_set_tooltip_text (gw_query_opn_btn,
                                 "Open query info cache directory");
    gtk_widget_set_tooltip_text (gw_conf_opn_btn,
                                 "Open configuration files directory");
    gtk_widget_set_tooltip_text (gw_wall_opn_btn,
                                 "Open downloaded wallpapers directory");
    gtk_widget_set_tooltip_text (gw_thumb_clr_btn, "Clear thumbnail cache");
    gtk_widget_set_tooltip_text (gw_query_clr_btn, "Clear query info cache");
    gtk_widget_set_tooltip_text (gw_conf_clr_btn, "Delete configuration files");
    gtk_widget_set_tooltip_text (gw_wall_clr_btn,
                                 "Delete downloaded wallpapers");
    free (s_thumb_path);
    free (s_query_path);
    free (s_thumb_size);
    free (s_query_size);
    free (s_conf_path);
    free (s_conf_size);
    free (s_wall_path);
    free (s_wall_size);

    g_signal_connect_swapped (gw_thumb_opn_btn, "clicked",
                              G_CALLBACK (event_open_dir_from_label),
                                  GTK_LABEL (
                                      gw_array[GW_SETT_THUMB_PATH_LABEL]));
    g_signal_connect_swapped (gw_query_opn_btn, "clicked",
                              G_CALLBACK (event_open_dir_from_label),
                                  GTK_LABEL (
                                      gw_array[GW_SETT_QUERY_PATH_LABEL]));
    g_signal_connect_swapped (gw_conf_opn_btn, "clicked",
                              G_CALLBACK (event_open_dir_from_label),
                                  GTK_LABEL (
                                      gw_array[GW_SETT_CONF_PATH_LABEL]));
    g_signal_connect_swapped (gw_wall_opn_btn, "clicked",
                              G_CALLBACK (event_open_dir_from_label),
                                  GTK_LABEL (
                                      gw_array[GW_SETT_WALL_PATH_LABEL]));

    g_signal_connect_swapped (gw_thumb_clr_btn, "clicked",
                              G_CALLBACK (event_delete_thumb_data),
                              gw_array);
    g_signal_connect_swapped (gw_query_clr_btn, "clicked",
                              G_CALLBACK (event_delete_query_data),
                              gw_array);
    g_signal_connect_swapped (gw_wall_clr_btn, "clicked",
                              G_CALLBACK (event_delete_wallpapers),
                              gw_array);
    g_signal_connect_swapped (gw_conf_clr_btn, "clicked",
                              G_CALLBACK (event_delete_config_files),
                              gw_array);

    gw_hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    ga_adjust = gtk_adjustment_new ((gdouble) i_jpg_qual,
                                     0.0, 100.0, 1.0, 10.0, 0.0);
    gw_jpg_spin = gtk_spin_button_new (ga_adjust, 1.0, 0);
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gtk_label_new ("Cached thumbnail jpg quality:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_hbox),
                        gw_jpg_spin,
                        FALSE, FALSE, 4);
    gtk_widget_set_halign (gw_hbox, GTK_ALIGN_CENTER);

    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_hbox,
                        FALSE, FALSE, 4);

    if (s_fn != NULL) {
        gp_pbuf = gdk_pixbuf_new_from_file_at_size (s_fn, 250, 250, &g_error);
        g_clear_error (&g_error);
        if (gp_pbuf != NULL) {
            gw_image_o = gtk_image_new_from_pixbuf (gp_pbuf);
            gw_image_p = gtk_image_new_from_pixbuf (gp_pbuf);
            g_object_unref (gp_pbuf);

            gw_grid = gtk_grid_new ();
            gtk_grid_set_row_spacing    (GTK_GRID (gw_grid), 8);
            gtk_grid_set_column_spacing (GTK_GRID (gw_grid), 8);
            gtk_widget_set_halign (gw_grid, GTK_ALIGN_CENTER);
            gtk_grid_attach (GTK_GRID (gw_grid),
                             gtk_label_new ("Original:"),
                             0,0,1,1);
            gtk_grid_attach (GTK_GRID (gw_grid),
                             gtk_label_new ("JPG Preview:"),
                             1,0,1,1);
            gtk_grid_attach (GTK_GRID (gw_grid),
                             gw_image_o,
                             0,1,1,1);
            gtk_grid_attach (GTK_GRID (gw_grid),
                             gw_image_p,
                             1,1,1,1);
            gtk_box_pack_start (GTK_BOX (gw_content_box),
                                gw_grid,
                                FALSE, FALSE, 4);
        }
        gtk_box_pack_start (GTK_BOX (gw_content_box),
                            gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                            FALSE, FALSE, 4);
    }

    /* Cache directories paths and sized */
    gw_grid = gtk_grid_new ();
    gtk_widget_set_halign (gw_grid, GTK_ALIGN_CENTER);
    gtk_grid_set_row_spacing    (GTK_GRID (gw_grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (gw_grid), 8);
    /* Title */
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gtk_label_new ("Application directories"),
                     0,0,4,1);
    /* Query cache */
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gtk_label_new ("Query cache:"),
                     0,2,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_array[GW_SETT_QUERY_PATH_LABEL],
                     1,2,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_array[GW_SETT_QUERY_SIZE_LABEL],
                     2,2,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_query_opn_btn,
                     3,2,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_query_clr_btn,
                     4,2,1,1);
    /* Thumbnail cache */
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gtk_label_new ("Thumbnails:"),
                     0,3,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_array[GW_SETT_THUMB_PATH_LABEL],
                     1,3,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_array[GW_SETT_THUMB_SIZE_LABEL],
                     2,3,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_thumb_opn_btn,
                     3,3,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_thumb_clr_btn,
                     4,3,1,1);
    /* Wallpapers */
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                     //gtk_label_new ("Application data directories"),
                     0,4,4,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gtk_label_new ("Wallpapers:"),
                     0,5,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_array[GW_SETT_WALL_PATH_LABEL],
                     1,5,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_array[GW_SETT_WALL_SIZE_LABEL],
                     2,5,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_wall_opn_btn,
                     3,5,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_wall_clr_btn,
                     4,5,1,1);
    /* Config */
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gtk_label_new ("Config files:"),
                     0,6,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_array[GW_SETT_CONF_PATH_LABEL],
                     1,6,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_array[GW_SETT_CONF_SIZE_LABEL],
                     2,6,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_conf_opn_btn,
                     3,6,1,1);
    gtk_grid_attach (GTK_GRID (gw_grid),
                     gw_conf_clr_btn,
                     4,6,1,1);

    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_grid,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    gw_array[GW_SETT_IMG_O]    = gw_image_o;
    gw_array[GW_SETT_IMG_P]    = gw_image_p;
    gw_array[GW_SETT_JPG_SPIN] = gw_jpg_spin;

    refresh_preview (gw_array);

    g_signal_connect_swapped (gw_jpg_spin, "value-changed",
                              G_CALLBACK (refresh_preview),
                              gw_array);

    gtk_widget_show_all (gw_content_box);

    if (gtk_dialog_run (GTK_DIALOG (gw_dialog)) == GTK_RESPONSE_ACCEPT) {
        i_jpg_qual = gtk_spin_button_get_value_as_int (
            GTK_SPIN_BUTTON (gw_jpg_spin));
        st_sett = setting_new_int (get_setting_name (SETT_THUMB_QUALITY),
                                    i_jpg_qual);
        setts_check_update_file (s_cfg_file, st_sett);
        settings_free_all (st_sett);
    }
    gtk_widget_destroy (gw_dialog);
}
/*----------------------------------------------------------------------------*/
