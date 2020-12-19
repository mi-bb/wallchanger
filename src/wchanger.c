/**
 * @file  wchanger.c
 * @copyright Copyright (C) 2019-2020 Michał Bąbik
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
 * @brief Wallpaper change dialog
 *
 * Automatic wallpaper changer
 *
 * @date December 19, 2020
 *
 * @version 1.6.15
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdio.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include "dialogdata.h"
#include "setts.h"
#include "wpset.h"
#include "imgs.h"
#include "dlgs.h"
#include "dlgsmsg.h"
#include "dlgcmd.h"
#include "dlgothsett.h"
#include "treev.h"
#include "dirlist.h"
#include "fdfn.h"
#include "preview.h"
#include "strfun.h"
#include "errs.h"
#include "cfgfile.h"
#include "defs.h"
#include "hashfun.h"
#include "dmfn.h"
#include "wmsfn.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static uint32_t get_wallpaper_ch_interval (const DialogData *dd_data)
 *
 * @brief  Get wallpaper change minutes interval from widgets
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        Change interval value
 *
 * @fn  static void set_wallpaper_ch_interval (const DialogData *dd_data,
 *                                             const uint32_t    i_val)
 *
 * @brief  Set wallpaper change interval value to widgets
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @param[in]     i_val    Change interval value to set
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static uint32_t    get_wallpaper_ch_interval   (const DialogData  *dd_data);

static void        set_wallpaper_ch_interval   (const DialogData  *dd_data,
                                                const uint32_t     i_val);
/*----------------------------------------------------------------------------*/
/**
 * @fn  static Setting * widgets_get_settings (const DialogData  *dd_data)
 *
 * @brief  Read settings from widgets and store them in WallSett object.
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        List of settings
 *
 * @fn  static void widgets_set_settings (const DialogData  *dd_data,
 *                                        Setting           *st_settings,
 *                                        Setting           *st_wmlist,
 *                                        int               *i_err)
 *
 * @brief  Loading data from list of settings to program window.
 *
 * @param[in,out] dd_data      DialogData object with settings and widget data
 * @param[in]     st_settings  List with settings
 * @param[in]     st_wmlist    List with window manager info
 * @param[out]    i_err        Error output
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static Setting   * widgets_get_settings        (const DialogData  *dd_data);

static void        widgets_set_settings        (const DialogData  *dd_data,
                                                Setting           *st_settings,
                                                Setting           *st_wmlist,
                                                int               *i_err);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets statusbar info about actual config file.
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 */
static void        statusbar_push_config_info  (const DialogData *dd_data);
/*----------------------------------------------------------------------------*/
/**
 * @fn  static gboolean event_on_delete (GtkWidget        *window,
 *                                       GdkEvent         *event,
 *                                       const DialogData *dd_data)
 *
 * @brief  On main window delete, check settings.
 *
 * @param[in] window   Window that received the signal
 * @param[in] event    Event that triggered signal
 * @param[in] dd_data  DialogData object with widgets and settings info
 * @return    Stop or propagate event further 
 *
 * @fn  static void shutdown (GtkApplication *application, DialogData *dd_data);
 *
 * @brief  Application shutdown signal.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @param[in,out] dd_data      DialogData object with widgets and settings info
 * @return        none
 *
 * @fn  static gint local_options (GApplication *application,
 *                                 GVariantDict *options,
 *                                 DialogData   *dd_data)
 *
 * @brief  Handling command line options.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @param[in]     options      Options dictionary
 * @param[in,out] dd_data      DialogData object with widgets and settings info
 *
 * @fn  static void activate (GtkApplication *app, DialogData *dd_data)
 *
 * @brief  Application activate signal.
 *
 * @param[in,out] app      Pointer to GtkApplication
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static gboolean    event_on_delete             (GtkWidget         *window,
                                                GdkEvent          *event,
                                                const DialogData  *dd_data);

static void        shutdown                    (GtkApplication    *application,
                                                DialogData        *dd_data);

static gint        local_options               (GApplication      *application,
                                                GVariantDict      *options,
                                                DialogData        *dd_data);

static void        activate                    (GtkApplication    *app,
                                                DialogData        *dd_data);
/*----------------------------------------------------------------------------*/
/**
 * @fn  static void event_add_img_pressed (const DialogData *dd_data)
 *
 * @brief  "Add images" button pressed.
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void event_add_img_dir_pressed (const DialogData *dd_data)
 *
 * @brief  "Add images from folder" button pressed.
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void event_add_img_web_pressed (const DialogData *dd_data)
 *
 * @brief  "Add images from web" button pressed.
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void event_set_wallpaper_pressed (const DialogData *dd_data)
 *
 * @brief  "Set wallpaper" button pressed.
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void event_save_settings_pressed (const DialogData *dd_data)
 *
 * @brief  "Save settings" button pressed.
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return none
 *
 * @fn  static void event_img_list_activated (GtkTreeView       *tree_view,
 *                                            GtkTreePath       *path,
 *                                            GtkTreeViewColumn *column,
 *                                            GtkWidget         *gw_img_prev)
 *
 * @brief  Make preview image widget of image (file path).
 *
 * @param[in]  tree_view    The object on which the signal is emitted
 * @param[in]  path         The GtkTreePath for the activated row
 * @param      column       The GtkTreeViewColumn in which the activation
 *                          occurred
 * @param[out] gw_img_prev  Preview image widget
 * @return none
 *
 * @fn  static gboolean event_treeview_key_press (GtkWidget         *widget,
 *                                                GdkEventKey       *event,
 *                                                const DialogData  *dd_data)
 * @brief  React to key pressed in TreeView.
 *
 * @param[in]     widget   The object which received the signal
 * @param[in]     event    The event which triggered this signal
 * @param[in,out] dd_data  DialogData object with widgets and settings info
 *
 * @fn  static void event_autostart_toggled (GtkToggleButton   *togglebutton,
 *                                           gpointer           user_data)
 *
 * @brief  Changed active state of autostart checkbox
 *
 * @param[in] togglebutton  Togglebutton which changed state
 * @param[in] user_data     Data passed to function
 * @return    none
 */
