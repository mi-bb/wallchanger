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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include "defs.h"
#include "imgs.h"
#include "iminfo.h"
#include "wmsfn.h"
#include "wpset.h"
#include "strfun.h"
#include "dlgs.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   PREV_LEN
 * @brief Length of file name preview string for combobox.
 */
#define PREV_LEN 47
/*----------------------------------------------------------------------------*/
enum {
    COLUMN_ID,      /**< Combobox ListStore column for id */
    COLUMN_WM_ID,   /**< Combobox ListStore column for window manager id */
    COLUMN_NAME,    /**< Combobox ListStore column for window manager name */
    COLUMN_COMMAND, /**< Combobox ListStore column for wallpaper set command */
    COLUMN_COUNT    /**< Column count */
};
/*----------------------------------------------------------------------------*/
enum {
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
 * @brief  Replace newline 0A characters in s_str string to space.
 *
 * @param[in,out] s_str  String to process
 * @return        none
 */
static void
newline_to_space (char *s_str)
{
    while ((s_str = strchr (s_str, '\n')) != NULL) {
        *s_str++ = ' ';
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set active combobox position
 *
 * @param[in,out] gw_combo ComboBox item
 * @param[in]     i_act_id Id of window manager to set active
 * @return        none
 */
static void
combo_set_active_by_wm_id (GtkWidget *gw_combo,
                           const int  i_act_id)
{
    GtkTreeModel *gtm_model;
    GtkTreeIter   iter;
    int           i_wm_id = -1; /* Window manager id */
    int           i_res   = 0;  /* Getting iter result */

    gtm_model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
    i_res = gtk_tree_model_get_iter_first (gtm_model, &iter);

    while (i_res) {
        gtk_tree_model_get (gtm_model, &iter, COLUMN_WM_ID, &i_wm_id, -1);
        if (i_act_id == i_wm_id) {
            gtk_combo_box_set_active_iter (GTK_COMBO_BOX (gw_combo), &iter);
            break;
        }
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
    if (s_txt == NULL) {
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
 * @brief  Set text in TextView
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
 * @brief  Get text from TextView
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
 * @param[in]  wm_list List with window manager info.
 * @return     ComboBox widget
 */
static GtkWidget *
wm_combo (Wms **wms_list)
{
    GtkWidget       *gw_combo;      /* Result ComboBox */
    GtkCellRenderer *cell;          /* CellRenderer */
    GtkListStore    *list_store;    /* ListStore for window manager data */
    GtkTreeIter      iter;          /* TreeIter */
    int              i_prev = -1;   /* Preview wm id value */
    Wms            **wms_it = NULL; /* For iteration */

    list_store = gtk_list_store_new (COLUMN_COUNT,
                                     G_TYPE_INT,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store),
                                          COLUMN_NAME, GTK_SORT_ASCENDING);

    for (wms_it = wms_list; *wms_it != NULL; ++wms_it) {
        if (i_prev != (*wms_it)->wm_id) {
            gtk_list_store_append (list_store, &iter);
            gtk_list_store_set (list_store, &iter,
                                COLUMN_ID,      (*wms_it)->id,
                                COLUMN_WM_ID,   (*wms_it)->wm_id,
                                COLUMN_NAME,    (*wms_it)->name,
                                COLUMN_COMMAND, (*wms_it)->command,
                                -1);
            i_prev = (*wms_it)->wm_id;
        }
    }
    gw_combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

    g_object_unref (G_OBJECT (list_store));

    cell = gtk_cell_renderer_text_new ();

    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (gw_combo), cell, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (gw_combo),
                                   cell, "text", COLUMN_NAME);
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
                                PREV_NAME_FULL, ii_info->s_full_path,
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
 * @brief  Clicked button to get wm's default wallpaper set command.
 *
 * @param[in] gw_array  Array with combo and entry widgets
 * @return    none
 */
static void
event_get_default_button_clicked (GtkWidget **gw_array)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    GtkWidget    *gw_combo;         /* ComboBox with window managers */
    GtkWidget    *gw_tview;         /* Textview with wallpaper set command */
    GtkWindow    *gw_parent;        /* Parent window for dialog */
    char         *s_cmd     = NULL; /* Wallpaper set command */
    int           i_wm_id   = 0;    /* Wm's id */

    gw_combo  = gw_array[0];
    gw_tview  = gw_array[2];
    gw_parent = GTK_WINDOW (gw_array[3]);

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter, COLUMN_COMMAND, &s_cmd, -1);
        gtk_tree_model_get (model, &iter, COLUMN_WM_ID, &i_wm_id, -1);

        if (i_wm_id == WM_ID_XFCE) {
            s_cmd = xfce_dialog_run (gw_parent);
        }

        if (s_cmd != NULL) {
            textview_set_text (gw_tview, s_cmd);
            free (s_cmd);
        }
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
    GtkWidget   *gw_radiobutton  = NULL; /* Radiobutton */
    GtkToggleButton *gtb_button  = NULL; /* ToggleButton */
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

    s_displays = get_xfce_display_list ();
    if (*s_displays == NULL) {
        gw_tlabel = gtk_label_new ("Could not find Xfce screens");
    }
    else {
        gw_tlabel = gtk_label_new ("Select display to set wallpaper:");
    }
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_tlabel,
                        FALSE, FALSE, 4);

    for (s_it = s_displays; *s_it != NULL; ++s_it) {
        gw_radiobutton = gtk_radio_button_new_with_label_from_widget (
                GTK_RADIO_BUTTON (gw_radiobutton), *s_it);
        gtk_widget_set_name (gw_radiobutton, *s_it);

        gtk_box_pack_start (GTK_BOX (gw_content_box), gw_radiobutton,
                        FALSE, FALSE, 4);
    }

    for (s_it = s_displays; *s_it != NULL; ++s_it) {
        free (*s_it);
    }
    free (s_displays);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT && gw_radiobutton != NULL) {
        gs_list = gtk_radio_button_get_group (
                GTK_RADIO_BUTTON (gw_radiobutton));
        while (gs_list != NULL) {
            gtb_button = gs_list->data;
            if (gtk_toggle_button_get_active (gtb_button)) {
                s_disp = gtk_widget_get_name (GTK_WIDGET (gtb_button));
                break;
            }
            gs_list = gs_list->next;
        }
    }
    if (s_disp != NULL) {
        s_res = str_comb ("xfconf-query --channel xfce4-desktop --property ",
                          s_disp);
        str_append (&s_res, " --set [F]");
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
    GtkWidget *gw_get_def_button; /* Button for getting default wallpaper cmd */
    GtkWidget *gw_wm_box;         /* Box for window manager combo and button */
    GtkWidget *gw_tview;          /* Wallpaper set command textview */
    GtkWidget *gw_preview_combo;  /* ComboBox with wallpapers */
    GtkWidget *gw_test_button;    /* Button for testing wallpaper command */
    GtkWidget *gw_test_box;       /* Box for test button and wallpaper list */
    GtkWidget *gw_array[4];       /* Array with widgets for clicked events */
    Wms       *wms_wm;            /* Window manager structure for searching */
    char      *s_result = NULL;   /* Result string with wall set command */
    int        i_res    = 0;      /* Config dialog result */

    Wms **wms_list = NULL;
    wms_list = wms_list_get ();
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    gw_dialog = gtk_dialog_new_with_buttons (
                                      "Wallpaper set command configuraion",
                                      gw_parent,
                                      flags,
                                      "_OK",
                                      GTK_RESPONSE_ACCEPT,
                                      "_Cancel",
                                      GTK_RESPONSE_REJECT,
                                      NULL);

    gw_wm_label       = gtk_label_new (NULL);
    gw_tview          = create_command_textview ();
    gw_wm_combo       = wm_combo (wms_list);
    gw_get_def_button = gtk_button_new_with_label ("Get default");
    gw_test_button    = gtk_button_new_with_label ("Test");
    gw_test_box       = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gw_wm_box         = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gw_content_box    = gtk_dialog_get_content_area (GTK_DIALOG (gw_dialog));
    gw_preview_combo  = preview_combo (gsl_iinfo);
    gw_combo_label    = gtk_label_new (
            "List of window managers with provided wallpaer set commands:");
    wms_list_free (wms_list);

    gtk_widget_set_tooltip_markup (gw_get_def_button,
            "Get default command for selected window manager.");

    gtk_container_set_border_width (GTK_CONTAINER (gw_content_box), 8);

    gw_array[0] = gw_wm_combo;
    gw_array[1] = gw_preview_combo;
    gw_array[2] = gw_tview;
    gw_array[3] = gw_dialog;

    /* Packing window manager box */
    gtk_box_pack_start (GTK_BOX (gw_wm_box),
                        gw_wm_combo,
                        TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (gw_wm_box),
                        gw_get_def_button,
                        FALSE, FALSE, 0);

    /* Setting command from main window */
    if (s_current_cmd != NULL)
        textview_set_text (gw_tview, s_current_cmd);

    /* Trying to detect what window manager is in use and set wm label text and
     * combobox active index to found wm */
    if ((wms_wm = find_window_magager ()) != NULL) {
        combo_set_active_by_wm_id (gw_wm_combo, wms_wm->wm_id);
        wm_label_set_text (gw_wm_label, wms_wm->name);
        free (wms_wm);
    }
    else {
        wm_label_set_text (gw_wm_label, NULL);
    }

    /* Packing wallpaper command test box with test button and combobox with
     * sample wallpapers*/
    gtk_box_pack_start (GTK_BOX (gw_test_box),
                        gw_test_button,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (gw_test_box),
                        gw_preview_combo,
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

    /* Wallpaper set command label and treeview */
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new ("Wallpaper set command:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_tview,
                        TRUE, TRUE, 4);

    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    /* Testing label, and box with combo and button */
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                     gtk_label_new ("Test command using wallpaper from list:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_test_box,
                        FALSE, FALSE, 4);

    g_signal_connect_swapped (gw_get_def_button, "clicked",
                              G_CALLBACK (event_get_default_button_clicked),
                              gw_array);

    g_signal_connect_swapped (gw_test_button, "clicked",
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
/**
 * @brief  Select folder dialog.
 */
char *
add_images_folder_dialog (GtkWindow *gw_parent)
{
    GtkWidget *gw_dialog;        /* Directory choose dialog */
    char      *s_folder  = NULL; /* Selected folder name */

    gw_dialog = gtk_file_chooser_dialog_new ("Select Folder",
                                          gw_parent,
                                          GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                          "_Cancel",
                                          GTK_RESPONSE_CANCEL,
                                          "_Open",
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    if (gtk_dialog_run (GTK_DIALOG (gw_dialog)) == GTK_RESPONSE_ACCEPT) {
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
/**
 * @brief  Dialog with information about application.
 */
void
about_app_dialog (gpointer data __attribute__ ((unused)))
{
    GtkWidget *gw_dialog;
    const char *s_authors[] = {"Michal Babik <michalb1981@o2.pl> "
                               "http://init6.pomorze.pl", NULL};
    const char *s_art[]     = {
        "https://fr.wikisource.org/wiki/Le_Tour_de_la_France_par_deux_enfants",
        NULL};

    gw_dialog = gtk_about_dialog_new ();

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

