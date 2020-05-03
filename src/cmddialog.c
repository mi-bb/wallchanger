/**
 * @file  cmddialog.c
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
 * @brief  Wallpaper set command window for detailed command setting.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <err.h>
#include "procfn.h"
#include "iminfo.h"
#include "wpset.h"
#include "cmddialog.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   PREV_LEN
 * @brief Length of file name preview string for combobox.
 */
#define PREV_LEN 47
/*----------------------------------------------------------------------------*/
enum {
    COLUMN_ID,      /**< Combobox ListStore column for id */
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
enum {
    WM_ID_I3,       /**< Id for i3 window manager */
    WM_ID_MATE,     /**< Id foe MATE window manager */
    WM_ID_GNOME,    /**< Id for Gnome window manager */
    WM_ID_CINNAMON, /**< Id for Cinnamon window manager */
    WM_ID_PLASMA,   /**< Id for KDE Plasma window manager */
    WM_ID_OPENBOX,  /**< Id for Openbox window manager */
    WM_ID_FLUXBOX,  /**< Id for Fluxbox window manager */
    WM_ID_END       /**< End of Ids */
};
/*----------------------------------------------------------------------------*/
/**
 * @struct Wms
 *
 * @brief Structure with window manager info.
 *
 * @var   Wms::wm_id
 * @brief Window manager id
 *
 * @var   Wms::process
 * @brief Window manager process name used to find it
 *
 * @var   Wms::name
 * @brief Name of window manager
 *
 * @var   Wms::command
 * @brief Command to set wallpaper for window manager
 */
typedef struct Wms {
    int  wm_id;
    char process [32];
    char name    [16];
    char command [512];
} Wms;
/*----------------------------------------------------------------------------*/
/**
 * @var   wms
 * @brief List with window managers.
 */
static Wms wms[] = {
    {WM_ID_MATE, "mate-session", "MATE",
     "gsettings set org.mate.background picture-filename [F]"},
    {WM_ID_GNOME, "gnome-session-b", "Gnome",
     "gsettings set org.gnome.desktop.background picture-uri file://[F]"},
    {WM_ID_GNOME, "gnome-session-binary", "Gnome",
     "gsettings set org.gnome.desktop.background picture-uri file://[F]"},
    {WM_ID_CINNAMON, "cinnamon-sessio", "Cinnamon",
     "gsettings set org.cinnamon.desktop.background picture-uri file://[F]"},
    {WM_ID_CINNAMON, "cinnamon", "Cinnamon",
     "gsettings set org.cinnamon.desktop.background picture-uri file://[F]"},
    {WM_ID_PLASMA, "plasma-session", "KDE Plasma",
     "qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScri"
     "pt 'var allDesktops = desktops();print (allDesktops);for (i=0;i<allDesk"
     "tops.length;i++) {d = allDesktops[i];d.wallpaperPlugin = \"org.kde.imag"
     "e\";d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"Ge"
     "neral\");d.writeConfig(\"Image\", \"file://[F]\")}'"},
    {WM_ID_I3, "i3", "i3",
     "feh --bg-fill [F]"},
    {WM_ID_OPENBOX, "openbox", "Openbox",
     "feh --bg-fill [F]"},
    {WM_ID_FLUXBOX, "fluxbox", "Fluxbox",
     "feh --bg-fill [F]"},
    {WM_ID_END, "", "", ""}
    };

/*----------------------------------------------------------------------------*/
/**
 * @brief  Find window manager that is currently in use.
 *
 * @param[in] wm_list  List with window manager info
 * return     Wms item with info about window manager or null if no wm was
 *            found. It should be freed after use using free.
 */
static Wms *
find_window_magager (const Wms *wm_list)
{
    Wms *wms_ret = NULL; /* Wms to return */

    while (wm_list->wm_id != WM_ID_END) {

        if (process_exists_b (wm_list->process)) {

            if ((wms_ret = malloc (sizeof (Wms))) == NULL)
                err (EXIT_FAILURE, NULL);

            wms_ret->wm_id = wm_list->wm_id;
            strcpy (wms_ret->name, wm_list->name);
            strcpy (wms_ret->process, wm_list->process);
            strcpy (wms_ret->command, wm_list->command);
            return wms_ret;
        }
        wm_list++;
    }
    return wms_ret;
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
    GtkTreeModel *gtm_model;  /* TreeModel */
    GtkTreeIter   iter;       /* TreeIter */
    int           i_id  = -1; /* Window manager id */
    int           i_res = 0;  /* Getting iter result */

    gtm_model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
    i_res = gtk_tree_model_get_iter_first (gtm_model, &iter);

    while (i_res) {
        gtk_tree_model_get (gtm_model, &iter, COLUMN_ID, &i_id, -1);
        if (i_act_id == i_id) {
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
 * @brief  Creates ComboBox with window managers.
 *
 * @param[in]  wm_list List with window manager info.
 * @return     ComboBox widget
 */
static GtkWidget *
cmddialog_combo (const Wms *wm_list)
{
    GtkWidget       *gw_combo;      /* Result ComboBox */
    GtkCellRenderer *cell;          /* CellRenderer */
    GtkListStore    *list_store;    /* ListStore for window manager data */
    GtkTreeIter      iter;          /* TreeIter */
    int              i_prev = -1;   /* Preview wm id value */

    list_store = gtk_list_store_new (COLUMN_COUNT,
                                     G_TYPE_INT,
                                     G_TYPE_STRING,
                                     G_TYPE_STRING);

    gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store),
                                          COLUMN_NAME, GTK_SORT_ASCENDING);

    while (wm_list->wm_id != WM_ID_END) {
        if (i_prev != wm_list->wm_id) {
            gtk_list_store_append (list_store, &iter);
            gtk_list_store_set (list_store, &iter,
                                COLUMN_ID,      wm_list->wm_id,
                                COLUMN_NAME,    wm_list->name,
                                COLUMN_COMMAND, wm_list->command,
                                -1);

            i_prev = wm_list->wm_id;
        }
        ++wm_list;
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
 * @brief  Combobox with window managers changed.
 *
 * @param[in] gw_combo  ComboBox widget
 * @param[in] gw_entry  Entry widget
 * @return    none
 */
static void
event_combo_changed (GtkWidget *gw_combo,
                     GtkWidget *gw_entry)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;
    char         *s_cmd  = NULL;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {

        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter, COLUMN_COMMAND, &s_cmd, -1 );
        if (s_cmd != NULL) {
            gtk_entry_set_text (GTK_ENTRY (gw_entry), s_cmd);
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
    GtkTreeModel *model;                    /* TreeModel */
    GtkTreeIter   iter;                     /* TreeIter */
    GtkWidget    *gw_combo  = gw_array[0];  /* ComboBox with wallpapers */
    GtkWidget    *gw_entry  = gw_array[1];  /* Entry with command */
    char         *s_file    = NULL;         /* String for wallpaper file */
    const char   *s_command = NULL;         /* String for wallpaper command */

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (gw_combo), &iter)) {
        model = gtk_combo_box_get_model (GTK_COMBO_BOX (gw_combo));
        gtk_tree_model_get (model, &iter, PREV_NAME_FULL, &s_file, -1 );
    }
    s_command = gtk_entry_get_text (GTK_ENTRY (gw_entry));

    if (s_command != NULL && s_file != NULL) {
        wallpaper_test_set (s_command, s_file);
    }
    free (s_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Run wallpaper set command configuration dialog.
 */
char *
cmddialog_run (GtkWindow    *gw_parent,
               const GSList *gsl_iinfo)
{
    GtkWidget *gw_dialog;        /* Wallpaper set command dialog */
    GtkWidget *gw_content_box;   /* Dialog's box */
    GtkWidget *gw_entry;         /* Wallpaper set command entry */
    GtkWidget *gw_combo;         /* ComboBox with window managers */
    GtkWidget *gw_preview_combo; /* ComboBox with wallpapers */
    GtkWidget *gw_wm_label;      /* Finding window manager label */
    GtkWidget *gw_combo_label;   /* Label for window manager combo */
    GtkWidget *gw_test_button;   /* Button for testing wallpaper command */
    GtkWidget *gw_test_box;      /* Box for test button and wallpaper list */
    GtkWidget *gw_array[2];      /* Array with widgets for test clicked event */
    Wms       *wms_wm;           /* Window manager structure for searching */
    char      *s_result = NULL;  /* Result string with wall set command */
    int        i_res    = 0;     /* Config dialog result */

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

    gw_wm_label      = gtk_label_new (NULL);
    gw_entry         = gtk_entry_new ();
    gw_combo         = cmddialog_combo (wms);
    gw_test_button   = gtk_button_new_with_label ("Test");
    gw_test_box      = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    gw_content_box   = gtk_dialog_get_content_area (GTK_DIALOG (gw_dialog));
    gw_preview_combo = preview_combo (gsl_iinfo);
    gw_combo_label   = gtk_label_new (
            "List of window managers with provided wallpaer set commands:");

    gtk_container_set_border_width (GTK_CONTAINER (gw_content_box), 8);

    gw_array[0] = gw_preview_combo;
    gw_array[1] = gw_entry;

    if ((wms_wm = find_window_magager (wms)) != NULL) {
        combo_set_active_by_wm_id (gw_combo, wms_wm->wm_id);
        wm_label_set_text (gw_wm_label, wms_wm->name);
        gtk_entry_set_text (GTK_ENTRY (gw_entry), wms_wm->command);
        free (wms_wm);
    }
    gtk_box_pack_start (GTK_BOX (gw_test_box),
                        gw_test_button,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (gw_test_box),
                        gw_preview_combo,
                        TRUE, TRUE, 0);

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
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_combo_label,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_combo,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new ("Wallpaper set command:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_entry,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                     gtk_label_new ("Test command using wallpaper from list:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box), gw_test_box,
                        FALSE, FALSE, 4);

    g_signal_connect (gw_combo, "changed", G_CALLBACK (event_combo_changed),
                      gw_entry);
    g_signal_connect_swapped (gw_test_button, "clicked",
                              G_CALLBACK (event_test_button_clicked),
                              gw_array);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT) {
        s_result = strdup (gtk_entry_get_text (GTK_ENTRY (gw_entry)));
    }
    gtk_widget_destroy (gw_dialog);
    return s_result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find command for current window manager.
 */
char *
cmddialog_find_command (void)
{
    char *s_ret  = NULL; /* Command to return */
    Wms  *wms_wm = NULL; /* Window manager info */

    if ((wms_wm = find_window_magager (wms)) != NULL) {
        s_ret = strdup (wms_wm->command);
        free (wms_wm);
    }
    return s_ret;
}
/*----------------------------------------------------------------------------*/

