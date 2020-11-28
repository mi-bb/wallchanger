/**
 * @file  dlgcmd.c
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
 * @brief  Wallpaper set command dialog
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include "errs.h"
#include "wpset.h"
#include "iminfo.h"
#include "dlgsmsg.h"
#include "strfun.h"
#include "wmsfn.h"
#include "setting.h"
#include "dlgcmd.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   PREV_LEN
 * @brief Length of file name in preview combobox.
 */
#define PREV_LEN 47
/*----------------------------------------------------------------------------*/
/**
 * @brief  Columns for combobox with window manager list.
 */
enum e_wm_columns {
    WM_COLUMN_NAME,    /**< Combobox ListStore column for window manager name */
    WM_COLUMN_COMMAND, /**< Combobox ListStore column for wallp set command */
    WM_COLUMN_COUNT    /**< Column count */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Columns for combobox with preview wallpapers.
 */
enum e_preview_columns {
    PREV_NAME_SHOW,   /**< Combobox ListStore column for shown name */
    PREV_NAME_FULL,   /**< Combobox ListStore column for full file path */
    PREV_COLUMN_COUNT /**< Column count */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Run Xfce wallpaper set command configuration dialog.
 *
 * @param[in] gw_parent  Window widget for setting dialog modal
 * @return    String with wallpaper set command or null. It should be
 *            freed after use using free.
 */
static char * xfce_dialog_run (GtkWindow *gw_parent);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set active combobox position based on wm's name.
 *
 * @param[in,out] gw_combo ComboBox item
 * @param[in]     s_name   Window manager's name
 * @return        none
 */
static void
combo_set_active_by_wm_name (GtkWidget  *gw_combo,
                             const char *s_name)
{
    GtkTreeModel *gtm_model;
    GtkTreeIter   iter;
    int           i_res   = 0;    /* Getting iter result */
    char         *s_cname = NULL; /* For wm name from combobox */

    gtm_model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
    i_res     = gtk_tree_model_get_iter_first (gtm_model, &iter);

    while (i_res) {
        gtk_tree_model_get (gtm_model, &iter, WM_COLUMN_NAME, &s_cname, -1);
        if (strcmp (s_name, s_cname) == 0) {
            gtk_combo_box_set_active_iter (GTK_COMBO_BOX (gw_combo), &iter);
            free (s_cname);
            break;
        }
        free (s_cname);
        i_res = gtk_tree_model_iter_next (gtm_model, &iter);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set text in window manager search label.
 *
 * @param[out] gw_label  Label to set text
 * @param[in]  s_txt     Text to set in label
 * @return     none
 */
static void
wm_label_set_text (GtkWidget  *gw_label,
                   const char *s_txt)
{
    if (strcmp (s_txt, "Unknown") == 0) {
        gtk_label_set_markup (GTK_LABEL (gw_label),
            "<span font_size=\"large\">Could not find the window manager that "
            "is currently in use.</span>");
    }
    else {
        char *s_markup = g_markup_printf_escaped (
            "<span foreground=\"#0099e6\" font_size=\"large\">Found window "
            "maganer: <span weight=\"bold\">%s</span></span>",
            s_txt);
        gtk_label_set_markup (GTK_LABEL (gw_label), s_markup);
        free (s_markup);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set text in TextView.
 *
 * @param[in,out] gw_tview  TextView widget
 * @param[in]     s_txt     text to set
 * @return        none
 */
static void
textview_set_text (GtkWidget  *gw_tview,
                   const char *s_txt)
{
    GtkTextBuffer *buffer;

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (gw_tview));
    gtk_text_buffer_set_text (buffer, s_txt, -1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get text from TextView.
 *
 * @param[in,out] gw_tview  TextView widget
 * @return        String with text, it should be freed using free after use
 */
static char *
textview_get_text (GtkWidget *gw_tview)
{
    char          *s_txt = NULL;
    GtkTextBuffer *buffer;
    GtkTextIter    it_start;
    GtkTextIter    it_end;

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (gw_tview));
    gtk_text_buffer_get_start_iter (buffer, &it_start);
    gtk_text_buffer_get_end_iter (buffer, &it_end);
    s_txt = gtk_text_buffer_get_text (buffer, &it_start, &it_end, FALSE);
    newline_to_space (s_txt);

    return s_txt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates ComboBox with window managers.
 *
 * @param[in] st_wmlist  List with window manager info.
 * @return    ComboBox widget
 */
static GtkWidget *
wm_combo (Setting *st_wmlist)
{
    GtkWidget       *gw_combo;       /* Result ComboBox */
    GtkCellRenderer *cell;           /* CellRenderer */
    GtkListStore    *list_store;     /* ListStore for window manager data */
    GtkTreeIter      iter;           /* TreeIter */
    Setting         *st_wm   = NULL; /* Window manager info list */
    Setting         *st_cmd  = NULL; /* For wallpaper set command */
    Setting         *st_unkn = NULL; /* For setting with unknown wm */
    const char      *s_name  = NULL; /* For window manager name string */
    const char      *s_cmd   = NULL; /* For wallpaper set command string */

    list_store = gtk_list_store_new (WM_COLUMN_COUNT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store),
                                          WM_COLUMN_NAME, GTK_SORT_ASCENDING);
    st_wm = st_wmlist;

    while (st_wm != NULL) {
        s_name = setting_get_name (st_wm);
        if (strcmp (s_name, "Unknown") == 0) {
            st_unkn = st_wm;
            st_wm = st_wm->next;
            continue;
        }
        if ((st_cmd = setting_find_child (st_wm, "Command")) != NULL)
            s_cmd = setting_get_string (st_cmd);

        gtk_list_store_append (list_store, &iter);
        gtk_list_store_set (list_store, &iter,
                            WM_COLUMN_NAME,    s_name,
                            WM_COLUMN_COMMAND, s_cmd,
                            -1);
        st_wm = st_wm->next;
    }

    gtk_tree_sortable_set_sort_column_id (
            GTK_TREE_SORTABLE (list_store),
            GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID,
            GTK_SORT_ASCENDING);

    s_name = setting_get_name (st_unkn);
    if ((st_cmd = setting_find_child (st_unkn, "Command")) != NULL)
        s_cmd = setting_get_string (st_cmd);
    gtk_list_store_prepend (list_store, &iter);
    gtk_list_store_set (list_store, &iter,
                                WM_COLUMN_NAME,    s_name,
                                WM_COLUMN_COMMAND, s_cmd,
                                -1);

    gw_combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

    g_object_unref (G_OBJECT (list_store));

    cell = gtk_cell_renderer_text_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (gw_combo), cell, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (gw_combo),
                                   cell, "text", WM_COLUMN_NAME);
    return gw_combo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates ComboBox with wallpaper list.
 *
 * @param[in]  gsl_iinfo List with wallpaper ImageInfo items.
 * @return     ComboBox widget
 */
static GtkWidget *
preview_combo (const GSList *gsl_iinfo)
{
    GtkWidget       *gw_combo;      /* Result ComboBox */
    GtkCellRenderer *cell;          /* CellRenderer */
    GtkListStore    *list_store;    /* ListStore for wallpaper data */
    ImageInfo       *ii_info;       /* ImageInfo for gsl_iinfo data */
    GtkTreeIter      iter;          /* TreeIter */
    const GSList    *gl_ii = NULL;  /* List pointer copy */
    int              i_cnt = 0;     /* For wallpaper counting */
    char s_prev [PREV_LEN * 4 + 4]; /* Buffer for file name length checking */

    list_store = gtk_list_store_new (PREV_COLUMN_COUNT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);
    gl_ii = gsl_iinfo;

    while (gl_ii != NULL && i_cnt++ < 10) {

        ii_info = (ImageInfo *) gl_ii->data;
        /* Check file name length and shrink to PREV_LEN if it is longer
         * so ComboBox will not be so wide with longer names */
        if (g_utf8_strlen (ii_info->s_file_name, -1) > PREV_LEN) {
            g_utf8_strncpy (s_prev, ii_info->s_file_name, PREV_LEN);
            strcat (s_prev, "...");
        }
        else {
            strcpy (s_prev, ii_info->s_file_name);
        }
        gtk_list_store_append (list_store, &iter);
        gtk_list_store_set (list_store, &iter,
                                PREV_NAME_SHOW, s_prev,
                                PREV_NAME_FULL, ii_info->s_file_path,
                                -1);
        gl_ii = gl_ii->next;
    }
    gw_combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

    g_object_unref (G_OBJECT (list_store));

    cell = gtk_cell_renderer_text_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (gw_combo), cell, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (gw_combo),
                                   cell, "text", PREV_NAME_SHOW);

    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter))
        gtk_combo_box_set_active_iter (GTK_COMBO_BOX (gw_combo), &iter);

    return gw_combo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clicked button to get wm's default saved wallpaper set command.
 *
 * @param[in] gw_array  Array with combo and entry widgets
 * @return    none
 */
static void
event_get_saved_button_clicked (GtkWidget **gw_array)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    GtkWidget    *gw_combo;         /* ComboBox with window manager info */
    GtkWidget    *gw_tview;         /* Textview with wallpaper set command */
    char         *s_cmd     = NULL; /* Wallpaper set command */

    gw_combo  = gw_array[0];
    gw_tview  = gw_array[2];

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter, WM_COLUMN_COMMAND, &s_cmd, -1);

        if (s_cmd != NULL) {
            textview_set_text (gw_tview, s_cmd);
            free (s_cmd);
        }
        else {
            textview_set_text (gw_tview, "");
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clicked button to get wm's application default wallpaper set command.
 *
 * @param[in] gw_array  Array with combo and entry widgets
 * @return    none
 */
static void
event_get_default_button_clicked (GtkWidget **gw_array)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    GtkWidget    *gw_combo;           /* ComboBox with window managers */
    GtkWidget    *gw_tview;           /* Textview with wallpaper set command */
    GtkWindow    *gw_parent;          /* Parent window for dialog */
    char         *s_name      = NULL; /* Window manager name */
    char         *s_cmd       = NULL; /* Wallpaper set command */
    Setting      *st_settings = NULL; /* For window manager setting list */
    Setting      *st_item     = NULL; /* For checking wm name and command */
    int i_err = 0;

    gw_combo  = gw_array[0];
    gw_tview  = gw_array[2];
    gw_parent = GTK_WINDOW (gw_array[3]);

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {

        /* Load settings from deffiles template */
        st_settings = wms_get_wm_info_data (&i_err);
        if (i_err != ERR_OK) {
            message_dialog_error (gw_parent, err_get_message (i_err));
            return;
        }
        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter, WM_COLUMN_NAME, &s_name, -1);

        /* If wm is Xfce select diaplay for command */
        if (strcmp (s_name, "Xfce") == 0) {
            s_cmd = xfce_dialog_run (gw_parent);
        }
        /* Find selected wm in default info data */
        else if ((st_item = setting_find_child (st_settings, s_name)) != NULL) {
            if ((st_item = setting_find_child (st_item, "Command")) != NULL) {
                s_cmd = strdup (setting_get_string (st_item));
            }
        }
        /* Set command if it is OK */
        if (s_cmd != NULL) {
            textview_set_text (gw_tview, s_cmd);
            free (s_cmd);
        }
        free (s_name);
        settings_free_all (st_settings);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clicked button to save wallpaper set command for selected wm.
 *
 * @param[in] gw_array  Array with combo and entry widgets
 * @return    none
 */
static void
event_save_command_button_clicked (GtkWidget **gw_array)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    GtkWidget    *gw_combo;         /* ComboBox with window managers */
    GtkWidget    *gw_tview;         /* Textview with wallpaper set command */
    GtkWindow    *gw_parent;        /* Parent window for dialog */
    char         *s_name    = NULL; /* Window manager name */
    char         *s_command = NULL; /* String for wallpaper command */
    int           i_err     = 0;    /* Error output */

    gw_combo  = gw_array[0];
    gw_tview  = gw_array[2];
    gw_parent = GTK_WINDOW (gw_array[3]);

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter, WM_COLUMN_NAME, &s_name, -1);

        s_command = textview_get_text (gw_tview);

        if ((i_err = wms_update_wm_command (s_name, s_command)) == ERR_OK) {
            gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                                WM_COLUMN_COMMAND, s_command,
                                -1);
        }
        else {
            /* err (EXIT_FAILURE, NULL); */
            message_dialog_error (gw_parent, err_get_message (i_err));
        }
        free (s_name);
        free (s_command);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clicked button to test wallpaper set command.
 *
 * @param[in] gw_array  Array with combo and entry widgets
 * @return    none
 */
static void
event_test_button_clicked (GtkWidget **gw_array)
{
    GtkTreeModel *model;            /* TreeModel */
    GtkTreeIter   iter;             /* TreeIter */
    GtkWidget    *gw_combo;         /* ComboBox with wallpapers */
    GtkWidget    *gw_tview;         /* Textview with command */
    char         *s_file    = NULL; /* String for wallpaper file */
    char         *s_command = NULL; /* String for wallpaper command */

    gw_combo = gw_array[1];
    gw_tview = gw_array[2];

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {
        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter, PREV_NAME_FULL, &s_file, -1 );
    }
    s_command = textview_get_text (gw_tview);

