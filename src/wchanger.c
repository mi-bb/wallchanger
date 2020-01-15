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
 * @date January 15, 2020
 *
 * @version 1.3.7
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdio.h> 
#include <stdint.h> 
#include <gtk/gtk.h>
#include "settlist.h"
#include "setts.h"
#include "iminfo.h"
#include "wpset.h"
#include "imgs.h"
#include "dlgs.h"
#include "treev.h"
#include "preview.h"
#include "strfun.h"
#include "errs.h"
#include "cfgfile.h"
#include "defs.h"
#include "fdops.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct DialogData
 *
 * @brief  Structure to pass widgets and settings to callback
 *
 * @var   DialogData::gw_window
 * @brief Pointer to main config dialog window
 * @var   DialogData::gw_view
 * @brief Pointer to TreeView with wallpaper list
 * @var   DialogData::gw_random
 * @brief Pointer to CheckBox for selecting random wallpapers
 * @var   DialogData::gw_lastused
 * @brief Pointer to CheckBox for restoring last used wallpaper on start
 * @var   DialogData::gw_command
 * @brief Pointer to Entry with command to set wallpaper
 * @var   DialogData::gw_interval
 * @brief Pointer to CheckBox for selecting wallpaper change interval
 * @var   DialogData::gw_inter_combo
 * @brief Pointer to ComboBox for selecting wallpapet change interval
 * @var   DialogData::s_cfg_file
 * @brief Array of chars for config file path
 */
