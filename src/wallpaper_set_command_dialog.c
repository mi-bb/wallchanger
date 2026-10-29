/**
 * @file  wallpaper_set_command_dialog.c
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
 * @brief  Wallpaper set command dialog
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include "errors.h"
#include "wallpaper_set.h"
#include "image_info.h"
#include "message_dialogs.h"
#include "string_functions.h"
#include "window_manager_info.h"
#include "setting_item.h"
#include "wallpaper_set_command_dialog.h"
/*----------------------------------------------------------------------------*/
/**
 * @var   PREV_LEN
 * @brief Length of file name in preview combobox.
 */
constexpr int PREV_LEN = 47;
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
 * @brief  Widgets for command dialog.
 */
enum gw_cmd_widgets {
    GW_CMD_WM_COMBO,   /**< Combobox with window manager list */
    GW_CMD_PREV_COMBO, /**< Combobox with preview wallpapers */
    GW_CMD_TREEVIEW,   /**< Treeview for command textview */
    GW_CMD_DIALOG,     /**< Command dialog */
    GW_CMD_CNT         /**< Array widgets count */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Run Xfce wallpaper set command configuration dialog.
 *
 * @param[in] parent  Window widget for setting dialog modal
 * @return    String with wallpaper set command or null. It should be
 *            freed after use using free.
 */
static char * xfce_dialog_run (GtkWindow *parent);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set active combobox position based on wm's name.
 *
 * @param[in,out] combo ComboBox item
 * @param[in]     name  Window manager's name
 * @return        none
 */
static void
combo_set_active_by_wm_name (GtkWidget  *combo,
                             const char *name)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    int           res        = 0;       /* Getting iter result */
    char         *combo_name = nullptr; /* For wm name from combobox */

    model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));
    res   = gtk_tree_model_get_iter_first (model, &iter);

    while (res) {
        gtk_tree_model_get (model, &iter, WM_COLUMN_NAME, &combo_name, -1);
        if (strcmp (name, combo_name) == 0) {
            gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);
            free (combo_name);
            break;
        }
        free (combo_name);
        res = gtk_tree_model_iter_next (model, &iter);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set text in window manager search label.
 *
 * @param[out] label  Label to set text
 * @param[in]  text   Text to set in label
 * @return     none
 */