    if (s_command != NULL && s_file != NULL) {
        wallpaper_test_set (s_command, s_file);
    }
    free (s_command);
    free (s_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create textview for command display.
 *
 * @return  TextView widget
 */
static GtkWidget *
create_command_textview (void)
{
    GtkWidget *gw_tview; /* TextView to return */

    gw_tview = gtk_text_view_new ();

    /* Settings for command treeview */
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (gw_tview), GTK_WRAP_WORD);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (gw_tview), TRUE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (gw_tview), TRUE);
    gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (gw_tview), FALSE);
    gtk_text_view_set_monospace (GTK_TEXT_VIEW (gw_tview), TRUE);
    gtk_text_view_set_border_window_size (GTK_TEXT_VIEW (gw_tview),
                                          GTK_TEXT_WINDOW_LEFT, 8);
    gtk_text_view_set_border_window_size (GTK_TEXT_VIEW (gw_tview),
                                          GTK_TEXT_WINDOW_RIGHT, 8);
    gtk_text_view_set_border_window_size (GTK_TEXT_VIEW (gw_tview),
                                          GTK_TEXT_WINDOW_TOP, 8);
    gtk_text_view_set_border_window_size (GTK_TEXT_VIEW (gw_tview),
                                          GTK_TEXT_WINDOW_BOTTOM, 8);
    return gw_tview;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Run Xfce wallpaper set command configuration dialog.
 *
 * @param[in] gw_parent  Window widget for setting dialog modal
 * @return    String with wallpaper set command or null. It should be
 *            freed after use using free.
 */
static char *
xfce_dialog_run (GtkWindow *gw_parent)
{
    GtkWidget   *gw_dialog;              /* Xfce command dialog */
    GtkWidget   *gw_tlabel;              /* Title label */
    GtkWidget   *gw_content_box;         /* Dialog's box */
    GtkWidget   *gw_radiobtn     = NULL; /* Radiobutton */
    GtkToggleButton *gtb_btn     = NULL; /* ToggleButton */
    GSList      *gs_list         = NULL; /* List for buttons */
    char        *s_res           = NULL; /* Result command string */
    int          i_res           = 0;    /* Dialog result */
    char       **s_displays      = NULL; /* Null term. list of xfce displays */
    char       **s_it            = NULL; /* For display list iteration */
    const char  *s_disp          = NULL; /* Selected display string */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    gw_dialog = gtk_dialog_new_with_buttons (
                                      "Xfce wallpaper set command configuraion",
                                      gw_parent,
                                      flags,
                                      "_OK",
                                      GTK_RESPONSE_ACCEPT,
                                      "_Cancel",
                                      GTK_RESPONSE_REJECT,
                                      NULL);

    gw_content_box = gtk_dialog_get_content_area (GTK_DIALOG (gw_dialog));
    gtk_container_set_border_width (GTK_CONTAINER (gw_content_box), 8);

    s_displays = wms_get_xfce_display_list ();
    gw_tlabel  = *s_displays == NULL ?
                 gtk_label_new ("Could not find Xfce screens") :
                 gtk_label_new ("Select display to set wallpaper:");

    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_tlabel,
                        FALSE, FALSE, 4);