typedef struct
DialogData {
    GtkWindow  *gw_window;       /* App window pointer */
    GtkWidget  *gw_view;         /* TreeView file list widget */
    GtkWidget  *gw_random;       /* Random background select check box */
    GtkWidget  *gw_lastused;     /* Set last used wallpaper check box */
    GtkWidget  *gw_command;      /* Set wallpaper command entry */
    GtkWidget  *gw_interval;     /* Time interval check button */
    GtkWidget  *gw_inter_combo;  /* Time interval combo box */
    char        s_cfg_file[256]; /* Config file string */
} DialogData;
/*----------------------------------------------------------------------------*/
/**
 * @fn  static uint32_t get_wallpaper_ch_interval (const DialogData *dd_data)
 * @brief         Get wallpaper change minutes interval from widgets
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        Change interval value
 *
 * @fn  static void set_wallpaper_ch_interval (DialogData    *dd_data,
 *                                             const uint32_t ui_val)
 * @brief         Set wallpaper change interval value to widgets
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @param[in]     ui_val   Change interval value to set
 * @return        none
 *
 * @fn  static void get_wallpaper_list (GtkWidget *gw_view, SettList  *st_list)
 * @brief      Insert list of wallpapers from treeview to SettList array.
 * @param[in]  gw_view  TreeView to read file list
 * @param[out] st_list  List with settings to insert file list
 * @return     none
 *
 * @fn  static void set_wallpaper_list (const SettList *sl_walls,
 *                                      GtkWidget      *gw_view)
 * @brief      Insert list of wallpapers from SettList array to treeview.
 * @param[in]  sl_walls List with wallpaper file paths
 * @param[out] gw_view  TreeView to insert file list
 * @return     none
 *
 * @fn  static SettList widgets_get_settings (const DialogData *dd_data)
 * @brief         Read settings from widgets and store them in WallSett object.
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void widgets_set_settings (DialogData     *dd_data,
 *                                        const SettList *st_list)
 * @brief         Loading data from SettList list of settings to program window.
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @param[in]     st_list  List with settings
 * @return        none
 *
 * @fn  static void event_add_img_pressed (GtkWidget  *widget,
 *                                         DialogData *dd_data)
 * @brief         Add images button pressed.
 * @param         widget   The object on which the signal is emitted
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void event_add_img_dir_pressed (GtkWidget  *widget,
 *                                             DialogData *dd_data)
 * @brief         Add images from folder button pressed.
 * @param         widget   The object on which the signal is emitted
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void event_rem_from_list_pressed (GtkWidget *widget,
 *                                               GtkWidget *gw_tview)
 * @brief         Remove items from list pressed.
 * @param         widget    The object on which the signal is emitted
 * @param[in,out] gw_tview  TreeView to remove items
 * @return        none
 *
 * @fn  static void event_move_up_pressed (GtkWidget *widget,
 *                                         GtkWidget *gw_tview)
 * @brief         Move up items pressed.
 * @param         widget    The object on which the signal is emitted
 * @param[in,out] gw_tview  TreeView to move items up
 * @return        none
 *
 * @fn  static void event_move_down_pressed (GtkWidget *widget,
 *                                           GtkWidget *gw_tview)
 * @brief         Move down items pressed.
 * @param         widget    The object on which the signal is emitted
 * @param[in,out] gw_tview  TreeView to move items down
 * @return        none
 *
 * @fn  static void event_sort_list_pressed (GtkWidget *widget,
 *                                           GtkWidget *gw_tview)
 * @brief         Sort wallpaper list button pressed.
 * @param         widget    The object on which the signal is emitted
 * @param[in,out] gw_tview  TreeView to sort items
 * @return        none
 *
 * @fn  static void event_rem_dupl_pressed (GtkWidget *widget,
 *                                          GtkWidget *gw_tview)
 * @brief         Remove duplicates button pressed.
 * @param         widget    The object on which the signal is emitted
 * @param[in,out] gw_tview  TreeView to remove duplicates
 * @return        none
 *
 * @fn  static void event_set_wallpaper_pressed (GtkWidget  *widget,
 *                                               DialogData *dd_data)
 * @brief         Set wallpaper button pressed.
 * @param         widget   The object on which the signal is emitted
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 *
 * @fn  static void event_save_settings_pressed (GtkWidget  *widget,
 *                                               DialogData *dd_data)
 * @brief         Save settings button pressed.
 * @param         widget   The object on which the signal is emitted
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return none
 *
 * @fn  static void event_img_list_activated (GtkTreeView       *tree_view,
 *                                            GtkTreePath       *path,
 *                                            GtkTreeViewColumn *column,
 *                                            GtkWidget         *gw_img_prev)
 * @brief      Make preview image widget of image (file path).
 * @param[in]  tree_view    The object on which the signal is emitted
 * @param[in]  path         The GtkTreePath for the activated row
 * @param      column       The GtkTreeViewColumn in which the activation
 *                          occurred
 * @param[out] gw_img_prev  Preview image widget
 * @return none
 *
 * @fn  static gboolean event_on_delete (GtkWidget  *window,
 *                                       GdkEvent   *event,
 *                                       DialogData *dd_data)
 * @brief     On main window delete, check settings.
 * @param[in] window   Window that received the signal
 * @param     event    Event that triggered signal
 * @param[in] dd_data  DialogData object with widgets and settings info
 * @return    Stop or propagate event further 
 *
 * @fn static void create_title_widget (GtkWidget **gw_widget)
 * @brief      Create top window title text.
 * @param[out] gw_widget  Widget to write data
 * @return     none
 *
 * @fn  static GtkWidget * create_image_button (const char   *s_label,
 *                                              const char   *s_hint,
 *                                              const IconImg i_but)
 * @brief     Create button with icon/label/hint.
 * @param[in] s_label  Button label
 * @param[in] s_hint   Button hint
 * @param[in] i_but    Icon number
 * @return    Button
 *
 * @fn  static void create_buttons_widget (GtkWidget **gw_widget,
 *                                         DialogData *dd_data)
 * @brief         Create side buttons widget.
 * @param[out]    gw_widget  Pointer to widget where to set buttons
 * @param[in,out] dd_data    DialogData object with widgets and
 *                           settings info
 * @return         none
 *
 * @fn  static void create_settings_widget (GtkWidget **gw_widget,
 *                                          DialogData *dd_data)
 * @brief         Creates widget with settings for wallpaper changing.
 * @param[out]    gw_widget  Pointer to destination widget
 * @param[in,out] dd_data    DialogData object with settings and
 *                           widget data
 * @return        none
 *
 * @fn  static void activate (GtkApplication *app, DialogData *dd_data)
 * @brief         Application activate signal.
 * @param[in,out] app      Pointer to GtkApplication
 * @param[in,out] dd_data  DialogData object with settings and widget data
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static uint32_t    get_wallpaper_ch_interval   (const DialogData  *dd_data);

static void        set_wallpaper_ch_interval   (DialogData        *dd_data,
                                                const uint32_t     ui_val);
/*----------------------------------------------------------------------------*/
static void        get_wallpaper_list          (GtkWidget         *gw_view,
                                                SettList          *st_list);

