/**
 * @file  config_main.c
 * @copyright Copyright (C) 2019-2026 Michał Bąbik
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
 * @brief Wallpaper change config window
 *
 * Automatic wallpaper changer
 *
 * @version 1.7.0
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdint.h>
#include <gtk/gtk.h>
#include "dialog_data.h"
#include "settings.h"
#include "wallpaper_set.h"
#include "icons.h"
#include "image_dialogs.h"
#include "dialogs_other.h"
#include "message_dialogs.h"
#include "wallpaper_set_command_dialog.h"
#include "other_settings_dialog.h"
#include "tree_view.h"
#include "dir_content.h"
#include "file_dir_functions.h"
#include "preview.h"
#include "string_functions.h"
#include "errors.h"
#include "cfg_file.h"
#include "defs.h"
#include "daemon.h"
#include "window_manager_info.h"
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
 *                                             const uint32_t    val)
 *
 * @brief  Set wallpaper change interval value to widgets
 *
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @param[in]     val      Change interval value to set
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static uint32_t    get_wallpaper_ch_interval   (const DialogData  *dd_data);

static void        set_wallpaper_ch_interval   (const DialogData  *dd_data,
                                                const uint32_t     val);
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
 *                                        Setting           *settings,
 *                                        Setting           *wmlist,
 *                                        int               *err)
 *
 * @brief  Loading data from list of settings to program window.
 *
 * @param[in,out] dd_data   DialogData object with settings and widget data
 * @param[in]     settings  List with settings
 * @param[in]     wmlist    List with window manager info
 * @param[out]    err       Error output
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static Setting   * widgets_get_settings        (const DialogData  *dd_data);

static void        widgets_set_settings        (const DialogData  *dd_data,
                                                Setting           *settings,
                                                Setting           *wmlist,
                                                int               *err);
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
 *                                            GtkWidget         *img_prev)
 *
 * @brief  Make preview image widget of image (file path).
 *
 * @param[in]  tree_view    The object on which the signal is emitted
 * @param[in]  path         The GtkTreePath for the activated row
 * @param      column       The GtkTreeViewColumn in which the activation
 *                          occurred
 * @param[out] img_prev     Preview image widget
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
                                                GtkWidget         *img_prev);

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
 * @fn  static GtkWidget * create_image_button (const char   *label,
 *                                              const char   *hint,
 *                                              const IconImg icon)
 * @brief  Create button with icon/label/hint.
 *
 * @param[in] label  Button label
 * @param[in] hint   Button hint
 * @param[in] icon   Icon number
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

static GtkWidget * create_image_button         (const char        *label,
                                                const char        *hint,
                                                const IconImg      icon);

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
cfg_file_check_create_dirs (void)
{
    char *ret = nullptr;
    int   err = ERR_OK;
    int   i   = 0;
    char *dirs[3];

    dirs[0] = cfg_file_get_query_path ();
    dirs[1] = cfg_file_get_app_thumbnails_path ();
    dirs[2] = cfg_file_get_app_wallpapers_path ();

    for (i = 0; i < 3; ++i) {
        if ((err = dir_create_with_subdirs (dirs[i])) != ERR_OK) {
            ret = str_comb (dirs[i], "\n");
            str_append (&ret, err_get_message (err));
            break;
        }
    }
    for (i = 0; i < 3; ++i) {
        free (dirs[i]);
    }
    return ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get wallpaper change minutes interval from widgets
 */