/*----------------------------------------------------------------------------*/
static void        event_add_img_pressed       (const DialogData  *dd_data);

static void        event_add_img_dir_pressed   (const DialogData  *dd_data);

static void        event_add_img_web_pressed   (const DialogData  *dd_data);

static void        event_set_wallpaper_pressed (const DialogData  *dd_data);

static void        event_save_settings_pressed (const DialogData  *dd_data);

static void        event_img_list_activated    (GtkTreeView       *tree_view,
                                                GtkTreePath       *path,
                                                GtkTreeViewColumn *column,
                                                GtkWidget         *gw_img_prev);

static gboolean    event_treeview_key_press    (GtkWidget         *widget,
                                                GdkEventKey       *event,
                                                const DialogData  *dd_data);

static void        event_autostart_toggled     (GtkToggleButton   *togglebutton,
                                                gpointer           user_data);
/*----------------------------------------------------------------------------*/
/**
 * @fn  static void event_interval_changed (GtkSpinButton    *spin_button,
 *                                          const DialogData *dd_data)
 *
 * @brief  Wallpaper change interval changed
 *
 * @param[in]     spin_button  Spin button with interval value
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void event_command_button_pressed (const DialogData *dd_data)
 *
 * @brief  Command select button pressed.
 *
 * @param[in,out] dd_data  DialogData object with widgets and settings info
 * @return        none
 *
 * @fn  static void event_start_daemon_pressed (DialogData *dd_data)
 *
 * @brief  Start background daemon process
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void event_stop_daemon_pressed (DialogData *dd_data)
 *
 * @brief  Stop background daemon process
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void event_other_settings_pressed (DialogData *dd_data)
 *
 * @brief  Other settings button pressed.
 *
 * @param[in,out] dd_data  DialogData object with widgets and settings info
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static void        event_interval_changed       (GtkSpinButton     *spin_button,
                                                 const DialogData  *dd_data);

static void        event_command_button_pressed (const DialogData  *dd_data);

static void        event_start_daemon_pressed   (DialogData        *dd_data);

static void        event_stop_daemon_pressed    (DialogData        *dd_data);

static void        event_other_settings_pressed (DialogData        *dd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Monitors running of wchangerd daemon.
 *
 * @param[out]  data  DialogData object with widgets and settings info
 * @return      Remove source or not
 */
static gboolean    daemon_monitor              (gpointer data);
/*----------------------------------------------------------------------------*/
/**
 * @fn static GtkWidget * create_title_widget (void)
 *
 * @brief  Create top window title text.
 *
 * @return     Result widget
 *
 * @fn  static GtkWidget * create_image_button (const char   *s_label,
 *                                              const char   *s_hint,
 *                                              const IconImg i_but)
 * @brief  Create button with icon/label/hint.
 *
 * @param[in] s_label  Button label
 * @param[in] s_hint   Button hint
 * @param[in] i_but    Icon number
 * @return    Button
 *
 * @fn  static GtkWidget * create_buttons_widget (DialogData *dd_data)
 *
 * @brief  Create side buttons widget.
 *
 * @param[in,out] dd_data    DialogData object with widgets and settings info
 * @return        Result widget
 *
 * @fn  static GtkWidget * create_settings_widget (DialogData *dd_data)
 *
 * @brief  Creates widget with settings for wallpaper changing.
 *
 * @param[in,out] dd_data    DialogData object with settings and widget data
 * @return        Settings widget
 *
 * @fn  static GtkWidget * create_daemon_widget (DialogData *dd_data)
 *
 * @brief  Creates widget for monitoring, starting and stopping wchangerd
 *
 * @param[in,out] dd_data    DialogData object with settings and widget data
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static GtkWidget * create_title_widget         (void);

static GtkWidget * create_image_button         (const char        *s_label,
                                                const char        *s_hint,
                                                const IconImg      i_but);

static GtkWidget * create_buttons_widget       (DialogData        *dd_data);

static GtkWidget * create_settings_widget      (DialogData        *dd_data);

static GtkWidget * create_daemon_widget        (DialogData        *dd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create application directories, return error message on error.
 *
 * @return String with error info or null if there was no error.
 *         After use, it should be freed using free.
 */