static void        set_wallpaper_list          (const SettList    *sl_walls,
                                                GtkWidget         *gw_view);
/*----------------------------------------------------------------------------*/
static SettList  * widgets_get_settings        (const DialogData  *dd_data);

static void        widgets_set_settings        (DialogData        *dd_data,
                                                const SettList    *st_list);
/*----------------------------------------------------------------------------*/
static void        event_add_img_pressed       (GtkWidget         *widget,
                                                DialogData        *dd_data);

static void        event_add_img_dir_pressed   (GtkWidget         *widget,
                                                DialogData        *dd_data);

static void        event_rem_from_list_pressed (GtkWidget         *widget,
                                                GtkWidget         *gw_tview);

static void        event_move_up_pressed       (GtkWidget         *widget,
                                                GtkWidget         *gw_tview);

static void        event_move_down_pressed     (GtkWidget         *widget,
                                                GtkWidget         *gw_tview);

static void        event_sort_list_pressed     (GtkWidget         *widget,
                                                GtkWidget         *gw_tview);

static void        event_rem_dupl_pressed      (GtkWidget         *widget,
                                                GtkWidget         *gw_tview);

static void        event_set_wallpaper_pressed (GtkWidget         *widget,
                                                DialogData        *dd_data);

static void        event_save_settings_pressed (GtkWidget         *widget,
                                                DialogData        *dd_data);

static void        event_about_app_pressed     (GtkWidget         *widget);

static void        event_interval_changed      (GtkSpinButton     *spin_button,
                                                DialogData        *dd_data);
/*----------------------------------------------------------------------------*/
static void        event_img_list_activated    (GtkTreeView       *tree_view,
                                                GtkTreePath       *path,
                                                GtkTreeViewColumn *column,
                                                GtkWidget         *gw_img_prev);
/*----------------------------------------------------------------------------*/
static gboolean    event_on_delete             (GtkWidget         *window,
                                                GdkEvent          *event,
                                                DialogData        *dd_data);
/*----------------------------------------------------------------------------*/
static void        create_title_widget         (GtkWidget        **gw_widget);

static GtkWidget * create_image_button         (const char        *s_label,
                                                const char        *s_hint,
                                                const IconImg      i_but);

static void        create_buttons_widget       (GtkWidget        **gw_widget,
                                                DialogData        *dd_data);

static void        create_settings_widget      (GtkWidget        **gw_widget,
                                                DialogData        *dd_data);