static void
wm_label_set_text (GtkWidget  *label,
                   const char *text)
{
    if (strcmp (text, "Unknown") == 0) {
        gtk_label_set_markup (GTK_LABEL (label),
            "<span font_size=\"large\">Could not find the window manager that "
            "is currently in use.</span>");
    }
    else {
        char *markup = g_markup_printf_escaped (
            "<span foreground=\"#0099e6\" font_size=\"large\">Found window "
            "maganer: <span weight=\"bold\">%s</span></span>",
            text);
        gtk_label_set_markup (GTK_LABEL (label), markup);
        free (markup);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set text in TextView.
 *
 * @param[in,out] textview  TextView widget
 * @param[in]     text      text to set
 * @return        none
 */
static void
textview_set_text (GtkWidget  *textview,
                   const char *text)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    gtk_text_buffer_set_text (buffer, text, -1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get text from TextView.
 *
 * @param[in,out] textview  TextView widget
 * @return        String with text, it should be freed using free after use
 */
static char *
textview_get_text (GtkWidget *textview)
{
    char          *text = nullptr; /* String to return */
    GtkTextBuffer *buffer;         /* TextBuffer */
    GtkTextIter    start;          /* Start of text */
    GtkTextIter    end;            /* End of text */

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);
    text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
    newline_to_space (text);

    return text;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates ComboBox with window managers.
 *
 * @param[in] wm_list  List with window manager info.
 * @return    ComboBox widget
 */
static GtkWidget *
wm_combo (Setting *wm_list)
{
    GtkWidget       *combo;           /* Result ComboBox */
    GtkCellRenderer *cell;            /* CellRenderer */
    GtkListStore    *list_store;      /* ListStore for window manager data */
    GtkTreeIter      iter;            /* TreeIter */
    Setting         *wm           = nullptr; /* Window manager info list */
    Setting         *cmd_setting  = nullptr; /* For wallpaper set command */
    Setting         *unknown      = nullptr; /* For setting with unknown wm */
    const char      *name         = nullptr; /* For window manager name string */
    const char      *cmd          = nullptr; /* For wallp set command string */

    list_store = gtk_list_store_new (WM_COLUMN_COUNT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store),
                                          WM_COLUMN_NAME, GTK_SORT_ASCENDING);
    wm = wm_list;

    while (wm != nullptr) {
        name = setting_get_name (wm);
        if (strcmp (name, "Unknown") == 0) {
            unknown = wm;
            wm = wm->next;
            continue;
        }
        if ((cmd_setting = setting_find_child (wm, "Command")) != nullptr)
            cmd = setting_get_string (cmd_setting);

        gtk_list_store_append (list_store, &iter);
        gtk_list_store_set (list_store, &iter,
                            WM_COLUMN_NAME,    name,
                            WM_COLUMN_COMMAND, cmd,
                            -1);
        wm = wm->next;
    }

    gtk_tree_sortable_set_sort_column_id (
            GTK_TREE_SORTABLE (list_store),
            GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID,
            GTK_SORT_ASCENDING);

    name = setting_get_name (unknown);
    if ((cmd_setting = setting_find_child (unknown, "Command")) != nullptr)
        cmd = setting_get_string (cmd_setting);
    gtk_list_store_prepend (list_store, &iter);
    gtk_list_store_set (list_store, &iter,
                        WM_COLUMN_NAME,    name,
                        WM_COLUMN_COMMAND, cmd,
                        -1);

    combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

    g_object_unref (G_OBJECT (list_store));

    cell = gtk_cell_renderer_text_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), cell, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (combo),
                                   cell, "text", WM_COLUMN_NAME);
    return combo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates ComboBox with wallpaper list.
 *
 * @param[in]  images  List with wallpaper ImageInfo items.
 * @return     ComboBox widget
 */
static GtkWidget *
preview_combo (const GSList *images)
{
    GtkWidget       *combo;       /* Result ComboBox */
    GtkCellRenderer *cell;        /* CellRenderer */
    GtkListStore    *list_store;  /* ListStore for wallpaper data */
    ImageInfo       *ii_info;     /* ImageInfo for images data */
    GtkTreeIter      iter;        /* TreeIter */
    int              cnt = 0;     /* For wallpaper counting */
    char name [PREV_LEN * 4 + 4]; /* Buffer for file name length checking */

    list_store = gtk_list_store_new (PREV_COLUMN_COUNT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);
    while (images != nullptr && cnt++ < 10) {

        ii_info = (ImageInfo *) images->data;
        /* Check file name length and shrink to PREV_LEN if it is longer
         * so ComboBox will not be so wide with longer names */
        if (g_utf8_strlen (ii_info->s_file_name, -1) > PREV_LEN) {
            g_utf8_strncpy (name, ii_info->s_file_name, PREV_LEN);
            strcat (name, "...");
        }
        else {
            strcpy (name, ii_info->s_file_name);
        }
        gtk_list_store_append (list_store, &iter);
        gtk_list_store_set (list_store, &iter,
                            PREV_NAME_SHOW, name,
                            PREV_NAME_FULL, ii_info->s_file_path,
                            -1);
        images = images->next;
    }
    combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

    g_object_unref (G_OBJECT (list_store));

    cell = gtk_cell_renderer_text_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo), cell, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (combo),
                                   cell, "text", PREV_NAME_SHOW);

    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store), &iter))
        gtk_combo_box_set_active_iter (GTK_COMBO_BOX (combo), &iter);

    return combo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clicked button to get wm's default saved wallpaper set command.
 *
 * @param[in] widgets  Array with combo and entry widgets
 * @return    none
 */