static char *
cfgfile_check_create_dirs (void)
{
    char *s_ret = NULL;
    int   i_err = ERR_OK;
    int   i     = 0;
    char *s_dirs[3];

    s_dirs[0] = cfgfile_get_query_path ();
    s_dirs[1] = cfgfile_get_app_thumbnails_path ();
    s_dirs[2] = cfgfile_get_app_wallpapers_path ();

    for (i = 0; i < 3; ++i) {
        if ((i_err = dir_create_with_subdirs (s_dirs[i])) != ERR_OK) {
            s_ret = str_comb (s_dirs[i], "\n");
            str_append (&s_ret, err_get_message (i_err));
            break;
        }
    }
    for (i = 0; i < 3; ++i) {
        free (s_dirs[i]);
    }
    return s_ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper change minutes interval from widgets
 */
static uint32_t
get_wallpaper_ch_interval (const DialogData *dd_data)
{
    uint32_t ui_res = 0; /* Wallpaper change interval in minutes */

    /* Get spinbutton value of change time interval */
    ui_res = (uint32_t) gtk_spin_button_get_value (
            GTK_SPIN_BUTTON (dd_data->gw_interval));

    /* Check combobox setting if value is supposed to be minutes or hours.
     * If value is set to hours multiply returned value by 60 */
    if (gtk_combo_box_get_active (GTK_COMBO_BOX (dd_data->gw_inter_combo)) == 1)
        ui_res *= 60;

    return ui_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set wallpaper change interval value to widgets
 */
static void
set_wallpaper_ch_interval (const DialogData *dd_data,
                           const uint32_t    i_val)
{
    uint32_t ui_tmp = i_val; /* Temp inteval value */

    /* Check if minutes value can be displayed as hours */
    if ((ui_tmp / 60 >= 1) && (ui_tmp % 60 == 0)) {

        /* If value can be hours set combobox to hours */
        gtk_combo_box_set_active (GTK_COMBO_BOX (dd_data->gw_inter_combo), 1);
        ui_tmp /= 60;
    }
    else {
        /* Value is suppose to be set as minutes set combobox to minutes */
        gtk_combo_box_set_active (GTK_COMBO_BOX (dd_data->gw_inter_combo), 0);
    }
    /* Set spinbutton value with change interval */
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (dd_data->gw_interval),
                               (double) ui_tmp);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read settings from widgets and store them in WallSett object.
 */
static Setting *
widgets_get_settings (const DialogData *dd_data)
{
    Setting    *st_setts = NULL; /* Setting item to return */
    const char *s_val    = NULL; /* Value for a string setting */
    int64_t     i_val    = 0;    /* Value for an integer setting */

    /* Get wallpaper list from treeview and add to settlist */
    st_setts = setting_new_array (get_setting_name (SETT_WALL_ARRAY));
    setting_add_child (st_setts, treeview_get_setting_data (dd_data->gw_view));

    /* Get random wallpaper setting */
    i_val = (int64_t) gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (dd_data->gw_random));
    settings_append (st_setts,
            setting_new_int (get_setting_name (SETT_RANDOM_OPT), i_val));

    /* Get last used wallpaper on start setting */
    i_val = (int64_t) gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (dd_data->gw_lastused));
    settings_append (st_setts,
            setting_new_int (get_setting_name (SETT_LAST_USED_OPT), i_val));

    /* Get time align setting */
    i_val = (int64_t) gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (dd_data->gw_timealign));
    settings_append (st_setts,
            setting_new_int (get_setting_name (SETT_TIME_ALIGN_OPT), i_val));

    /* Get wallpaper change interval setting */
    i_val = (int64_t) get_wallpaper_ch_interval (dd_data);
    settings_append (st_setts,
            setting_new_int (get_setting_name (SETT_INTERVAL_VAL), i_val));

    /* Get wallpaper set command */
    s_val = gtk_entry_get_text (GTK_ENTRY (dd_data->gw_command));
    settings_append (st_setts,
            setting_new_string (get_setting_name (SETT_BG_CMD), s_val));

    return st_setts;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Loading data from SettList list of settings to program window.
 */