/*----------------------------------------------------------------------------*/
static void        activate                    (GtkApplication    *app,
                                                DialogData        *dd_data);
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
set_wallpaper_ch_interval (DialogData    *dd_data,
                           const uint32_t ui_val)
{
    uint32_t ui_tmp = ui_val; /* Temp inteval value */

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
 * @brief  Insert list of wallpapers from treeview to SettList array.
 */
static void
get_wallpaper_list (GtkWidget *gw_view,
                    SettList  *st_list)
{
    ImageInfo  *ii_info;
    Setting    *st_sett;
    GSList     *gsl_iinfo1 = NULL; /* List of ImageInfo data */
    GSList     *gsl_iinfo  = NULL; /* Temp list for data */
    const char *s_val      = NULL; /* Full file path */
    char       *s_name     = NULL; /* Unique name for background file */
    size_t      ul_no      = 0;    /* Number to append to name */

    /* get ImageInfo list of TreeView files */
    gsl_iinfo1 = treeview_get_data (gw_view);
    gsl_iinfo  = gsl_iinfo1;
    st_sett    = setting_new_array (get_setting_name (SETTING_WALL_ARRAY));

    stlist_insert_setting (st_list, st_sett);

    while (gsl_iinfo != NULL) {

        ii_info = gsl_iinfo->data;
        s_val   = ii_info->s_full_path;
        s_name  = str_name_with_number (get_setting_name (SETTING_WALL_ARRAY),
                                        ul_no++);
        st_sett = setting_new_string (s_val, s_name);

        free (s_name);
        stlist_insert_setting_to_array (st_list, st_sett,
                                        get_setting_name (SETTING_WALL_ARRAY));
        gsl_iinfo = gsl_iinfo->next;
    }
    g_slist_free_full (gsl_iinfo1, (GDestroyNotify) imageinfo_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert list of wallpapers from SettList array to treeview.
 */
static void
set_wallpaper_list (const SettList *sl_walls,
                    GtkWidget      *gw_view)
{
    treeview_add_items_settlist (gw_view, sl_walls);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read settings from widgets and store them in WallSett object.
 */
static SettList *
widgets_get_settings (const DialogData *dd_data)
{
    SettList   *st_list;
    Setting    *st_sett;
    const char *s_val = NULL;
    uint32_t    i_val = 0;

    st_list = stlist_new_list ();

    /* Get wallpaper list from treeview and add to settlist */
    get_wallpaper_list (dd_data->gw_view, st_list);

    /* Get random wallpaper setting */
    i_val = (uint32_t) gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (dd_data->gw_random));
    st_sett = setting_new_uint32 (
            i_val, get_setting_name (SETTING_RANDOM_OPT));
    stlist_insert_setting (st_list, st_sett);

    /* Get last used wallpaper on start setting */
    i_val = (uint32_t) gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (dd_data->gw_lastused));
    st_sett = setting_new_uint32 (
            i_val, get_setting_name (SETTING_LAST_USED_OPT));
    stlist_insert_setting (st_list, st_sett);

    /* Get wallpaper change interval setting */
    i_val = get_wallpaper_ch_interval (dd_data);
    st_sett = setting_new_uint32 (
            i_val, get_setting_name (SETTING_INTERVAL_VAL));
    stlist_insert_setting (st_list, st_sett);

    /* Get wallpaper set command */
    s_val = gtk_entry_get_text (GTK_ENTRY (dd_data->gw_command));
    st_sett = setting_new_string (
            s_val, get_setting_name (SETTING_BG_CMD));
    stlist_insert_setting (st_list, st_sett);

    return st_list;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Loading data from SettList list of settings to program window.
 */
static void
widgets_set_settings (DialogData     *dd_data,
                      const SettList *st_list)
{
    Setting  *st_sett;
    SettList *sl_walls;
    int       i_w = 0;
    int       i_h = 0;

    /* Set background set command */
    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_BG_CMD));
    if (st_sett != NULL) {
        gtk_entry_set_text (GTK_ENTRY (dd_data->gw_command),
                            setting_get_string (st_sett));
    }
    /* Set last used wallpaper on start setting */
    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_LAST_USED_OPT));
    if (st_sett != NULL) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_lastused),
                                      (gboolean) setting_get_uint32 (st_sett));
    }
    /* Set minutes/hours of wallpaper change interval */
    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_INTERVAL_VAL));
    if (st_sett != NULL) {
        set_wallpaper_ch_interval (dd_data, setting_get_uint32 (st_sett));
    }
    /* Set wallpaper random choose setting */
    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_RANDOM_OPT));
    if (st_sett != NULL) {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dd_data->gw_random),
                                      (gboolean) setting_get_uint32 (st_sett));
    }
    /* Get wallpaper list and add to treeview */
    sl_walls = stlist_get_settings_in_array_name_p (st_list,
            get_setting_name (SETTING_WALL_ARRAY));

    if (sl_walls != NULL) {
        set_wallpaper_list (sl_walls, dd_data->gw_view);
        stlist_free_p (sl_walls);
    }

    /* Set window dimensions */
    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_WIN_WIDTH));
    if (st_sett != NULL) {
        i_w = (int) setting_get_uint32 (st_sett);
    }
    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_WIN_HEIGHT));
    if (st_sett != NULL) {
        i_h = (int) setting_get_uint32 (st_sett);
    }
 
    gtk_window_set_default_size (GTK_WINDOW (dd_data->gw_window), i_w, i_h);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add images button pressed.
 */