    for (s_it = s_displays; *s_it != NULL; ++s_it) {
        gw_radiobtn = gtk_radio_button_new_with_label_from_widget (
                GTK_RADIO_BUTTON (gw_radiobtn), *s_it);
        gtk_widget_set_name (gw_radiobtn, *s_it);

        gtk_box_pack_start (GTK_BOX (gw_content_box), gw_radiobtn,
                        FALSE, FALSE, 4);
    }
    wms_free_xfce_display_list (s_displays);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT && gw_radiobtn != NULL) {
        gs_list = gtk_radio_button_get_group (GTK_RADIO_BUTTON (gw_radiobtn));
        while (gs_list != NULL) {
            gtb_btn = gs_list->data;
            if (gtk_toggle_button_get_active (gtb_btn)) {
                s_disp = gtk_widget_get_name (GTK_WIDGET (gtb_btn));
                break;
            }
            gs_list = gs_list->next;
        }
    }
    if (s_disp != NULL) {
        s_res = wms_get_xfce_command (s_disp);
    }
    gtk_widget_destroy (gw_dialog);

    return s_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Run wallpaper set command configuration dialog.
 */
char *
cmddialog_run (GtkWindow    *gw_parent,
               const char   *s_current_cmd,
               const GSList *gsl_iinfo)
{
    GtkWidget *gw_dialog;         /* Wallpaper set command dialog */
    GtkWidget *gw_content_box;    /* Dialog's box */
    GtkWidget *gw_wm_label;       /* Finding window manager label */
    GtkWidget *gw_combo_label;    /* Label for window manager combo */
    GtkWidget *gw_wm_combo;       /* ComboBox with window managers */
    GtkWidget *gw_get_sav_btn;    /* Button for getting saved app wall cmd */
    GtkWidget *gw_get_def_btn;    /* Button for getting app default wall cmd */
    GtkWidget *gw_save_def_btn;   /* Button for saving default wall cmd */
    GtkWidget *gw_wm_box;         /* Box for window manager combo and button */
    GtkWidget *gw_tview;          /* Wallpaper set command textview */
    GtkWidget *gw_preview_combo;  /* ComboBox with wallpapers */
    GtkWidget *gw_test_btn;       /* Button for testing wallpaper command */
    GtkWidget *gw_test_box;       /* Box for test button and wallpaper list */
    GtkWidget *gw_array[4];       /* Array with widgets for clicked events */
    const Setting *st_crwm = NULL;/* Current window manager info */
    Setting   *st_wms   = NULL;   /* Window manager info list */
    char      *s_result = NULL;   /* Result string with wall set command */
    int        i_res    = 0;      /* Config dialog result */
    int        i_err    = ERR_OK; /* Error output */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    st_wms = wms_get_wm_info (&i_err);

    if (i_err != ERR_OK) {
        message_dialog_error (gw_parent, err_get_message (i_err));
        return NULL;
    }
    gw_dialog = gtk_dialog_new_with_buttons (
                                      "Wallpaper set command configuraion",
                                      gw_parent,
                                      flags,
                                      "_OK",
                                      GTK_RESPONSE_ACCEPT,
                                      "_Cancel",
                                      GTK_RESPONSE_REJECT,
                                      NULL);

    gw_wm_label      = gtk_label_new (NULL);
    gw_tview         = create_command_textview ();
    gw_wm_combo      = wm_combo (st_wms);
    gw_get_sav_btn   = gtk_button_new_with_label ("Get saved");
    gw_get_def_btn   = gtk_button_new_with_label ("Get app default");
    gw_save_def_btn  = gtk_button_new_with_label (
            "Save command for selected window manager");
    gw_test_btn      = gtk_button_new_with_label ("Test");
    gw_test_box      = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gw_wm_box        = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gw_content_box   = gtk_dialog_get_content_area (GTK_DIALOG (gw_dialog));
    gw_preview_combo = preview_combo (gsl_iinfo);
    gw_combo_label   = gtk_label_new (
            "List of window managers with provided wallpaer set commands:");

    gtk_widget_set_tooltip_markup (gw_get_sav_btn,
            "Get wallpaper set command for selected window manager "
            "(from user's config file).");
    gtk_widget_set_tooltip_markup (gw_get_def_btn,
            "Get wallpaper set command for selected window manager "
            "(application default).");
    gtk_widget_set_tooltip_markup (gw_save_def_btn,
            "Save wallpaper set command for selected window manager "
            "(in user's config file).");

    gtk_container_set_border_width (GTK_CONTAINER (gw_content_box), 8);

    gw_array[0] = gw_wm_combo;
    gw_array[1] = gw_preview_combo;
    gw_array[2] = gw_tview;
    gw_array[3] = gw_dialog;

    /* Packing window manager box */
    gtk_box_pack_start (GTK_BOX (gw_wm_box), gw_wm_combo,
                        TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (gw_wm_box), gw_get_sav_btn,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (gw_wm_box), gw_get_def_btn,
                        FALSE, FALSE, 0);

    /* Setting command from main window */
    if (s_current_cmd != NULL)
        textview_set_text (gw_tview, s_current_cmd);

    /* Trying to detect what window manager is in use and set wm label text and
     * combobox active index to found wm */
    if ((st_crwm = wms_get_current_wm (st_wms)) != NULL) {
        combo_set_active_by_wm_name (gw_wm_combo, setting_get_name (st_crwm));
        wm_label_set_text (gw_wm_label, setting_get_name (st_crwm));
    }
    settings_free_all (st_wms);

    /* Packing wallpaper command test box with test button and combobox with
     * sample wallpapers*/
    gtk_box_pack_start (GTK_BOX (gw_test_box), gw_test_btn,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (gw_test_box), gw_preview_combo,
                        TRUE, TRUE, 0);

    /* Found window manager labels */
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new ("Looking for window manager..."),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_wm_label,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    /* Window manager label and combobox */
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_combo_label,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_wm_box,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    /* Wallpaper set command label, treeview, save command buttton */
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new ("Wallpaper set command:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_tview,
                        TRUE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_save_def_btn,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    /* Testing label, and box with combo and button */
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                     gtk_label_new ("Test command using wallpaper from list:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_test_box,
                        FALSE, FALSE, 4);

    g_signal_connect_swapped (gw_get_sav_btn, "clicked",
                              G_CALLBACK (event_get_saved_button_clicked),
                              gw_array);

    g_signal_connect_swapped (gw_get_def_btn, "clicked",
                              G_CALLBACK (event_get_default_button_clicked),
                              gw_array);

    g_signal_connect_swapped (gw_save_def_btn, "clicked",
                              G_CALLBACK (event_save_command_button_clicked),
                              gw_array);

    g_signal_connect_swapped (gw_test_btn, "clicked",
                              G_CALLBACK (event_test_button_clicked),
                              gw_array);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT) {
        s_result = textview_get_text (gw_tview);
    }
    gtk_widget_destroy (gw_dialog);

    return s_result;
}
/*----------------------------------------------------------------------------*/