static void
widgets_set_settings (const DialogData *dd_data,
                      Setting          *st_setts,
                      Setting          *st_wmlist,
                      int              *i_err)
{
    Setting    *st_item    = NULL; /* Setting to read */
    int         i_w        = 0;    /* Window width value */
    int         i_h        = 0;    /* Window height value */
    const char *s_lastused = NULL; /* Last used wallpapet string */
    char       *s_cmd      = NULL; /* Wallpaper set command */

    *i_err = ERR_OK;

    /* Check window managers and set wallpaper set command */
    s_cmd = wms_get_wallpaper_command (dd_data->s_cfg_file,
                                       st_setts,
                                       st_wmlist,
                                       i_err);
    if (*i_err != ERR_OK) {
        free (s_cmd);
        return;
    }
    gtk_entry_set_text (GTK_ENTRY (dd_data->gw_command), s_cmd);
    free (s_cmd);

    /* Set last used wallpaper on start setting */
    st_item = settings_find (st_setts, get_setting_name (SETT_LAST_USED_OPT));
    if (st_item != NULL) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_lastused),
                                      (gboolean) setting_get_int (st_item));
    }
    /* Set minutes/hours of wallpaper change interval */
    st_item = settings_find (st_setts, get_setting_name (SETT_INTERVAL_VAL));
    if (st_item != NULL) {
        set_wallpaper_ch_interval (dd_data,
                                   (uint32_t) setting_get_int (st_item));
    }
    /* Set wallpaper random choose setting */
    st_item = settings_find (st_setts, get_setting_name (SETT_RANDOM_OPT));
    if (st_item != NULL) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_random),
                                      (gboolean) setting_get_int (st_item));
    }
    /* Set time align setting */
    st_item = settings_find (st_setts, get_setting_name (SETT_TIME_ALIGN_OPT));
    if (st_item != NULL) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_timealign),
                                      (gboolean) setting_get_int (st_item));
    }
    /* Get wallpaper list and add to treeview */
    st_item = settings_find (st_setts, get_setting_name (SETT_WALL_ARRAY));
    if (st_item != NULL) {
        treeview_add_items_setting (dd_data->gw_view,
                                    setting_get_child (st_item));
    }
    /* Set window dimensions */
    st_item = settings_find (st_setts, get_setting_name (SETT_WIN_WIDTH));
    if (st_item != NULL) {
        i_w = (int) setting_get_int (st_item);
    }
    st_item = settings_find (st_setts, get_setting_name (SETT_WIN_HEIGHT));
    if (st_item != NULL) {
        i_h = (int) setting_get_int (st_item);
    }
    gtk_window_set_default_size (GTK_WINDOW (dd_data->gw_window), i_w, i_h);

    /* Check if there is last used wallpaper info, if it is select it on
     * list and make a preview image */
    st_item = settings_find (st_setts, get_setting_name (SETT_LAST_USED_STR));
    if (st_item != NULL)
        s_lastused = setting_get_string (st_item);

    if (s_lastused != NULL) {
        preview_from_file (dd_data->gw_imgprev, s_lastused);
        treeview_find_select_item (dd_data->gw_view, s_lastused);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets statusbar info about actual config file.
 */
static void
statusbar_push_config_info (const DialogData *dd_data)
{
    char *s_info = NULL;

    s_info = dialogdata_get_status_config_info (dd_data);

    gtk_statusbar_push (GTK_STATUSBAR (dd_data->gw_statusbar),
            gtk_statusbar_get_context_id (GTK_STATUSBAR (dd_data->gw_statusbar),
                                          STATUS_CONFIG),
            s_info);
    free (s_info);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Add images" button pressed.
 */
static void
event_add_img_pressed (const DialogData *dd_data)
{
    GSList *gsl_files = NULL; /* List with files from dialog */

    /* Run image select dialog and get selected files */
    if ((gsl_files = add_images_dialog (dd_data->gw_window)) != NULL) {
        /* Add items to TreeView */
        treeview_add_items_gslist (dd_data->gw_view, gsl_files);
        g_slist_free_full (gsl_files, g_free);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Add images from folder" button pressed.
 */
static void
event_add_img_dir_pressed (const DialogData *dd_data)
{
    char  *s_folder = NULL;  /* Selecred directory name */
    GList *gl_files = NULL;  /* Images in directory */

    /* Run directory select dialog and get selected directory name */
    if ((s_folder = add_images_folder_dialog (dd_data->gw_window)) != NULL) {
        /* Scan directory for files and append them to file list */
        if ((gl_files = get_dir_content_filter_images (s_folder)) != NULL) {
            /* Add items to TreeView */
            treeview_add_items_glist (dd_data->gw_view, gl_files);
            g_list_free_full (gl_files, g_free);
        }
        g_free (s_folder);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Add images from web" button pressed.
 */
static void
event_add_img_web_pressed (const DialogData *dd_data)
{
    GList *gl_list = NULL; /* List with wallpapers to add */

    gl_list = add_images_from_web_dilaog (dd_data->gw_window,
                                          dd_data->s_cfg_file);
    treeview_add_items_glist (dd_data->gw_view, gl_list);
    g_list_free_full (gl_list, (GDestroyNotify) free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Set wallpaper" button pressed.
 */
static void
event_set_wallpaper_pressed (const DialogData *dd_data)
{
    GtkTreeSelection *gts_sele;
    GtkTreeModel     *gtm_model;
    GtkTreeIter       gti_iter;
    ImageInfo        *ii_info;
    GList            *gl_list   = NULL;
    const char       *s_cmd     = NULL;
    int               i_err     = 0;

    gtm_model = gtk_tree_view_get_model (GTK_TREE_VIEW (dd_data->gw_view));
    gts_sele  = gtk_tree_view_get_selection (GTK_TREE_VIEW (dd_data->gw_view));
    gl_list   = gtk_tree_selection_get_selected_rows (gts_sele, &gtm_model);

    if (gl_list == NULL)
        return;

    if (gtk_tree_model_get_iter (gtm_model, &gti_iter, gl_list->data)) {

        ii_info = treemodel_get_data (gtm_model, gti_iter);
        s_cmd   = gtk_entry_get_text (GTK_ENTRY (dd_data->gw_command));
        i_err   = wallpaper_dialog_set (s_cmd,
                                        imageinfo_get_file_path (ii_info),
                                        dialogdata_get_cfg_file (dd_data));
        if (i_err != ERR_OK) {
            message_dialog_error (dd_data->gw_window, err_get_message (i_err));
        }
        imageinfo_free (ii_info);
    }
    g_list_free_full (gl_list, (GDestroyNotify) gtk_tree_path_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Save settings" button pressed.
 */
static void
event_save_settings_pressed (const DialogData *dd_data)
{
    Setting *st_settings; /* List of settings */
    int      i_err = 0;   /* Error value */

    st_settings = widgets_get_settings (dd_data);
    i_err = setts_check_update_file (dialogdata_get_cfg_file (dd_data),
                                     st_settings);
    settings_free_all (st_settings);

    if (i_err != ERR_OK) {
        message_dialog_error (dd_data->gw_window, err_get_message (i_err));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make preview image widget of image (file path).
 */
static void
event_img_list_activated (GtkTreeView       *tree_view,
                          GtkTreePath       *path,
                          GtkTreeViewColumn *column __attribute__ ((unused)),
                          GtkWidget         *gw_img_prev)
{
    GtkTreeModel *gtm_model;
    ImageInfo    *ii_info;
    GtkTreeIter   gti_iter;

    gtm_model = gtk_tree_view_get_model (tree_view);

    if (gtk_tree_model_get_iter (gtm_model, &gti_iter, path)) {

        ii_info = treemodel_get_data (gtm_model, gti_iter);

        preview_from_file (gw_img_prev, imageinfo_get_file_path (ii_info));
        imageinfo_free (ii_info);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  React to key pressed in TreeView.
 */
static gboolean
event_treeview_key_press (GtkWidget        *widget __attribute__ ((unused)),
                          GdkEventKey      *event,
                          const DialogData *dd_data)
{
    /* Catch Del key in TreeView and delete item on list */
    if (event->keyval == GDK_KEY_Delete) {
        treeview_remove_selected (dd_data->gw_view);
    }
    return FALSE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Interval spin changed
 */
static void
event_interval_changed (GtkSpinButton    *spin_button,
                        const DialogData *dd_data)
{
    static int    i_prev   = 0;    /* Previous interval value */
    GtkTreeModel *gtm_model;       /* TreeModel */
    GtkComboBox  *gcb_box;         /* ComboBox */
    GtkTreeIter   gti_iter;        /* TreeIter */
    char         *s_min    = NULL; /* String for minute entry */
    char         *s_hour   = NULL; /* String for hour entry */
    int           i_val    = 0;    /* Actual interval value */
    int           i_id     = 0;    /* id position */
    int           i_en     = 0;    /* entry position */
    int           i_change = 0;    /* Change text or not */

    gcb_box = GTK_COMBO_BOX (dd_data->gw_inter_combo);
    s_min   = strdup ("minute");
    s_hour  = strdup ("hour");
    i_val   = gtk_spin_button_get_value_as_int (spin_button);

    if (i_prev == 1 && i_val != 1) {
        i_change = 1;
        str_append (&s_min,  "s");
        str_append (&s_hour, "s");
    }
    else if (i_prev != 1 && i_val == 1) {
        i_change = 1;
    }
    if (i_change) {
        gtm_model = gtk_combo_box_get_model (gcb_box);
        i_id      = gtk_combo_box_get_id_column (gcb_box);
        i_en      = gtk_combo_box_get_entry_text_column (gcb_box);

        if (gtk_tree_model_get_iter_first (gtm_model, &gti_iter)) {
            gtk_list_store_set (GTK_LIST_STORE (gtm_model), &gti_iter,
                        i_id, s_min,
                        i_en, s_min,
                        -1);
            if (gtk_tree_model_iter_next (gtm_model, &gti_iter)) {
                gtk_list_store_set (GTK_LIST_STORE (gtm_model), &gti_iter,
                        i_id, s_hour,
                        i_en, s_hour,
                        -1);
            }
        }
    }
    free (s_min);
    free (s_hour);
    i_prev = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Command select button pressed.
 */
static void
event_command_button_pressed (const DialogData *dd_data)
{
    GSList *gsl_files = NULL; /* For wallpaers */
    char   *s_command = NULL; /* Command from dialog */

    gsl_files = treeview_get_data (dd_data->gw_view);
    s_command = cmddialog_run (dd_data->gw_window,
            gtk_entry_get_text (GTK_ENTRY (dd_data->gw_command)), gsl_files);

    if (s_command != NULL) {
        gtk_entry_set_text (GTK_ENTRY (dd_data->gw_command), s_command);
        free (s_command);
    }
    g_slist_free_full (gsl_files, (GDestroyNotify) imageinfo_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed active state of autostart checkbox
 */
static void
event_autostart_toggled (GtkToggleButton *togglebutton,
                         gpointer         user_data __attribute__ ((unused)))
{
    gtk_toggle_button_get_active (togglebutton) ?
        cfgfile_autostart_create () :
        cfgfile_autostart_remove ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  On main window delete, check settings.
 */
static gboolean
event_on_delete (GtkWidget        *window,
                 GdkEvent         *event __attribute__ ((unused)),
                 const DialogData *dd_data)
{
    GtkWidget *dialog;        /* Question dialog */
    Setting   *st_settings;   /* Program settings */
    char      *s_buff = NULL; /* Buffer for settings data */
    int        i_res  = 0;    /* Dialog result */
    int        i_err  = 0;    /* Error value */
    int        i_w    = 0;    /* Window width */
    int        i_h    = 0;    /* Window height */

    gtk_window_get_size (GTK_WINDOW (window), &i_w, &i_h);

    st_settings = widgets_get_settings (dd_data);

    s_buff  = setts_check_update (dialogdata_get_cfg_file (dd_data),
                                  st_settings,
                                  &i_err);
    settings_free_all (st_settings);

    if (i_err != ERR_OK) {
        message_dialog_error (GTK_WINDOW (window), err_get_message (i_err));
    }
    if (s_buff != NULL) {
        dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_QUESTION,
                                         GTK_BUTTONS_YES_NO,
            "Settings changed, do you want to save them ?");
        gtk_window_set_title (GTK_WINDOW (dialog), "Settings changed");

        i_res = gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

        if (i_res == GTK_RESPONSE_YES) {
            i_err = setts_update_file (dialogdata_get_cfg_file (dd_data),
                                       s_buff);
            if (i_err != ERR_OK) {
                message_dialog_error (GTK_WINDOW (window),
                                      err_get_message (i_err));
            }
        }
        free (s_buff);
    }
    setts_update_window_size (dialogdata_get_cfg_file (dd_data), i_w, i_h);
    return FALSE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Start background daemon process
 */
static void
event_start_daemon_pressed (DialogData *dd_data)
{
    dmfn_start ();
    daemon_monitor (dd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Stop background daemon process
 */
static void
event_stop_daemon_pressed (DialogData *dd_data)
{
    dmfn_kill ();
    daemon_monitor (dd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Other settings button pressed.
 */
static void
event_other_settings_pressed (DialogData *dd_data)
{
    char *s_fn = treeview_get_one_file (dd_data->gw_view);
    other_settings_dialog (dd_data->gw_window, dd_data->s_cfg_file, s_fn);
    free (s_fn);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Monitors running of wchangerd daemon.
 */
static gboolean
daemon_monitor (gpointer data)
{
    DialogData *dd_data = (DialogData *) data;

    if (dmfn_check_presence ()) {
        gtk_label_set_markup (GTK_LABEL (dd_data->gw_dm_label), 
                "<span weight=\"bold\" foreground=\"#009900\" "
                "style=\"italic\">running</span>");
    }
    else {
        gtk_label_set_markup (GTK_LABEL (dd_data->gw_dm_label), 
                "<span weight=\"bold\" foreground=\"#000000\" "
                "style=\"italic\">stopped</span>");
    }
    return G_SOURCE_CONTINUE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create top window title text.
 */
static GtkWidget *
create_title_widget (void)
{
    GtkWidget  *gw_widget; /* Widget to return */
    const char *s_str    = APP_NAME " - Program settings";
    const char *s_format = "<span size=\"20000\" weight=\"bold\" "
                           "foreground=\"#0099e6\" style=\"italic\">%s</span>";
    char       *s_markup = NULL;
    GtkWidget  *gw_label = gtk_label_new (NULL);

    gw_widget = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);

    s_markup = g_markup_printf_escaped (s_format, s_str);
    gtk_label_set_markup (GTK_LABEL (gw_label), s_markup);
    g_free (s_markup);

    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_label,
                        FALSE, FALSE, 24);
    return gw_widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create button with icon/label/hint.
 */
static GtkWidget *
create_image_button (const char   *s_label,
                     const char   *s_hint,
                     const IconImg i_but)
{
    GtkWidget *gw_btn;         /* Return button with image */
    GtkWidget *gw_img;         /* Image widget to put in button */
    GdkPixbuf *gd_pix = NULL;  /* Pixbuf for image data */

    gw_btn = gtk_button_new ();

    if (!str_is_empty (s_label))
        gtk_button_set_label (GTK_BUTTON (gw_btn), s_label);

    if (!str_is_empty (s_hint))
        gtk_widget_set_tooltip_text (gw_btn, s_hint);

    if (i_but < W_IMG_COUNT && (gd_pix = get_image (i_but)) != NULL) {
        gw_img = gtk_image_new_from_pixbuf (gd_pix);
        gtk_button_set_image (GTK_BUTTON (gw_btn), gw_img);
        g_object_unref (gd_pix);
    }
    return gw_btn;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create side buttons widget.
 */
static GtkWidget *
create_buttons_widget (DialogData  *dd_data)
{
    GtkWidget *gw_widget;
    GtkWidget *gw_button;

    gw_widget = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);

    gw_button = create_image_button (NULL, "Add images", W_ICON_ADD);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (event_add_img_pressed),
                              dd_data);
    gw_button = create_image_button (NULL, "Add images from folder",
                                     W_ICON_ADD_DIR);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (event_add_img_dir_pressed),
                              dd_data);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    gw_button = create_image_button (NULL, "Add wallpapers from web",
                                     W_ICON_ADD_WEB);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (event_add_img_web_pressed),
                              dd_data);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    gw_button = create_image_button (NULL, "Remove selected images",
                                     W_ICON_REMOVE);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (treeview_remove_selected),
                              dd_data->gw_view);
    gw_button = create_image_button (NULL, "Move up", W_ICON_UP);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (treeview_move_up),
                              dd_data->gw_view);
    gw_button = create_image_button (NULL, "Move down", W_ICON_DOWN);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (treeview_move_down),
                              dd_data->gw_view);
    gw_button = create_image_button (NULL, "Remove duplicates", W_ICON_DUPL);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (treeview_remove_duplicates),
                              dd_data->gw_view);
    gw_button = create_image_button (NULL, "Set wallpaper", W_ICON_SCREEN);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (event_set_wallpaper_pressed),
                              dd_data);
    gw_button = create_image_button (NULL, "Save settings", W_ICON_FLOPPY);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (event_save_settings_pressed),
                              dd_data);
    gw_button = create_image_button (NULL, "Other settings", W_ICON_SETTING);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (event_other_settings_pressed),
                              dd_data);
    gw_button = create_image_button (NULL, "About Wall Changer", W_ICON_INFO);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (about_app_dialog),
                              GTK_WINDOW (dd_data->gw_window));
    gw_button = create_image_button (NULL, "Exit configuration dialog",
                                     W_ICON_EXIT);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (gtk_window_close),
                              dd_data->gw_window);
    gtk_box_pack_start (GTK_BOX (gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    return gw_widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates widget with settings for wallpaper changing.
 */
static GtkWidget *
create_settings_widget (DialogData *dd_data)
{
    GtkWidget     *gw_widget;            /* Widget to return */
    GtkWidget     *gw_random_button;     /* Random image GtkCheckButton */
    GtkWidget     *gw_button_selectlast; /* Set last used GtkCheckButton */
    GtkWidget     *gw_align_button;      /* Time align GtkCheckButton */
    GtkWidget     *gw_command_label;     /* Wallpaper set cmd GtkLabel */
    GtkWidget     *gw_command_button;    /* Button for command dialog */
    GtkWidget     *gw_command_entry;     /* Wallpaper set cmd GtkEntry */
    GtkWidget     *gw_interval_label;    /* Time interval GtkLabel */
    GtkWidget     *gw_spinbutton;        /* Time interval GtkSpinButton */
    GtkAdjustment *ga_adjustment;        /* Time interval GtkAdjustment */
    GtkWidget     *gw_time_combo;        /* Time interval GtkComboBoxText*/
    GtkWidget     *gw_autostart_button;  /* Autostart file GtkCheckButton */

    /* Random wallpaper change button */
    gw_random_button = gtk_check_button_new ();
    gtk_widget_set_tooltip_markup (gw_random_button, 
        "When <b>enabled</b> wallpaper images will be selected randomly.\n"
        "When <b>disabled</b> wallpapers will be set in the same order as "
        "they appear on the list");
    gtk_button_set_label (GTK_BUTTON (gw_random_button),
                          "Random wallpaper change");

    /* Select last used wallpaper button */
    gw_button_selectlast = gtk_check_button_new ();
    gtk_widget_set_tooltip_markup (gw_button_selectlast,
        "When <b>enabled</b> first wallpaper set on program start will "
        " be the last one previously set.\n"
        "When <b>disabled</b> wallpaper will be set randomly or next in list "
        "to the last one before stop.");
    gtk_button_set_label (GTK_BUTTON (gw_button_selectlast),
                          "Select last used wallpaper at start");

    /* Time align button */
    gw_align_button = gtk_check_button_new ();
    gtk_widget_set_tooltip_markup (gw_align_button, 
        "This option enables time align, when <b>enabled</b> wallpaper"
        " change intervals will be set for changes to appear on full hour."
        "\ne.g. interval is set to 20 minutes, daemon starts at"
        " 10:15, first interval will be changed to 5 minutes so changes"
        " will appear on 10:20, 10:40, 11:00 ..."
        );
    gtk_button_set_label (GTK_BUTTON (gw_align_button), "Align");

    /* Wallpaper set command entry */
    gw_command_label = gtk_label_new ("Background set command : ");
    gtk_label_set_xalign (GTK_LABEL (gw_command_label), 0);
    gw_command_entry = gtk_entry_new ();

    gtk_widget_set_tooltip_markup (gw_command_entry,
        "This command will be executed to set background image\n"
        "e.g. <b>feh --bg-fill</b>\nFor more complex commands use <b>[F]</b>" 
        " as a file name\ne.g. <b>feh --bg-fill \"[F]\"</b>");
    gw_command_button = gtk_button_new_with_label ("Select");
    /*
    gtk_widget_set_tooltip_text (gw_command_button,
        "Open wallpaper command select window.");
    */
    g_signal_connect_swapped (gw_command_button,
                              "clicked",
                              G_CALLBACK (event_command_button_pressed),
                              dd_data);

    /* Background change interval widgets */
    ga_adjustment = gtk_adjustment_new (30.0, 1.0, 6000.0, 1.0, 5.0, 0.0);
    gw_spinbutton = gtk_spin_button_new (ga_adjustment, 1.0, 0);
    gtk_widget_set_tooltip_markup (gw_spinbutton,
            "Time between background changes");
    gw_interval_label = gtk_label_new ("Background change interval : ");
    gtk_label_set_xalign (GTK_LABEL (gw_interval_label), 0);
    gw_time_combo = gtk_combo_box_text_new ();
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_time_combo),
                               NULL,
                               "minutes");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (gw_time_combo),
                               NULL,
                               "hours");
    gtk_combo_box_set_active (GTK_COMBO_BOX (gw_time_combo), 0);

    g_signal_connect (gw_spinbutton, "value-changed",
                      G_CALLBACK (event_interval_changed), dd_data);

    /* Autostart button */
    gw_autostart_button = gtk_check_button_new ();
    gtk_widget_set_tooltip_markup (gw_autostart_button, 
        "When <b>enabled</b> wchangerd daemon entry will be placed in "
        "users autostart directory."
        "\nWhen <b>disabled</b> daemon entry will be removed from "
        "users autostart directory."
        "\nThis will allow to start (restart actually) wchangerd daemon "
        " in XDG Autostart compatible window managers.");
    gtk_button_set_label (GTK_BUTTON (gw_autostart_button),
                          "Create autostart entry");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_autostart_button),
                                  cfgfile_autostart_exists ());
    g_signal_connect (gw_autostart_button, "toggled",
                      G_CALLBACK (event_autostart_toggled), NULL);

    /* Setting pointers in DialogData */
    dd_data->gw_random      = gw_random_button;
    dd_data->gw_lastused    = gw_button_selectlast;
    dd_data->gw_command     = gw_command_entry;
    dd_data->gw_interval    = gw_spinbutton;
    dd_data->gw_timealign   = gw_align_button;
    dd_data->gw_inter_combo = gw_time_combo;

    /* Container for settings widgets */
    gw_widget = gtk_grid_new ();
    gtk_grid_set_column_spacing (GTK_GRID (gw_widget), 8);
    gtk_grid_set_row_spacing (GTK_GRID (gw_widget), 8);
    gtk_grid_set_row_homogeneous (GTK_GRID (gw_widget), TRUE);

    /* Packing time interval widgets */
    gtk_grid_attach (GTK_GRID (gw_widget),
                     gw_interval_label, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_widget),
                             gw_spinbutton, gw_interval_label,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_widget),
                             gw_time_combo, gw_spinbutton,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_widget),
                             gw_align_button, gw_time_combo,
                             GTK_POS_RIGHT, 1, 1);

    /* Packing background set command */
    gtk_grid_attach_next_to (GTK_GRID (gw_widget),
                             gw_command_label, gw_interval_label,
                             GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_widget),
                             gw_command_entry, gw_command_label,
                             GTK_POS_RIGHT, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_widget),
                             gw_command_button, gw_command_entry,
                             GTK_POS_RIGHT, 1, 1);

    /* Separator for checkbuttons */
    gtk_grid_attach (GTK_GRID (gw_widget),
                     gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                     4, 0, 1, 3);

    /* Packing button for random change */
    gtk_grid_attach (GTK_GRID (gw_widget),
                     gw_random_button, 5, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_widget),
                             gw_button_selectlast, gw_random_button,
                             GTK_POS_BOTTOM, 1, 1);

    /* Packing autostart widget */
    gtk_grid_attach_next_to (GTK_GRID (gw_widget),
                             gw_autostart_button, gw_button_selectlast,
                             GTK_POS_BOTTOM, 1, 1);
    return gw_widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates widget for monitoring, starting and stopping wchangerd
 */
static GtkWidget *
create_daemon_widget (DialogData *dd_data)
{
    GtkWidget *gw_button;
    GtkWidget *gw_widget;
    GtkWidget *gw_label;

    /* Grid to pack widgets */
    gw_widget = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (gw_widget), 4);
    gtk_grid_set_column_spacing (GTK_GRID (gw_widget), 4);

    /* Daemon name label */
    gw_label = gtk_label_new ("wchangerd");
    gtk_label_set_xalign (GTK_LABEL (gw_label), 0.5);

    gtk_grid_attach (GTK_GRID (gw_widget), gw_label, 0, 0, 2, 1);

    /* Daemon run status label */
    dd_data->gw_dm_label = gtk_label_new (" ");
    gtk_label_set_xalign (GTK_LABEL (dd_data->gw_dm_label), 0.5);

    gtk_grid_attach (GTK_GRID (gw_widget), dd_data->gw_dm_label, 0, 1, 2, 1);

    /* Start daemon button */
    gw_button = create_image_button (NULL, "Start wchangerd daemon",
                                     W_ICON_PLAY);
    g_signal_connect_swapped (gw_button, "clicked",
                              G_CALLBACK (event_start_daemon_pressed), dd_data);

    gtk_grid_attach (GTK_GRID (gw_widget), gw_button, 0, 2, 1, 1);

    /* Stop daemon button */
    gw_button = create_image_button (NULL, "Stop wchangerd daemon",
                                     W_ICON_STOP);
    g_signal_connect_swapped (gw_button, "clicked",
                              G_CALLBACK (event_stop_daemon_pressed), dd_data);

    gtk_grid_attach (GTK_GRID (gw_widget), gw_button, 1, 2, 1, 1);

    gtk_widget_set_halign (gw_widget, GTK_ALIGN_CENTER);

    return gw_widget;
}
/*----------------------------------------------------------------------------*/

/**
 * @brief  Application activate signal.
 */
static void
activate (GtkApplication *app,
          DialogData     *dd_data)
{
    GtkWidget  *gw_window;          /* Application window */
    GtkWidget  *gw_title_widget;    /* Top title widget */
    GtkWidget  *gw_tview;           /* Wallpaper list TreeView */
    GtkWidget  *gw_scroll;          /* Scrolled window for wallpaper list */
    GtkWidget  *gw_buttons_widget;  /* Buttons widget */
    GtkWidget  *gw_img_prev;        /* Wallpaper preview widget */
    GtkWidget  *gw_box_prev;        /* Widget for wallpaper preview */
    GtkWidget  *gw_box_list_btns;   /* Widget for list, buttons, preview */
    GtkWidget  *gw_settings_widget; /* Setings for wallpaper changing */
    GtkWidget  *gw_box_main;        /* Main box to pack everything */
    GtkWidget  *gw_statusbar;       /* Bottom status bar */
    Setting    *st_settings;        /* Program settings */
    Setting    *st_wm      = NULL;  /* Window manager info */
    GdkPixbuf  *gd_pix     = NULL;  /* Default widget icon */
    char       *s_err      = NULL;  /* For error output */
    int         i_err      = 0;     /* For error output */

    /* Find config file and set config file name */
    dialogdata_do_config_file_stuff (dd_data);

    /* Image preview widget */
    gw_img_prev = gtk_image_new ();
    dd_data->gw_imgprev = gw_img_prev;

    /* Main window */
    gw_window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (gw_window), APP_NAME " v" APP_VER);
    g_signal_connect (gw_window, "delete-event",
                      G_CALLBACK (event_on_delete), dd_data);
    dd_data->gw_window = GTK_WINDOW (gw_window);

    /* Create application directories */
    if ((s_err = cfgfile_check_create_dirs ()) != NULL) {
        message_dialog_error (dd_data->gw_window, s_err);
        free (s_err);
        g_application_quit (G_APPLICATION (app));
        return;
    }
    /* Default widget icon */
    if ((gd_pix = get_image (W_ICON_ABOUT)) != NULL) {
        gtk_window_set_default_icon (gd_pix);
        g_object_unref (gd_pix);
    }
    /* Treeview for background file list */
    gw_tview = create_tview ();
    dd_data->gw_view = gw_tview;
    g_signal_connect (gw_tview, "row-activated",
                      G_CALLBACK (event_img_list_activated), gw_img_prev);
    g_signal_connect (G_OBJECT (gw_tview), "key-press-event",
                      G_CALLBACK (event_treeview_key_press), dd_data);

    gw_title_widget    = create_title_widget ();
    gw_buttons_widget  = create_buttons_widget (dd_data);
    gw_settings_widget = create_settings_widget (dd_data);
    gw_statusbar       = gtk_statusbar_new ();
    dd_data->gw_statusbar = gw_statusbar;

    preview_from_file (gw_img_prev, NULL);

    /* Scrolled window for TreeView */
    gw_scroll = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (gw_scroll), gw_tview);

    /* Box with wallpaper preview */
    gw_box_prev = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_prev),
                        gw_img_prev,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_prev),
                        create_preview_label (),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_prev),
                        create_daemon_widget (dd_data),
                        FALSE, FALSE, 4);

    /* Pack file list, button box, separatr and wallpaper preview */
    gw_box_list_btns = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_list_btns),
                        gw_scroll,
                        TRUE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_list_btns),
                        gw_buttons_widget,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_list_btns),
                        gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_list_btns),
                        gw_box_prev,
                        FALSE, FALSE, 4);

    /* Pack in main box title and box with list, buttons and preview */
    gw_box_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_title_widget,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_box_list_btns,
                        TRUE, TRUE, 4);

    /* Pack in main box separator and settings */
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_settings_widget,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_statusbar,
                        FALSE, FALSE, 4);

    statusbar_push_config_info (dd_data);

    gtk_container_set_border_width (GTK_CONTAINER (gw_box_main), 10);
    gtk_container_add (GTK_CONTAINER (gw_window), gw_box_main);

    /* Read program settings */
    st_settings = setts_read (dialogdata_get_cfg_file (dd_data), &i_err);

    /* Check window managers data config file for new wms and */
    /* get window manager data */
    if ((i_err = wms_check_for_new_wms ()) == ERR_OK) {
        st_wm = wms_get_wm_info (&i_err);
    }
    if (i_err != ERR_OK) {
        message_dialog_error (NULL, err_get_message (i_err));
        settings_free_all (st_settings);
        settings_free_all (st_wm);
        g_application_quit (G_APPLICATION (app));
        return;
    }
    setts_check_defaults (st_settings);

    widgets_set_settings (dd_data, st_settings, st_wm, &i_err);

    settings_free_all (st_wm);
    settings_free_all (st_settings);

    if (i_err != ERR_OK) {
        message_dialog_error (NULL, err_get_message (i_err));
        g_application_quit (G_APPLICATION (app));
        return;
    }
    /* Set info about wchangerd deamon presence */
    daemon_monitor (dd_data);
    /* Add gtk thread for checking wchangerd deaemon process presence */
    gdk_threads_add_timeout_full (G_PRIORITY_LOW,
                                  3000,
                                  daemon_monitor,
                                  dd_data,
                                  NULL);

    gtk_window_set_application (GTK_WINDOW (gw_window), GTK_APPLICATION (app));
    gtk_widget_show_all (gw_window);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application shutdown signal.
 */