static void
event_add_img_pressed (GtkWidget  *widget __attribute__ ((unused)),
                       DialogData *dd_data)
{
    GSList *gsl_files = NULL; /* List with files from dialog */

    /* Run image select dialog and get selected files */
    gsl_files = add_images_dialog (dd_data->gw_window);

    /* Add items to TreeView */
    if (gsl_files != NULL) {

        treeview_add_items_gslist (dd_data->gw_view, gsl_files);
        g_slist_free_full (gsl_files, g_free);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add images from folder button pressed.
 */
static void
event_add_img_dir_pressed (GtkWidget  *widget __attribute__ ((unused)),
                           DialogData *dd_data)
{
    char       *s_folder = NULL;  /* Selecred directory name */
    GList      *gl_files = NULL;  /* Images in directory */

    /* Run directory select dialog and get selected directory name */
    s_folder = add_images_folder_dialog (dd_data->gw_window);

    if (s_folder != NULL) {
        /* Scan directory for files and append them to file list */
        gl_files = get_dir_content_filter_images (s_folder);

        if (gl_files != NULL) {
            /* Add items to TreeView */
            treeview_add_items_glist (dd_data->gw_view, gl_files);
            g_list_free_full (gl_files, g_free);
        }
        g_free (s_folder);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove items from list pressed.
 */
static void
event_rem_from_list_pressed (GtkWidget *widget __attribute__ ((unused)),
                             GtkWidget *gw_tview)
{
    treeview_remove_selected (gw_tview);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move up items pressed.
 */
static void
event_move_up_pressed (GtkWidget *widget __attribute__ ((unused)),
                       GtkWidget *gw_tview)
{
    treeview_move_up (gw_tview);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Move down items pressed.
 */
static void
event_move_down_pressed (GtkWidget *widget __attribute__ ((unused)),
                         GtkWidget *gw_tview)
{
    treeview_move_down (gw_tview);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sort wallpaper list button pressed.
 */
static void
event_sort_list_pressed (GtkWidget *widget __attribute__ ((unused)),
                         GtkWidget *gw_tview)
{
    treeview_sort_list (gw_tview);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove duplicates button pressed.
 */
static void
event_rem_dupl_pressed (GtkWidget *widget __attribute__ ((unused)),
                        GtkWidget *gw_tview)
{
    treeview_remove_duplicates (gw_tview);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set wallpaper button pressed.
 */
static void
event_set_wallpaper_pressed (GtkWidget  *widget __attribute__ ((unused)),
                             DialogData *dd_data)
{
    GtkTreeSelection *gts_sele;
    GtkTreeModel     *gtm_model;
    ImageInfo        *ii_info;
    GtkTreeIter       gti_iter;
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
        s_cmd = gtk_entry_get_text (GTK_ENTRY (dd_data->gw_command));

        i_err = wallpaper_dialog_set (s_cmd,
                                      imageinfo_get_full_name (ii_info),
                                      dd_data->s_cfg_file);
        if (i_err != ERR_OK) {
            message_dialog_error (dd_data->gw_window, err_get_message (i_err));
        }
        imageinfo_free (ii_info);
    }
    g_list_free_full (gl_list, (GDestroyNotify) gtk_tree_path_free);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save settings button pressed.
 */
static void
event_save_settings_pressed (GtkWidget  *widget __attribute__ ((unused)),
                             DialogData *dd_data)
{
    SettList *st_list;
    int       i_err = 0;

    st_list = widgets_get_settings (dd_data);
    i_err   = settings_check_update_file (st_list, dd_data->s_cfg_file);

    stlist_free (st_list);

    if (i_err != ERR_OK) {
        message_dialog_error (dd_data->gw_window, err_get_message (i_err));
        /* g_application_quit (G_APPLICATION (dd_data)); */
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  About Wall Changer
 */
static void
event_about_app_pressed (GtkWidget  *widget __attribute__ ((unused)))
{
    about_app_dialog (APP_VERSION);
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

        preview_from_file (gw_img_prev, imageinfo_get_full_name (ii_info));
        imageinfo_free (ii_info);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Interval spin changed
 */
static void
event_interval_changed (GtkSpinButton *spin_button,
                        DialogData    *dd_data)
{
    static int i_prev = 0; /* Previous interval value */
    int        i_val  = 0; /* Actual interval value */
    int        i_act  = 0; /* Active interval combo index */

    i_val = gtk_spin_button_get_value_as_int (spin_button);

    if (i_prev == 1 && i_val != 1) {

        i_act = gtk_combo_box_get_active (
                GTK_COMBO_BOX (dd_data->gw_inter_combo));

        gtk_combo_box_text_remove_all (
                GTK_COMBO_BOX_TEXT (dd_data->gw_inter_combo));

        gtk_combo_box_text_append (
                GTK_COMBO_BOX_TEXT (dd_data->gw_inter_combo),
                NULL,
                "minutes");

        gtk_combo_box_text_append (
                GTK_COMBO_BOX_TEXT (dd_data->gw_inter_combo),
                NULL,
                "hours");

        gtk_combo_box_set_active (GTK_COMBO_BOX (dd_data->gw_inter_combo),
                                                 i_act);
    }
    else if (i_prev != 1 && i_val == 1) {

        i_act = gtk_combo_box_get_active (
                GTK_COMBO_BOX (dd_data->gw_inter_combo));

        gtk_combo_box_text_remove_all (
                GTK_COMBO_BOX_TEXT (dd_data->gw_inter_combo));

        gtk_combo_box_text_append (
                GTK_COMBO_BOX_TEXT (dd_data->gw_inter_combo),
                NULL,
                "minute");

        gtk_combo_box_text_append (
                GTK_COMBO_BOX_TEXT (dd_data->gw_inter_combo),
                NULL,
                "hour");

        gtk_combo_box_set_active (GTK_COMBO_BOX (dd_data->gw_inter_combo),
                                                 i_act);
    }

    i_prev = i_val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  On main window delete, check settings.
 */
static gboolean
event_on_delete (GtkWidget  *window,
                 GdkEvent   *event __attribute__ ((unused)),
                 DialogData *dd_data)
{
    GtkWidget *dialog;
    SettList  *st_list;
    char      *s_buff = NULL;
    int        i_res  = 0;
    int        i_err  = 0;
    int        i_w    = 0;
    int        i_h    = 0;

    gtk_window_get_size (GTK_WINDOW (window), &i_w, &i_h);

    st_list = widgets_get_settings (dd_data);
    s_buff  = settings_check_update (st_list, dd_data->s_cfg_file, &i_err);
    stlist_free (st_list);

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
            i_err = settings_update_file (s_buff, dd_data->s_cfg_file);
            if (i_err != ERR_OK) {
                message_dialog_error (GTK_WINDOW (window),
                                      err_get_message (i_err));
            }
        }
        free (s_buff);
    }

    settings_update_window_size (i_w, i_h, dd_data->s_cfg_file);
    return FALSE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create top window title text.
 */
static void
create_title_widget (GtkWidget **gw_widget)
{
    const char *s_str    = APP_NAME " - Program settings";
    const char *s_format = "<span size=\"20000\" weight=\"bold\" "
                           "foreground=\"#0099e6\" style=\"italic\">%s</span>";
    char       *s_markup = NULL;
    GtkWidget  *gw_label = gtk_label_new (NULL);

    *gw_widget = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);

    s_markup = g_markup_printf_escaped (s_format, s_str);
    gtk_label_set_markup (GTK_LABEL (gw_label), s_markup);
    g_free (s_markup);

    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_label,
                        FALSE, FALSE, 24);
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
    GtkWidget *gw_btn;
    GtkWidget *gw_img;
    GdkPixbuf *gd_pix = NULL;

    gw_btn = gtk_button_new ();

    if (s_label != NULL && strcmp (s_label, "") != 0)
        gtk_button_set_label (GTK_BUTTON (gw_btn), s_label);

    if (s_hint != NULL && strcmp (s_hint, "") != 0)
        gtk_widget_set_tooltip_text (gw_btn, s_hint);

    gd_pix = get_image (i_but);
    if (gd_pix != NULL) {
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
static void
create_buttons_widget (GtkWidget  **gw_widget,
                       DialogData  *dd_data)
{
    GtkWidget *gw_button;

    *gw_widget = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);

    gw_button = create_image_button (NULL, "Add images", W_ICON_ADD);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_add_img_pressed),
                      dd_data);
    gw_button = create_image_button (NULL, "Add images from folder",
                                     W_ICON_ADD_DIR);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_add_img_dir_pressed),
                      dd_data);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    gw_button = create_image_button (NULL, "Remove images", W_ICON_REMOVE);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_rem_from_list_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button (NULL, "Move up", W_ICON_UP);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_move_up_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button (NULL, "Move down", W_ICON_DOWN);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_move_down_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button (NULL, "Sort images", W_ICON_SORT);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_sort_list_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button (NULL, "Remove duplicates", W_ICON_DUPL);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_rem_dupl_pressed),
                      dd_data->gw_view);
    gw_button = create_image_button (NULL, "Set wallpaper", W_ICON_SCREEN);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_set_wallpaper_pressed),
                      dd_data);
    gw_button = create_image_button (NULL, "Save settings", W_ICON_FLOPPY);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_save_settings_pressed),
                      dd_data);
    gw_button = create_image_button (NULL, "About Wall Changer", W_ICON_INFO);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
    g_signal_connect (gw_button,
                      "clicked",
                      G_CALLBACK (event_about_app_pressed), NULL);
    gw_button = create_image_button (NULL, "Exit app", W_ICON_EXIT);
    g_signal_connect_swapped (gw_button,
                              "clicked",
                              G_CALLBACK (gtk_window_close),
                              dd_data->gw_window);
    gtk_box_pack_start (GTK_BOX (*gw_widget),
                        gw_button,
                        FALSE, FALSE, 4);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates widget with settings for wallpaper changing.
 */
static void
create_settings_widget (GtkWidget **gw_widget,
                        DialogData *dd_data)
{
    GtkWidget     *gw_random_button;     /* Random image GtkCheckButton */
    GtkWidget     *gw_button_selectlast; /* Set last used GtkCheckButton */
    GtkWidget     *gw_command_label;     /* Wallpaper set cmd GtkLabel */
    GtkWidget     *gw_command_entry;     /* Wallpaper set cmd GtkEntry */
    GtkWidget     *gw_interval_label;    /* Time interval GtkLabel */
    GtkWidget     *gw_spinbutton;        /* Time interval GtkSpinButton */
    GtkAdjustment *ga_adjustment;        /* Time interval GtkAdjustment */
    GtkWidget     *gw_time_combo;        /* Time interval GtkComboBoxText*/
    const char    *s_markup = NULL;      /* String for markups */

    /* Random wallpaper change button */
    s_markup = "When <b>enabled</b> wallpaper images will be selected "
        "randomly.\n"
        "When <b>disabled</b> wallpapers will be set in the same order as "
        "they appear on the list";
    gw_random_button = gtk_check_button_new ();
    gtk_widget_set_tooltip_markup (gw_random_button, s_markup);
    gtk_button_set_label (GTK_BUTTON (gw_random_button),
                          "Random wallpaper change");

    /* Select last used wallpaper button */
    s_markup = "When <b>enabled</b> first wallpaper set on program start will "
       " be the last one previously set.\n"
        "When <b>disabled</b> wallpaper will be set randomly or next in list "
        "to the last one before stop.";
    gw_button_selectlast = gtk_check_button_new ();
    gtk_widget_set_tooltip_markup (gw_button_selectlast, s_markup);
    gtk_button_set_label (GTK_BUTTON (gw_button_selectlast),
                          "Select last used wallpaper at start");

    /* Wallpaper set command entry */
    gw_command_entry = gtk_entry_new ();
    gw_command_label = gtk_label_new ("Background set command : ");

    s_markup = "This command will be executed to set background image\n"
        "e.g. <b>feh --bg-fill</b>\nFor more complex commands use <b>[F]</b>" 
        " as a file name\ne.g. <b>feh --bg-fill [F]</b>";
    gtk_widget_set_tooltip_markup (gw_command_entry, s_markup);

    /* Background change interval widgets */
    s_markup = "Time between background changes";
    ga_adjustment = gtk_adjustment_new (30.0, 1.0, 6000.0, 1.0, 5.0, 0.0);
    gw_spinbutton = gtk_spin_button_new (ga_adjustment, 1.0, 0);
    gtk_widget_set_tooltip_markup (gw_spinbutton, s_markup);
    gw_interval_label = gtk_label_new ("Background change interval : ");
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

    /* Setting pointers in DialogData */
    dd_data->gw_random      = gw_random_button;
    dd_data->gw_lastused    = gw_button_selectlast;
    dd_data->gw_command     = gw_command_entry;
    dd_data->gw_interval    = gw_spinbutton;
    dd_data->gw_inter_combo = gw_time_combo;

    /* Container for settings widgets */
    *gw_widget = gtk_grid_new ();
    gtk_grid_set_column_spacing (GTK_GRID (*gw_widget), 8);
    gtk_grid_set_row_spacing (GTK_GRID (*gw_widget), 8);

    /* Packing button for random change */
    gtk_grid_attach (GTK_GRID (*gw_widget),
                     gw_random_button, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_widget),
                             gw_button_selectlast, gw_random_button,
                             GTK_POS_BOTTOM, 1, 1);

    /* Packing background set command */
    gtk_grid_attach_next_to (GTK_GRID (*gw_widget),
                             gw_command_label, gw_random_button,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_widget),
                             gw_command_entry, gw_command_label,
                             GTK_POS_RIGHT, 1, 1);

    /* Packing time interval widgets */
    gtk_grid_attach_next_to (GTK_GRID (*gw_widget),
                             gw_interval_label, gw_button_selectlast,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_widget),
                             gw_spinbutton, gw_interval_label,
                             GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_widget),
                             gw_time_combo, gw_spinbutton,
                             GTK_POS_RIGHT, 1, 1);
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
    SettList   *st_list;
    Setting    *st_sett;
    const char *s_lastused = NULL;  /* Last used wallpaper path */
    int         i_err      = 0;     /* For error output */

    memset (dd_data->s_cfg_file, 0, sizeof (dd_data->s_cfg_file));

    i_err = set_config_file_path (dd_data->s_cfg_file);

    if (i_err != ERR_OK) {
        message_dialog_error (NULL, err_get_message (i_err));
        g_application_quit (G_APPLICATION (app));
        return;
    }
    
    /* Image preview widget */
    gw_img_prev = gtk_image_new ();

    /* Main window */
    gw_window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (gw_window),
                          APP_NAME " v" APP_VERSION);
    g_signal_connect (gw_window, "delete-event",
                  G_CALLBACK (event_on_delete), dd_data);
    dd_data->gw_window = GTK_WINDOW (gw_window);

    create_tview (&gw_tview);
    dd_data->gw_view = gw_tview;
    g_signal_connect (gw_tview, "row-activated",
                      G_CALLBACK (event_img_list_activated), gw_img_prev);

    GdkPixbuf *gd_pix = NULL;
    gd_pix = get_image (W_ICON_ABOUT);
    if (gd_pix != NULL) {
        gtk_window_set_default_icon (gd_pix);
        g_object_unref (gd_pix);
    }
    create_title_widget (&gw_title_widget);

    create_buttons_widget (&gw_buttons_widget, dd_data);

    preview_from_file (gw_img_prev, NULL);

    create_settings_widget (&gw_settings_widget, dd_data);

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
                        gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_box_main),
                        gw_settings_widget,
                        FALSE, FALSE, 4);

    gtk_container_set_border_width (GTK_CONTAINER (gw_box_main), 10);
    gtk_container_add (GTK_CONTAINER (gw_window), gw_box_main);

    st_list = settings_read (dd_data->s_cfg_file, &i_err);
    if (i_err != ERR_OK) {
        message_dialog_error (NULL, err_get_message (i_err));
        stlist_free (st_list);
        g_application_quit (G_APPLICATION (app));
        return;
    }

    settlist_check_defaults (st_list);

    widgets_set_settings (dd_data, st_list);

    st_sett = stlist_get_setting_with_name (
            st_list, get_setting_name (SETTING_LAST_USED_STR));

    if (st_sett != NULL)
        s_lastused = setting_get_string (st_sett);

    /* Check if there is last used wallpaper info, if it is select it on
     * list and make a preview image */
    if (s_lastused != NULL) {
        preview_from_file (gw_img_prev, s_lastused);
        treeview_find_select_item (gw_tview, s_lastused);
    }

    stlist_free (st_list);

    gtk_window_set_application (GTK_WINDOW (gw_window), GTK_APPLICATION (app));
    gtk_widget_show_all (gw_window);
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
    int             status;
    DialogData      dd_data;

    app = gtk_application_new ("org.nongnu.WallChanger",
                               G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), &dd_data);
    g_set_application_name (APP_NAME);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
/*----------------------------------------------------------------------------*/