static void
event_get_saved_button_clicked (GtkWidget **widgets)
{
    GtkTreeModel *model;         /* TreeModel */
    GtkTreeIter   iter;          /* TreeIter */
    char         *cmd = nullptr; /* Wallpaper set command */

    if (gtk_combo_box_get_active_iter (
                GTK_COMBO_BOX (widgets[GW_CMD_WM_COMBO]), &iter)) {

        model = gtk_combo_box_get_model (
                GTK_COMBO_BOX (widgets[GW_CMD_WM_COMBO]));
        gtk_tree_model_get (model, &iter, WM_COLUMN_COMMAND, &cmd, -1);

        if (cmd != nullptr) {
            textview_set_text (widgets[GW_CMD_TREEVIEW], cmd);
            free (cmd);
        }
        else {
            textview_set_text (widgets[GW_CMD_TREEVIEW], "");
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clicked button to get wm's application default wallpaper set command.
 *
 * @param[in] widgets  Array with combo and entry widgets
 * @return    none
 */
static void
event_get_default_button_clicked (GtkWidget **widgets)
{
    GtkTreeModel *model;               /* TreeModel */
    GtkTreeIter   iter;                /* TreeIter */
    char         *name      = nullptr; /* Window manager name */
    char         *cmd       = nullptr; /* Wallpaper set command */
    Setting      *settings  = nullptr; /* For window manager setting list */
    Setting      *item      = nullptr; /* For checking wm name and command */
    int           err       = 0;       /* Error output */

    if (gtk_combo_box_get_active_iter (
                GTK_COMBO_BOX (widgets[GW_CMD_WM_COMBO]), &iter)) {

        /* Load settings from deffiles template */
        settings = wms_get_wm_info_data (&err);
        if (err != ERR_OK) {
            message_dialog_error (GTK_WINDOW (widgets[GW_CMD_DIALOG]),
                                  err_get_message (err));
            return;
        }
        model = gtk_combo_box_get_model (
                GTK_COMBO_BOX (widgets[GW_CMD_WM_COMBO]));
        gtk_tree_model_get (model, &iter, WM_COLUMN_NAME, &name, -1);

        /* If wm is Xfce select display for command */
        if (strcmp (name, "Xfce") == 0) {
            cmd = xfce_dialog_run (GTK_WINDOW (widgets[GW_CMD_DIALOG]));
        }
        /* Find selected wm in default info data */
        else if ((item = setting_find_child (settings, name)) != nullptr) {
            if ((item = setting_find_child (item, "Command")) != nullptr) {
                cmd = strdup (setting_get_string (item));
            }
        }
        /* Set command if it is OK */
        if (cmd != nullptr) {
            textview_set_text (widgets[GW_CMD_TREEVIEW], cmd);
            free (cmd);
        }
        free (name);
        settings_free_all (settings);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clicked button to save wallpaper set command for selected wm.
 *
 * @param[in] widgets  Array with combo and entry widgets
 * @return    none
 */
static void
event_save_command_button_clicked (GtkWidget **widgets)
{
    GtkTreeModel *model;              /* TreeModel */
    GtkTreeIter   iter;               /* TreeIter */
    char         *name     = nullptr; /* Window manager name */
    char         *command  = nullptr; /* String for wallpaper command */
    int           err      = 0;       /* Error output */

    if (gtk_combo_box_get_active_iter (
                GTK_COMBO_BOX (widgets[GW_CMD_WM_COMBO]), &iter)) {

        model = gtk_combo_box_get_model (
                GTK_COMBO_BOX (widgets[GW_CMD_WM_COMBO]));
        gtk_tree_model_get (model, &iter, WM_COLUMN_NAME, &name, -1);

        command = textview_get_text (widgets[GW_CMD_TREEVIEW]);

        if ((err = wms_update_wm_command (name, command)) == ERR_OK) {
            gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                                WM_COLUMN_COMMAND, command,
                                -1);
        }
        else {
            /* err (EXIT_FAILURE, NULL); */
            message_dialog_error (GTK_WINDOW (widgets[GW_CMD_DIALOG]),
                                  err_get_message (err));
        }
        free (name);
        free (command);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clicked button to test wallpaper set command.
 *
 * @param[in] widgets  Array with combo and entry widgets
 * @return    none
 */
static void
event_test_button_clicked (GtkWidget **widgets)
{
    GtkTreeModel *model;              /* TreeModel */
    GtkTreeIter   iter;               /* TreeIter */
    char         *file     = nullptr; /* String for wallpaper file */
    char         *command  = nullptr; /* String for wallpaper command */

    if (gtk_combo_box_get_active_iter (
                GTK_COMBO_BOX (widgets[GW_CMD_PREV_COMBO]), &iter)) {

        model = gtk_combo_box_get_model (
                GTK_COMBO_BOX (widgets[GW_CMD_PREV_COMBO]));
        gtk_tree_model_get (model, &iter, PREV_NAME_FULL, &file, -1 );
    }
    command = textview_get_text (widgets[GW_CMD_TREEVIEW]);

    if (command != nullptr && file != nullptr) {
        wallpaper_test_set (command, file);
    }
    free (command);
    free (file);
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
    GtkWidget   *widget = gtk_text_view_new ();
    GtkTextView *view   = GTK_TEXT_VIEW (widget);

    /* Settings for command treeview */
    gtk_text_view_set_wrap_mode      (view, GTK_WRAP_WORD);
    gtk_text_view_set_editable       (view, TRUE);
    gtk_text_view_set_cursor_visible (view, TRUE);
    gtk_text_view_set_accepts_tab    (view, FALSE);
    gtk_text_view_set_monospace      (view, TRUE);
    gtk_text_view_set_border_window_size (view, GTK_TEXT_WINDOW_LEFT,   8);
    gtk_text_view_set_border_window_size (view, GTK_TEXT_WINDOW_RIGHT,  8);
    gtk_text_view_set_border_window_size (view, GTK_TEXT_WINDOW_TOP,    8);
    gtk_text_view_set_border_window_size (view, GTK_TEXT_WINDOW_BOTTOM, 8);

    return widget;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Run Xfce wallpaper set command configuration dialog.
 *
 * @param[in] parent  Window widget for setting dialog modal
 * @return    String with wallpaper set command or null. It should be
 *            freed after use using free.
 */
static char *
xfce_dialog_run (GtkWindow *parent)
{
    GtkWidget       *dialog;      /* Xfce command dialog */
    GtkWidget       *title_label; /* Title label */
    GtkWidget       *content_box; /* Dialog's box */
    GtkWidget       *radio_button = nullptr; /* Radiobutton */
    GtkToggleButton *button       = nullptr; /* ToggleButton */
    GSList          *group        = nullptr; /* List for buttons */
    char            *command      = nullptr; /* Result command string */
    int              res          = 0;       /* Dialog result */
    char           **displays     = nullptr; /* Null term. list of xfce displays */
    char           **it           = nullptr; /* For display list iteration */
    const char      *display      = nullptr; /* Selected display string */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons (
                                      "Xfce wallpaper set command configuration",
                                      parent,
                                      flags,
                                      "_OK",
                                      GTK_RESPONSE_ACCEPT,
                                      "_Cancel",
                                      GTK_RESPONSE_REJECT,
                                      nullptr);

    content_box = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    gtk_container_set_border_width (GTK_CONTAINER (content_box), 8);

    displays   = wms_get_xfce_display_list ();
    title_label = *displays == nullptr ?
                  gtk_label_new ("Could not find Xfce screens") :
                  gtk_label_new ("Select display to set wallpaper:");

    gtk_box_pack_start (GTK_BOX (content_box),
                        title_label,
                        FALSE, FALSE, 4);

    for (it = displays; *it != nullptr; ++it) {
        radio_button = gtk_radio_button_new_with_label_from_widget (
                GTK_RADIO_BUTTON (radio_button), *it);
        gtk_widget_set_name (radio_button, *it);

        gtk_box_pack_start (GTK_BOX (content_box), radio_button,
                        FALSE, FALSE, 4);
    }
    wms_free_xfce_display_list (displays);

    gtk_widget_show_all (content_box);

    res = gtk_dialog_run (GTK_DIALOG (dialog));

    if (res == GTK_RESPONSE_ACCEPT && radio_button != nullptr) {
        group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio_button));
        while (group != nullptr) {
            button = group->data;
            if (gtk_toggle_button_get_active (button)) {
                display = gtk_widget_get_name (GTK_WIDGET (button));
                break;
            }
            group = group->next;
        }
    }
    if (display != nullptr) {
        command = wms_get_xfce_command (display);
    }
    gtk_widget_destroy (dialog);

    return command;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Run wallpaper set command configuration dialog.
 */
char *
wp_set_command_dialog_run (GtkWindow    *parent,
                           const char   *current_cmd,
                           const GSList *images)
{
    GtkWidget *dialog;              /* Wallpaper set command dialog */
    GtkWidget *content_box;         /* Dialog's box */
    GtkWidget *wm_label;            /* Finding window manager label */
    GtkWidget *combo_label;         /* Label for window manager combo */
    GtkWidget *wm_combo_box;        /* ComboBox with window managers */
    GtkWidget *get_saved_btn;       /* Button for getting saved app wall cmd */
    GtkWidget *get_default_btn;     /* Button for getting app default wall cmd */
    GtkWidget *save_btn;            /* Button for saving default wall cmd */
    GtkWidget *wm_box;              /* Box for window manager combo and button */
    GtkWidget *textview;            /* Wallpaper set command textview */
    GtkWidget *wallpaper_combo;     /* ComboBox with wallpapers */
    GtkWidget *test_btn;            /* Button for testing wallpaper command */
    GtkWidget *test_box;            /* Box for test button and wallpaper list */
    GtkWidget *widgets[GW_CMD_CNT]; /* Array with widgets for clicked events */
    const Setting *current_wm = nullptr; /* Current window manager info */
    Setting   *wms     = nullptr;   /* Window manager info list */
    char      *result  = nullptr;   /* Result string with wall set command */
    int        res     = 0;         /* Config dialog result */
    int        err     = ERR_OK;    /* Error output */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    wms = wms_get_wm_info (&err);

    if (err != ERR_OK) {
        message_dialog_error (parent, err_get_message (err));
        return nullptr;
    }
    dialog = gtk_dialog_new_with_buttons (
                                      "Wallpaper set command configuration",
                                      parent,
                                      flags,
                                      "_OK",
                                      GTK_RESPONSE_ACCEPT,
                                      "_Cancel",
                                      GTK_RESPONSE_REJECT,
                                      nullptr);

    wm_label        = gtk_label_new (nullptr);
    textview        = create_command_textview ();
    wm_combo_box    = wm_combo (wms);
    get_saved_btn   = gtk_button_new_with_label ("Get saved");
    get_default_btn = gtk_button_new_with_label ("Get app default");
    save_btn        = gtk_button_new_with_label (
            "Save command for selected window manager");
    test_btn        = gtk_button_new_with_label ("Test");
    test_box        = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    wm_box          = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    content_box     = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    wallpaper_combo = preview_combo (images);
    combo_label     = gtk_label_new (
            "List of window managers with provided wallpaer set commands:");

    gtk_widget_set_tooltip_markup (get_saved_btn,
            "Get wallpaper set command for selected window manager "
            "(from user's config file).");
    gtk_widget_set_tooltip_markup (get_default_btn,
            "Get wallpaper set command for selected window manager "
            "(application default).");
    gtk_widget_set_tooltip_markup (save_btn,
            "Save wallpaper set command for selected window manager "
            "(in user's config file).");

    gtk_container_set_border_width (GTK_CONTAINER (content_box), 8);

    widgets[GW_CMD_WM_COMBO]   = wm_combo_box;
    widgets[GW_CMD_PREV_COMBO] = wallpaper_combo;
    widgets[GW_CMD_TREEVIEW]   = textview;
    widgets[GW_CMD_DIALOG]     = dialog;

    /* Packing window manager box */
    gtk_box_pack_start (GTK_BOX (wm_box), wm_combo_box,
                        TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (wm_box), get_saved_btn,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (wm_box), get_default_btn,
                        FALSE, FALSE, 0);

    /* Setting command from main window */
    if (current_cmd != nullptr)
        textview_set_text (textview, current_cmd);

    /* Trying to detect what window manager is in use and set wm label text and
     * combobox active index to found wm */
    if ((current_wm = wms_get_current_wm (wms)) != nullptr) {
        combo_set_active_by_wm_name (wm_combo_box, setting_get_name (current_wm));
        wm_label_set_text (wm_label, setting_get_name (current_wm));
    }
    settings_free_all (wms);

    /* Packing wallpaper command test box with test button and combobox with
     * sample wallpapers*/
    gtk_box_pack_start (GTK_BOX (test_box), test_btn,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (test_box), wallpaper_combo,
                        TRUE, TRUE, 0);

    /* Found window manager labels */
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new ("Looking for window manager..."),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box), wm_label,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    /* Window manager label and combobox */
    gtk_box_pack_start (GTK_BOX (content_box), combo_label,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box), wm_box,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    /* Wallpaper set command label, treeview, save command button */
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new ("Wallpaper set command:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box), textview,
                        TRUE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (content_box), save_btn,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    /* Testing label, and box with combo and button */
    gtk_box_pack_start (GTK_BOX (content_box),
                     gtk_label_new ("Test command using wallpaper from list:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box), test_box,
                        FALSE, FALSE, 4);

    g_signal_connect_swapped (get_saved_btn, "clicked",
                              G_CALLBACK (event_get_saved_button_clicked),
                              widgets);

    g_signal_connect_swapped (get_default_btn, "clicked",
                              G_CALLBACK (event_get_default_button_clicked),
                              widgets);

    g_signal_connect_swapped (save_btn, "clicked",
                              G_CALLBACK (event_save_command_button_clicked),
                              widgets);

    g_signal_connect_swapped (test_btn, "clicked",
                              G_CALLBACK (event_test_button_clicked),
                              widgets);

    gtk_widget_show_all (content_box);

    res = gtk_dialog_run (GTK_DIALOG (dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        result = textview_get_text (textview);
    }
    gtk_widget_destroy (dialog);

    return result;
}
/*----------------------------------------------------------------------------*/