static uint32_t
get_wallpaper_ch_interval (const DialogData *dd_data)
{
    uint32_t res = 0; /* Wallpaper change interval in minutes */

    /* Get spinbutton value of change time interval */
    res = (uint32_t) gtk_spin_button_get_value (
            GTK_SPIN_BUTTON (dd_data->gw_interval));

    /* Check combobox setting if value is supposed to be minutes or hours.
     * If value is set to hours multiply returned value by 60 */
    if (gtk_combo_box_get_active (GTK_COMBO_BOX (dd_data->gw_inter_combo)) == 1)
        res *= 60;

    return res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set wallpaper change interval value to widgets
 */
static void
set_wallpaper_ch_interval (const DialogData *dd_data,
                           const uint32_t    val)
{
    uint32_t tmp = val; /* Temp interval value */

    /* Check if minutes value can be displayed as hours */
    if ((tmp / 60 >= 1) && (tmp % 60 == 0)) {

        /* If value can be hours set combobox to hours */
        gtk_combo_box_set_active (GTK_COMBO_BOX (dd_data->gw_inter_combo), 1);
        tmp /= 60;
    }
    else {
        /* Value is suppose to be set as minutes set combobox to minutes */
        gtk_combo_box_set_active (GTK_COMBO_BOX (dd_data->gw_inter_combo), 0);
    }
    /* Set spinbutton value with change interval */
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (dd_data->gw_interval),
                               (double) tmp);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read settings from widgets and store them in WallSett object.
 */
static Setting *
widgets_get_settings (const DialogData *dd_data)
{
    Setting    *settings = nullptr; /* Setting item to return */
    const char *str      = nullptr; /* Value for a string setting */
    int64_t     val      = 0;       /* Value for an integer setting */

    /* Get wallpaper list from treeview and add to settlist */
    settings = setting_new_array (get_setting_name (SETT_WALL_ARRAY));
    setting_add_child (settings, treeview_get_setting_data (dd_data->gw_view));

    /* Get random wallpaper setting */
    val = (int64_t) gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (dd_data->gw_random));
    settings_append (settings,
            setting_new_int (get_setting_name (SETT_RANDOM_OPT), val));

    /* Get last used wallpaper on start setting */
    val = (int64_t) gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (dd_data->gw_lastused));
    settings_append (settings,
            setting_new_int (get_setting_name (SETT_LAST_USED_OPT), val));

    /* Get time align setting */
    val = (int64_t) gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (dd_data->gw_timealign));
    settings_append (settings,
            setting_new_int (get_setting_name (SETT_TIME_ALIGN_OPT), val));

    /* Get wallpaper change interval setting */
    val = (int64_t) get_wallpaper_ch_interval (dd_data);
    settings_append (settings,
            setting_new_int (get_setting_name (SETT_INTERVAL_VAL), val));

    /* Get wallpaper set command */
    str = gtk_entry_get_text (GTK_ENTRY (dd_data->gw_command));
    settings_append (settings,
            setting_new_string (get_setting_name (SETT_BG_CMD), str));

    return settings;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Loading data from SettList list of settings to program window.
 */