static void
shutdown (GtkApplication *application __attribute__ ((unused)),
          DialogData     *dd_data)
{
    dialogdata_free (dd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Handling command line options.
 */
static gint
local_options (GApplication *application __attribute__ ((unused)),
               GVariantDict *options,
               DialogData   *dd_data)
{
    char *s_val = NULL;

    if (g_variant_dict_lookup (options, "config", "s", &s_val)) {
        dd_data->s_cfg_file = strdup (s_val);
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Main function.
 *
 * @param[in] argc Arguments passed to the program from the environment in which
 *                 the program is run
 * @param[in] argv Pointer to the first element of an array of pointers that
 *                 represent the arguments passed to the program
 * @return         Return value
 */
int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    DialogData     *dd_data;
    int             status;

    dd_data = dialogdata_new ();
    app = gtk_application_new ("org.nongnu.WallChanger",
                               G_APPLICATION_FLAGS_NONE);
    g_application_add_main_option (G_APPLICATION (app),
                                   "config",
                                   0,
                                   G_OPTION_FLAG_IN_MAIN,
                                   G_OPTION_ARG_STRING,
                                   "Path to config file",
                                   "FILENAME");
    g_signal_connect (app, "activate", G_CALLBACK (activate), dd_data);
    g_signal_connect (app, "shutdown", G_CALLBACK (shutdown), dd_data);
    g_signal_connect (app, "handle-local-options",
            G_CALLBACK (local_options), dd_data);
    g_set_application_name (APP_NAME);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
/*----------------------------------------------------------------------------*/