static void
widgets_set_settings (const DialogData *dd_data,
                      Setting          *settings,
                      Setting          *wmlist,
                      int              *err)
{
    Setting    *item     = nullptr; /* Setting to read */
    int         width    = 0;       /* Window width value */
    int         height   = 0;       /* Window height value */
    const char *lastused = nullptr; /* Last used wallpapet string */
    char       *cmd      = nullptr; /* Wallpaper set command */

    *err = ERR_OK;

    /* Check window managers and set wallpaper set command */
    cmd = wms_get_wallpaper_command (dd_data->s_cfg_file,
                                     settings,
                                     wmlist,
                                     err);
    if (*err != ERR_OK) {
        free (cmd);
        return;
    }
    gtk_entry_set_text (GTK_ENTRY (dd_data->gw_command), cmd);
    free (cmd);

    /* Set last used wallpaper on start setting */
    item = settings_find (settings, get_setting_name (SETT_LAST_USED_OPT));
    if (item != nullptr) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_lastused),
                                      (gboolean) setting_get_int (item));
    }
    /* Set minutes/hours of wallpaper change interval */
    item = settings_find (settings, get_setting_name (SETT_INTERVAL_VAL));
    if (item != nullptr) {
        set_wallpaper_ch_interval (dd_data,
                                   (uint32_t) setting_get_int (item));
    }
    /* Set wallpaper random choose setting */
    item = settings_find (settings, get_setting_name (SETT_RANDOM_OPT));
    if (item != nullptr) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_random),
                                      (gboolean) setting_get_int (item));
    }
    /* Set time align setting */
    item = settings_find (settings, get_setting_name (SETT_TIME_ALIGN_OPT));
    if (item != nullptr) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_timealign),
                                      (gboolean) setting_get_int (item));
    }
    /* Get wallpaper list and add to treeview */
    item = settings_find (settings, get_setting_name (SETT_WALL_ARRAY));
    if (item != nullptr) {
        treeview_add_items_setting (dd_data->gw_view,
                                    setting_get_child (item));
    }
    /* Set window dimensions */
    item = settings_find (settings, get_setting_name (SETT_WIN_WIDTH));
    if (item != nullptr) {
        width = (int) setting_get_int (item);
    }
    item = settings_find (settings, get_setting_name (SETT_WIN_HEIGHT));
    if (item != nullptr) {
        height = (int) setting_get_int (item);
    }
    gtk_window_set_default_size (GTK_WINDOW (dd_data->gw_window), width, height);

    /* Check if there is last used wallpaper info, if it is select it on
     * list and make a preview image */
    item = settings_find (settings, get_setting_name (SETT_LAST_USED_STR));
    if (item != nullptr)
        lastused = setting_get_string (item);

    if (lastused != nullptr) {
        preview_from_file (dd_data->gw_imgprev, lastused);
        treeview_find_select_item (dd_data->gw_view, lastused);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sets statusbar info about actual config file.
 */
static void
statusbar_push_config_info (const DialogData *dd_data)
{
    char *info = nullptr;

    info = dialog_data_get_status_config_info (dd_data);

    gtk_statusbar_push (GTK_STATUSBAR (dd_data->gw_statusbar),
            gtk_statusbar_get_context_id (GTK_STATUSBAR (dd_data->gw_statusbar),
                                          STATUS_CONFIG),
            info);
    free (info);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Add images" button pressed.
 */
static void
event_add_img_pressed (const DialogData *dd_data)
{
    GSList *files = nullptr; /* List with files from dialog */

    /* Run image select dialog and get selected files */
    if ((files = add_images_dialog (dd_data->gw_window)) != nullptr) {
        /* Add items to TreeView */
        treeview_add_items_gslist (dd_data->gw_view, files);
        g_slist_free_full (files, g_free);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Add images from folder" button pressed.
 */
static void
event_add_img_dir_pressed (const DialogData *dd_data)
{
    char  *folder = nullptr; /* Selecred directory name */
    GList *files  = nullptr; /* Images in directory */

    /* Run directory select dialog and get selected directory name */
    if ((folder = add_images_folder_dialog (dd_data->gw_window)) != nullptr) {
        /* Scan directory for files and append them to file list */
        if ((files = get_dir_content_filter_images (folder)) != nullptr) {
            /* Add items to TreeView */
            treeview_add_items_glist (dd_data->gw_view, files);
            g_list_free_full (files, g_free);
        }
        g_free (folder);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Add images from web" button pressed.
 */
static void
event_add_img_web_pressed (const DialogData *dd_data)
{
    GList *list = nullptr; /* List with wallpapers to add */

    list = add_images_from_web_dialog (dd_data->gw_window,
                                       dd_data->s_cfg_file);
    treeview_add_items_glist (dd_data->gw_view, list);
    g_list_free_full (list, (GDestroyNotify) free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Set wallpaper" button pressed.
 */
static void
event_set_wallpaper_pressed (const DialogData *dd_data)
{
    GtkTreeSelection *selection;      /* TreeSelection */
    GtkTreeModel     *model;          /* TreeModel */
    GtkTreeIter       iter;           /* TreeIter */
    ImageInfo        *info;           /* Wallpaper image info */
    GList            *list = nullptr; /* List of selected wallpapers */
    const char       *cmd  = nullptr; /* Wallpaper set command */
    int               err  = 0;       /* Error output */

    model     = gtk_tree_view_get_model (GTK_TREE_VIEW (dd_data->gw_view));
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (dd_data->gw_view));
    list      = gtk_tree_selection_get_selected_rows (selection, &model);

    if (list == nullptr)
        return;

    if (gtk_tree_model_get_iter (model, &iter, list->data)) {

        info = treemodel_get_data (model, iter);
        cmd  = gtk_entry_get_text (GTK_ENTRY (dd_data->gw_command));
        err  = wallpaper_dialog_set (cmd,
                                     image_info_get_file_path (info),
                                     dialog_data_get_cfg_file (dd_data));
        if (err != ERR_OK) {
            message_dialog_error (dd_data->gw_window, err_get_message (err));
        }
        image_info_free (info);
    }
    g_list_free_full (list, (GDestroyNotify) gtk_tree_path_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Save settings" button pressed.
 */
static void
event_save_settings_pressed (const DialogData *dd_data)
{
    Setting *settings; /* List of settings */
    int      err = 0;  /* Error value */

    settings = widgets_get_settings (dd_data);
    err = setts_check_update_file (dialog_data_get_cfg_file (dd_data),
                                   settings);
    settings_free_all (settings);

    if (err != ERR_OK) {
        message_dialog_error (dd_data->gw_window, err_get_message (err));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make preview image widget of image (file path).
 */
static void
event_img_list_activated (GtkTreeView       *tree_view,
                          GtkTreePath       *path,
                          [[maybe_unused]] GtkTreeViewColumn *column,
                          GtkWidget         *img_prev)
{
    GtkTreeModel *model; /* TreeModel */
    GtkTreeIter   iter;  /* TreeIter */
    ImageInfo    *info;  /* Wallpaper image info */

    model = gtk_tree_view_get_model (tree_view);

    if (gtk_tree_model_get_iter (model, &iter, path)) {

        info = treemodel_get_data (model, iter);

        preview_from_file (img_prev, image_info_get_file_path (info));
        image_info_free (info);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  React to key pressed in TreeView.
 */
static gboolean
event_treeview_key_press ([[maybe_unused]] GtkWidget *widget,
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
    static int    prev      = 0;       /* Previous interval value */
    GtkTreeModel *model;               /* TreeModel */
    GtkComboBox  *combo;               /* ComboBox */
    GtkTreeIter   iter;                /* TreeIter */
    char         *minute    = nullptr; /* String for minute entry */
    char         *hour      = nullptr; /* String for hour entry */
    int           val       = 0;       /* Actual interval value */
    int           id_col    = 0;       /* id position */
    int           entry_col = 0;       /* entry position */
    int           change    = 0;       /* Change text or not */

    combo  = GTK_COMBO_BOX (dd_data->gw_inter_combo);
    minute = strdup ("minute");
    hour   = strdup ("hour");
    val    = gtk_spin_button_get_value_as_int (spin_button);

    if (prev == 1 && val != 1) {
        change = 1;
        str_append (&minute,  "s");
        str_append (&hour, "s");
    }
    else if (prev != 1 && val == 1) {
        change = 1;
    }
    if (change) {
        model     = gtk_combo_box_get_model (combo);
        id_col    = gtk_combo_box_get_id_column (combo);
        entry_col = gtk_combo_box_get_entry_text_column (combo);

        if (gtk_tree_model_get_iter_first (model, &iter)) {
            gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                        id_col,    minute,
                        entry_col, minute,
                        -1);
            if (gtk_tree_model_iter_next (model, &iter)) {
                gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                        id_col,    hour,
                        entry_col, hour,
                        -1);
            }
        }
    }
    free (minute);
    free (hour);
    prev = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Command select button pressed.
 */
static void
event_command_button_pressed (const DialogData *dd_data)
{
    GSList *files   = nullptr; /* For wallpaers */
    char   *command = nullptr; /* Command from dialog */

    files = treeview_get_data (dd_data->gw_view);
    command = wp_set_command_dialog_run (dd_data->gw_window,
            gtk_entry_get_text (GTK_ENTRY (dd_data->gw_command)), files);

    if (command != nullptr) {
        gtk_entry_set_text (GTK_ENTRY (dd_data->gw_command), command);
        free (command);
    }
    g_slist_free_full (files, (GDestroyNotify) image_info_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed active state of autostart checkbox
 */
static void
event_autostart_toggled (GtkToggleButton *togglebutton,
                         [[maybe_unused]] gpointer user_data)
{
    gtk_toggle_button_get_active (togglebutton) ?
        cfg_file_autostart_create () :
        cfg_file_autostart_remove ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  On main window delete, check settings.
 */
static gboolean
event_on_delete (GtkWidget        *window,
                 [[maybe_unused]] GdkEvent *event,
                 const DialogData *dd_data)
{
    GtkWidget *dialog;           /* Question dialog */
    Setting   *settings;         /* Program settings */
    char      *buff   = nullptr; /* Buffer for settings data */
    int        res    = 0;       /* Dialog result */
    int        err    = 0;       /* Error value */
    int        width  = 0;       /* Window width */
    int        height = 0;       /* Window height */

    gtk_window_get_size (GTK_WINDOW (window), &width, &height);

    settings = widgets_get_settings (dd_data);

    buff  = setts_check_update (dialog_data_get_cfg_file (dd_data),
                                settings,
                                &err);
    settings_free_all (settings);

    if (err != ERR_OK) {
        message_dialog_error (GTK_WINDOW (window), err_get_message (err));
    }
    if (buff != nullptr) {
        dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_QUESTION,
                                         GTK_BUTTONS_YES_NO,
            "Settings changed, do you want to save them ?");
        gtk_window_set_title (GTK_WINDOW (dialog), "Settings changed");

        res = gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

        if (res == GTK_RESPONSE_YES) {
            err = setts_update_file (dialog_data_get_cfg_file (dd_data),
                                     buff);
            if (err != ERR_OK) {
                message_dialog_error (GTK_WINDOW (window),
                                      err_get_message (err));
            }
        }
        free (buff);
    }
    setts_update_window_size (dialog_data_get_cfg_file (dd_data), width, height);
    return FALSE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Start background daemon process
 */
static void
event_start_daemon_pressed (DialogData *dd_data)
{
    daemon_start ();
    daemon_monitor (dd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Stop background daemon process
 */
static void
event_stop_daemon_pressed (DialogData *dd_data)
{
    daemon_kill ();
    daemon_monitor (dd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Other settings button pressed.
 */
static void
event_other_settings_pressed (DialogData *dd_data)
{
    char *file = treeview_get_one_file (dd_data->gw_view);
    other_settings_dialog (dd_data->gw_window, dd_data->s_cfg_file, file);
    free (file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Monitors running of wchangerd daemon.
 */
static gboolean
daemon_monitor (gpointer data)
{
    DialogData *dd_data = (DialogData *) data;

    if (daemon_check_presence ()) {
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
    GtkWidget  *widget; /* Widget to return */
    const char *str    = APP_NAME " - Program settings";
    const char *format = "<span size=\"20000\" weight=\"bold\" "
                         "style=\"italic\">%s</span>";
                         //"foreground=\"#0099e6\" style=\"italic\">%s</span>";
    char       *markup = nullptr;
    GtkWidget  *label  = gtk_label_new (nullptr);

    widget = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);

    markup = g_markup_printf_escaped (format, str);
    gtk_label_set_markup (GTK_LABEL (label), markup);
    g_free (markup);

    gtk_box_pack_start (GTK_BOX (widget),
                        label,
                        FALSE, FALSE, 24);
    return widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create button with icon/label/hint.
 */
static GtkWidget *
create_image_button (const char   *label,
                     const char   *hint,
                     const IconImg icon)
{
    GtkWidget *button;           /* Return button with image */
    GtkWidget *image;            /* Image widget to put in button */
    GdkPixbuf *pixbuf = nullptr; /* Pixbuf for image data */

    button = gtk_button_new ();

    if (!str_is_empty (label))
        gtk_button_set_label (GTK_BUTTON (button), label);

    if (!str_is_empty (hint))
        gtk_widget_set_tooltip_text (button, hint);

    if (icon < W_IMG_COUNT && (pixbuf = get_icon (icon)) != nullptr) {
        image = gtk_image_new_from_pixbuf (pixbuf);
        gtk_button_set_image (GTK_BUTTON (button), image);
        g_object_unref (pixbuf);
    }
    return button;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create side buttons widget.
 */
static GtkWidget *
create_buttons_widget (DialogData *dd_data)
{
    GtkWidget *widget; /* Widget to return */
    GtkWidget *button; /* For buttons */

    widget = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);

    button = create_image_button (nullptr, "Add images", W_ICON_ADD);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (event_add_img_pressed),
                              dd_data);
    button = create_image_button (nullptr, "Add images from folder",
                                     W_ICON_ADD_DIR);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (event_add_img_dir_pressed),
                              dd_data);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    button = create_image_button (nullptr, "Add wallpapers from web",
                                     W_ICON_ADD_WEB);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (event_add_img_web_pressed),
                              dd_data);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    button = create_image_button (nullptr, "Remove selected images",
                                     W_ICON_REMOVE);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (treeview_remove_selected),
                              dd_data->gw_view);
    button = create_image_button (nullptr, "Move up", W_ICON_UP);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (treeview_move_up),
                              dd_data->gw_view);
    button = create_image_button (nullptr, "Move down", W_ICON_DOWN);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (treeview_move_down),
                              dd_data->gw_view);
    button = create_image_button (nullptr, "Remove duplicates", W_ICON_DUPL);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (treeview_remove_duplicates),
                              dd_data->gw_view);
    button = create_image_button (nullptr, "Set wallpaper", W_ICON_SCREEN);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (event_set_wallpaper_pressed),
                              dd_data);
    button = create_image_button (nullptr, "Save settings", W_ICON_FLOPPY);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (event_save_settings_pressed),
                              dd_data);
    button = create_image_button (nullptr, "Other settings", W_ICON_SETTING);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (event_other_settings_pressed),
                              dd_data);
    button = create_image_button (nullptr, "About Wall Changer", W_ICON_INFO);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (about_app_dialog),
                              GTK_WINDOW (dd_data->gw_window));
    button = create_image_button (nullptr, "Exit configuration dialog",
                                     W_ICON_EXIT);
    g_signal_connect_swapped (button,
                              "clicked",
                              G_CALLBACK (gtk_window_close),
                              dd_data->gw_window);
    gtk_box_pack_start (GTK_BOX (widget),
                        button,
                        FALSE, FALSE, 4);
    return widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates widget with settings for wallpaper changing.
 */
static GtkWidget *
create_settings_widget (DialogData *dd_data)
{
    GtkWidget     *widget;            /* Widget to return */
    GtkWidget     *random_button;     /* Random image GtkCheckButton */
    GtkWidget     *button_selectlast; /* Set last used GtkCheckButton */
    GtkWidget     *align_button;      /* Time align GtkCheckButton */
    GtkWidget     *command_label;     /* Wallpaper set cmd GtkLabel */
    GtkWidget     *command_button;    /* Button for command dialog */
    GtkWidget     *command_entry;     /* Wallpaper set cmd GtkEntry */
    GtkWidget     *interval_label;    /* Time interval GtkLabel */
    GtkWidget     *spinbutton;        /* Time interval GtkSpinButton */
    GtkAdjustment *adjustment;        /* Time interval GtkAdjustment */
    GtkWidget     *time_combo;        /* Time interval GtkComboBoxText*/
    GtkWidget     *autostart_button;  /* Autostart file GtkCheckButton */

    /* Random wallpaper change button */
    random_button = gtk_check_button_new ();
    gtk_widget_set_tooltip_markup (random_button, 
        "When <b>enabled</b> wallpaper images will be selected randomly.\n"
        "When <b>disabled</b> wallpapers will be set in the same order as "
        "they appear on the list");
    gtk_button_set_label (GTK_BUTTON (random_button),
                          "Random wallpaper change");

    /* Select last used wallpaper button */
    button_selectlast = gtk_check_button_new ();
    gtk_widget_set_tooltip_markup (button_selectlast,
        "When <b>enabled</b> first wallpaper set on program start will "
        " be the last one previously set.\n"
        "When <b>disabled</b> wallpaper will be set randomly or next in list "
        "to the last one before stop.");
    gtk_button_set_label (GTK_BUTTON (button_selectlast),
                          "Select last used wallpaper at start");

    /* Time align button */
    align_button = gtk_check_button_new ();
    gtk_widget_set_tooltip_markup (align_button, 
        "This option enables time align, when <b>enabled</b> wallpaper"
        " change intervals will be set for changes to appear on full hour."
        "\ne.g. interval is set to 20 minutes, daemon starts at"
        " 10:15, first interval will be changed to 5 minutes so changes"
        " will appear on 10:20, 10:40, 11:00 ..."
        );
    gtk_button_set_label (GTK_BUTTON (align_button), "Align");

    /* Wallpaper set command entry */
    command_label = gtk_label_new ("Background set command : ");
    gtk_label_set_xalign (GTK_LABEL (command_label), 0);
    command_entry = gtk_entry_new ();

    gtk_widget_set_tooltip_markup (command_entry,
        "This command will be executed to set background image\n"
        "e.g. <b>feh --bg-fill</b>\nFor more complex commands use <b>[F]</b>" 
        " as a file name\ne.g. <b>feh --bg-fill \"[F]\"</b>");
    command_button = gtk_button_new_with_label ("Select");
    /*
    gtk_widget_set_tooltip_text (command_button,
        "Open wallpaper command select window.");
    */
    g_signal_connect_swapped (command_button,
                              "clicked",
                              G_CALLBACK (event_command_button_pressed),
                              dd_data);

    /* Background change interval widgets */
    adjustment = gtk_adjustment_new (30.0, 1.0, 6000.0, 1.0, 5.0, 0.0);
    spinbutton = gtk_spin_button_new (adjustment, 1.0, 0);
    gtk_widget_set_tooltip_markup (spinbutton,
            "Time between background changes");
    interval_label = gtk_label_new ("Background change interval : ");
    gtk_label_set_xalign (GTK_LABEL (interval_label), 0);
    time_combo = gtk_combo_box_text_new ();
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (time_combo),
                               nullptr,
                               "minutes");
    gtk_combo_box_text_append (GTK_COMBO_BOX_TEXT (time_combo),
                               nullptr,
                               "hours");
    gtk_combo_box_set_active (GTK_COMBO_BOX (time_combo), 0);

    g_signal_connect (spinbutton, "value-changed",
                      G_CALLBACK (event_interval_changed), dd_data);

    /* Autostart button */
    autostart_button = gtk_check_button_new ();
    gtk_widget_set_tooltip_markup (autostart_button, 
        "When <b>enabled</b> wchangerd daemon entry will be placed in "
        "users autostart directory."
        "\nWhen <b>disabled</b> daemon entry will be removed from "
        "users autostart directory."
        "\nThis will allow to start (restart actually) wchangerd daemon "
        " in XDG Autostart compatible window managers.");
    gtk_button_set_label (GTK_BUTTON (autostart_button),
                          "Create autostart entry");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (autostart_button),
                                  cfg_file_autostart_exists ());
    g_signal_connect (autostart_button, "toggled",
                      G_CALLBACK (event_autostart_toggled), nullptr);

    /* Setting pointers in DialogData */
    dd_data->gw_random      = random_button;
    dd_data->gw_lastused    = button_selectlast;
    dd_data->gw_command     = command_entry;
    dd_data->gw_interval    = spinbutton;
    dd_data->gw_timealign   = align_button;
    dd_data->gw_inter_combo = time_combo;

    /* Container for settings widgets */
    widget = gtk_grid_new ();
    gtk_grid_set_column_spacing (GTK_GRID (widget), 8);
    gtk_grid_set_row_spacing (GTK_GRID (widget), 8);
    gtk_grid_set_row_homogeneous (GTK_GRID (widget), TRUE);

    /* Packing time interval widgets */
    gtk_grid_attach (GTK_GRID (widget),
                     interval_label, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (widget),
                             spinbutton, interval_label,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (widget),
                             time_combo, spinbutton,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (widget),
                             align_button, time_combo,
                             GTK_POS_RIGHT, 1, 1);

    /* Packing background set command */
    gtk_grid_attach_next_to (GTK_GRID (widget),
                             command_label, interval_label,
                             GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (widget),
                             command_entry, command_label,
                             GTK_POS_RIGHT, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (widget),
                             command_button, command_entry,
                             GTK_POS_RIGHT, 1, 1);

    /* Separator for checkbuttons */
    gtk_grid_attach (GTK_GRID (widget),
                     gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                     4, 0, 1, 3);

    /* Packing button for random change */
    gtk_grid_attach (GTK_GRID (widget),
                     random_button, 5, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (widget),
                             button_selectlast, random_button,
                             GTK_POS_BOTTOM, 1, 1);

    /* Packing autostart widget */
    gtk_grid_attach_next_to (GTK_GRID (widget),
                             autostart_button, button_selectlast,
                             GTK_POS_BOTTOM, 1, 1);
    return widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates widget for monitoring, starting and stopping wchangerd
 */
static GtkWidget *
create_daemon_widget (DialogData *dd_data)
{
    GtkWidget *widget; /* Widget to return */
    GtkWidget *button; /* For buttons */
    GtkWidget *label;  /* Daemon label */

    /* Grid to pack widgets */
    widget = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (widget), 4);
    gtk_grid_set_column_spacing (GTK_GRID (widget), 4);

    /* Daemon name label */
    label = gtk_label_new ("wchangerd");
    gtk_label_set_xalign (GTK_LABEL (label), 0.5);

    gtk_grid_attach (GTK_GRID (widget), label, 0, 0, 2, 1);

    /* Daemon run status label */
    dd_data->gw_dm_label = gtk_label_new (" ");
    gtk_label_set_xalign (GTK_LABEL (dd_data->gw_dm_label), 0.5);

    gtk_grid_attach (GTK_GRID (widget), dd_data->gw_dm_label, 0, 1, 2, 1);

    /* Start daemon button */
    button = create_image_button (nullptr, "Start wchangerd daemon",
                                     W_ICON_PLAY);
    g_signal_connect_swapped (button, "clicked",
                              G_CALLBACK (event_start_daemon_pressed), dd_data);

    gtk_grid_attach (GTK_GRID (widget), button, 0, 2, 1, 1);

    /* Stop daemon button */
    button = create_image_button (nullptr, "Stop wchangerd daemon",
                                     W_ICON_STOP);
    g_signal_connect_swapped (button, "clicked",
                              G_CALLBACK (event_stop_daemon_pressed), dd_data);

    gtk_grid_attach (GTK_GRID (widget), button, 1, 2, 1, 1);

    gtk_widget_set_halign (widget, GTK_ALIGN_CENTER);

    return widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application activate signal.
 */
static void
activate (GtkApplication *app,
          DialogData     *dd_data)
{
    GtkWidget  *window;            /* Application window */
    GtkWidget  *title_widget;      /* Top title widget */
    GtkWidget  *tree_view;         /* Wallpaper list TreeView */
    GtkWidget  *scroll;            /* Scrolled window for wallpaper list */
    GtkWidget  *buttons_widget;    /* Buttons widget */
    GtkWidget  *img_prev;          /* Wallpaper preview widget */
    GtkWidget  *box_prev;          /* Widget for wallpaper preview */
    GtkWidget  *box_list_btns;     /* Widget for list, buttons, preview */
    GtkWidget  *settings_widget;   /* Settings for wallpaper changing */
    GtkWidget  *box_main;          /* Main box to pack everything */
    GtkWidget  *statusbar;         /* Bottom status bar */
    Setting    *settings;          /* Program settings */
    Setting    *wm      = nullptr; /* Window manager info */
    GdkPixbuf  *pixbuf  = nullptr; /* Default widget icon */
    char       *err_msg = nullptr; /* For error output */
    int         err     = 0;       /* For error output */

    /* Find config file and set config file name */
    dialog_data_do_config_file_stuff (dd_data);

    /* Image preview widget */
    img_prev = gtk_image_new ();
    dd_data->gw_imgprev = img_prev;

    /* Main window */
    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), APP_NAME " v" APP_VER);
    g_signal_connect (window, "delete-event",
                      G_CALLBACK (event_on_delete), dd_data);
    dd_data->gw_window = GTK_WINDOW (window);

    /* Create application directories */
    if ((err_msg = cfg_file_check_create_dirs ()) != nullptr) {
        message_dialog_error (dd_data->gw_window, err_msg);
        free (err_msg);
        g_application_quit (G_APPLICATION (app));
        return;
    }
    /* Default widget icon */
    if ((pixbuf = get_icon (W_ICON_ABOUT)) != nullptr) {
        gtk_window_set_default_icon (pixbuf);
        g_object_unref (pixbuf);
    }
    /* Treeview for background file list */
    tree_view = create_tview ();
    dd_data->gw_view = tree_view;
    g_signal_connect (tree_view, "row-activated",
                      G_CALLBACK (event_img_list_activated), img_prev);
    g_signal_connect (G_OBJECT (tree_view), "key-press-event",
                      G_CALLBACK (event_treeview_key_press), dd_data);

    title_widget    = create_title_widget ();
    buttons_widget  = create_buttons_widget (dd_data);
    settings_widget = create_settings_widget (dd_data);
    statusbar       = gtk_statusbar_new ();
    dd_data->gw_statusbar = statusbar;

    preview_from_file (img_prev, nullptr);

    /* Scrolled window for TreeView */
    scroll = gtk_scrolled_window_new (nullptr, nullptr);
    gtk_container_add (GTK_CONTAINER (scroll), tree_view);

    /* Box with wallpaper preview */
    box_prev = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start (GTK_BOX (box_prev),
                        img_prev,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box_prev),
                        create_preview_label (),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box_prev),
                        create_daemon_widget (dd_data),
                        FALSE, FALSE, 4);

    /* Pack file list, button box, separatr and wallpaper preview */
    box_list_btns = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_pack_start (GTK_BOX (box_list_btns),
                        scroll,
                        TRUE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (box_list_btns),
                        buttons_widget,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box_list_btns),
                        gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box_list_btns),
                        box_prev,
                        FALSE, FALSE, 4);

    /* Pack in main box title and box with list, buttons and preview */
    box_main = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start (GTK_BOX (box_main),
                        title_widget,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box_main),
                        box_list_btns,
                        TRUE, TRUE, 4);

    /* Pack in main box separator and settings */
    gtk_box_pack_start (GTK_BOX (box_main),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box_main),
                        settings_widget,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (box_main),
                        statusbar,
                        FALSE, FALSE, 4);

    statusbar_push_config_info (dd_data);

    gtk_container_set_border_width (GTK_CONTAINER (box_main), 10);
    gtk_container_add (GTK_CONTAINER (window), box_main);

    /* Read program settings */
    settings = setts_read (dialog_data_get_cfg_file (dd_data), &err);

    /* Check window managers data config file for new wms and */
    /* get window manager data */
    if ((err = wms_check_for_new_wms ()) == ERR_OK) {
        wm = wms_get_wm_info (&err);
    }
    if (err != ERR_OK) {
        message_dialog_error (nullptr, err_get_message (err));
        settings_free_all (settings);
        settings_free_all (wm);
        g_application_quit (G_APPLICATION (app));
        return;
    }
    settings = setts_check_defaults (settings);

    widgets_set_settings (dd_data, settings, wm, &err);

    settings_free_all (wm);
    settings_free_all (settings);

    if (err != ERR_OK) {
        message_dialog_error (nullptr, err_get_message (err));
        g_application_quit (G_APPLICATION (app));
        return;
    }
    /* Set info about wchangerd daemon presence */
    daemon_monitor (dd_data);
    /* Add gtk thread for checking wchangerd daemon process presence */
    gdk_threads_add_timeout_full (G_PRIORITY_LOW,
                                  3000,
                                  daemon_monitor,
                                  dd_data,
                                  nullptr);

    gtk_window_set_application (GTK_WINDOW (window), GTK_APPLICATION (app));
    gtk_widget_show_all (window);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application shutdown signal.
 */
static void
shutdown ([[maybe_unused]] GtkApplication *application,
          DialogData     *dd_data)
{
    dialog_data_free (dd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Handling command line options.
 */
static gint
local_options ([[maybe_unused]] GApplication *application,
               GVariantDict *options,
               DialogData   *dd_data)
{
    char *str = nullptr;

    if (g_variant_dict_lookup (options, "config", "s", &str)) {
        dd_data->s_cfg_file = strdup (str);
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

    dd_data = dialog_data_new ();
    app = gtk_application_new ("org.nongnu.WallChanger",
                               G_APPLICATION_DEFAULT_FLAGS);
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

