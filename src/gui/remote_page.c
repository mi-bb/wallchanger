/**
 * @file  remote_page.c
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
 * @brief  Main window page with remote live wallpaper services.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "live/live.h"
#include "settings/settings.h"
#include "util/errors.h"
#include "util/randomm.h"
#include "wallpaper/wallpaper_set.h"
#include "message_dialogs.h"
#include "preview.h"
#include "remote_page.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   PAGE_DATA_KEY
 * @brief Key of the RemotePage data attached to the page widget
 */
#define PAGE_DATA_KEY "wchanger-remote-page"
/*----------------------------------------------------------------------------*/
/**
 * @struct RemotePage
 *
 * @brief  Widgets and data of the remote page.
 *
 * @var   RemotePage::data
 * @brief Main window data
 *
 * @var   RemotePage::checks
 * @brief Check button for every service
 *
 * @var   RemotePage::entries
 * @brief Keywords entry for every service, null for one without keywords
 *
 * @var   RemotePage::preview
 * @brief Wallpaper preview image
 *
 * @var   RemotePage::info
 * @brief Label with wallpaper title, author and license
 *
 * @var   RemotePage::set_button
 * @brief "Set wallpaper now" button
 *
 * @var   RemotePage::spinner
 * @brief Spinner shown while a wallpaper is being downloaded
 *
 * @var   RemotePage::rand_mem
 * @brief Random memory for random service choice
 *
 * @var   RemotePage::screen_width
 * @brief Screen width in pixels
 *
 * @var   RemotePage::screen_height
 * @brief Screen height in pixels
 */
typedef struct RemotePage {
    DialogData  *data;
    GtkWidget  **checks;
    GtkWidget  **entries;
    GtkWidget   *preview;
    GtkWidget   *info;
    GtkWidget   *set_button;
    GtkWidget   *spinner;
    RandMem     *rand_mem;
    int          screen_width;
    int          screen_height;
} RemotePage;
/*----------------------------------------------------------------------------*/
/**
 * @struct SetNowTask
 *
 * @brief  Data of the background "Set wallpaper now" download.
 *
 * @var   SetNowTask::settings
 * @brief Settings the wallpaper is chosen with
 *
 * @var   SetNowTask::rand_mem
 * @brief Random memory of the page
 *
 * @var   SetNowTask::file
 * @brief Downloaded wallpaper path
 *
 * @var   SetNowTask::err
 * @brief Error code
 */
typedef struct SetNowTask {
    Setting *settings;
    RandMem *rand_mem;
    char    *file;
    int      err;
} SetNowTask;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the RemotePage data of a page widget.
 */
static RemotePage *
remote_page_data (GtkWidget *page)
{
    return g_object_get_data (G_OBJECT (page), PAGE_DATA_KEY);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free RemotePage data, called when the page is destroyed.
 */
static void
remote_page_free (gpointer user_data)
{
    RemotePage *page = user_data;

    randomm_free (page->rand_mem);
    free (page->checks);
    free (page->entries);
    free (page);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find the size of the screen in pixels.
 *
 * The whole monitor is used, not the work area without panels, as a
 * wallpaper covers it all. HiDPI monitors report their size in scaled units,
 * so it is multiplied by the scale factor.
 *
 * @param[out] width   Screen width, 0 when there is no monitor
 * @param[out] height  Screen height, 0 when there is no monitor
 * @return     none
 */
static void
detect_screen_size (int *width,
                    int *height)
{
    GdkRectangle geometry = {0};
    GdkDisplay  *display  = gdk_display_get_default ();
    GdkMonitor  *monitor  = nullptr;
    int          scale    = 1;

    if (display != nullptr) {
        monitor = gdk_display_get_primary_monitor (display);
        if (monitor == nullptr)
            monitor = gdk_display_get_monitor (display, 0);
    }
    if (monitor != nullptr) {
        gdk_monitor_get_geometry (monitor, &geometry);
        scale = gdk_monitor_get_scale_factor (monitor);
    }
    *width  = geometry.width  * scale;
    *height = geometry.height * scale;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the page settings: enabled services and keywords.
 */
Setting *
remote_page_get_settings (GtkWidget *page)
{
    RemotePage       *remote   = remote_page_data (page);
    const LiveSource *source   = nullptr;
    Setting          *settings = nullptr;
    Setting          *services = nullptr;

    services = setting_new_array (get_setting_name (SETT_LIVE_SERVICES));

    for (size_t i = 0; i < live_source_count (); ++i) {
        source = live_source_get (i);

        if (gtk_toggle_button_get_active (
                    GTK_TOGGLE_BUTTON (remote->checks[i]))) {
            setting_add_child (services,
                               setting_new_string (nullptr, source->name));
        }
        if (remote->entries[i] != nullptr) {
            settings = settings_append (settings,
                    setting_new_string (source->keywords_setting,
                        gtk_entry_get_text (GTK_ENTRY (remote->entries[i]))));
        }
    }
    return settings_append (settings, services);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the page widgets from settings.
 */
void
remote_page_set_settings (GtkWidget *page,
                          Setting   *settings)
{
    RemotePage       *remote = remote_page_data (page);
    const LiveSource *source = nullptr;
    Setting          *names  = nullptr;
    Setting          *item   = nullptr;
    const char       *name   = nullptr;
    bool              found  = false;

    names = setting_get_child (
            settings_find (settings, get_setting_name (SETT_LIVE_SERVICES)));

    for (size_t i = 0; i < live_source_count (); ++i) {
        source = live_source_get (i);
        found  = false;

        for (item = names; item != nullptr; item = item->next) {
            name = setting_get_string (item);
            if (name != nullptr && strcmp (name, source->name) == 0) {
                found = true;
                break;
            }
        }
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (remote->checks[i]),
                                      found);

        if (remote->entries[i] != nullptr) {
            name = setting_get_string (
                    settings_find (settings, source->keywords_setting));
            gtk_entry_set_text (GTK_ENTRY (remote->entries[i]),
                                name != nullptr ? name : "");
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if any service is enabled on the page.
 */
bool
remote_page_has_service (GtkWidget *page)
{
    RemotePage *remote = remote_page_data (page);

    for (size_t i = 0; i < live_source_count (); ++i) {
        if (gtk_toggle_button_get_active (
                    GTK_TOGGLE_BUTTON (remote->checks[i]))) {
            return true;
        }
    }
    return false;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the screen size in pixels found when the page was created.
 */
void
remote_page_get_screen_size (GtkWidget *page,
                             int       *width,
                             int       *height)
{
    RemotePage *remote = remote_page_data (page);

    *width  = remote->screen_width;
    *height = remote->screen_height;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Show a wallpaper preview with its title, author and license.
 */
void
remote_page_show_wallpaper (GtkWidget  *page,
                            const char *file)
{
    RemotePage *remote = remote_page_data (page);
    LiveImage   image  = {};
    GString    *text   = nullptr; /* Info label markup */
    char       *part   = nullptr; /* Escaped markup part */

    preview_from_file (remote->preview, file);

    if (!live_info_read (file, &image)) {
        gtk_label_set_text (GTK_LABEL (remote->info), "");
        return;
    }
    text = g_string_new (nullptr);

    if (image.title != nullptr) {
        part = g_markup_printf_escaped ("<b>%s</b>\n", image.title);
        g_string_append (text, part);
        g_free (part);
    }
    if (image.author != nullptr) {
        part = g_markup_printf_escaped ("Author: %s\n", image.author);
        g_string_append (text, part);
        g_free (part);
    }
    if (image.license != nullptr) {
        part = image.license_url != nullptr ?
            g_markup_printf_escaped ("License: <a href=\"%s\">%s</a>\n",
                                     image.license_url, image.license) :
            g_markup_printf_escaped ("License: %s\n", image.license);
        g_string_append (text, part);
        g_free (part);
    }
    if (image.page_url != nullptr) {
        part = g_markup_printf_escaped ("<a href=\"%s\">Source page</a>",
                                        image.page_url);
        g_string_append (text, part);
        g_free (part);
    }
    gtk_label_set_markup (GTK_LABEL (remote->info), text->str);
    g_string_free (text, TRUE);
    live_image_clear (&image);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free the data of a "Set wallpaper now" download.
 */
static void
set_now_task_free (gpointer user_data)
{
    SetNowTask *task = user_data;

    settings_free_all (task->settings);
    free (task->file);
    free (task);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Download a wallpaper in a background thread.
 */
static void
set_now_thread (GTask        *task,
                [[maybe_unused]] gpointer      source,
                gpointer      task_data,
                [[maybe_unused]] GCancellable *cancellable)
{
    SetNowTask *data = task_data;

    data->err = live_wallpaper_get (data->settings, data->rand_mem,
                                    &data->file);
    g_task_return_boolean (task, TRUE);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set the downloaded wallpaper when the background thread ends.
 */
static void
set_now_done (GObject      *source,
              GAsyncResult *result,
              [[maybe_unused]] gpointer user_data)
{
    GtkWidget  *page   = GTK_WIDGET (source);
    RemotePage *remote = remote_page_data (page);
    SetNowTask *data   = g_task_get_task_data (G_TASK (result));
    int         err    = data->err;

    gtk_spinner_stop (GTK_SPINNER (remote->spinner));
    gtk_widget_set_sensitive (remote->set_button, TRUE);

    if (err == ERR_OK) {
        err = wallpaper_dialog_set (
                gtk_entry_get_text (GTK_ENTRY (remote->data->command)),
                data->file,
                dialog_data_get_cfg_file (remote->data));
        remote_page_show_wallpaper (page, data->file);
    }
    if (err != ERR_OK)
        message_dialog_error (remote->data->window, err_get_message (err));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  "Set wallpaper now" button pressed.
 *
 * The wallpaper is downloaded in a background thread so the window does not
 * freeze.
 */
static void
event_set_now_pressed (GtkWidget *page)
{
    RemotePage *remote = remote_page_data (page);
    SetNowTask *data   = nullptr;
    GTask      *task   = nullptr;
    int64_t     random = 0;

    if (!remote_page_has_service (page)) {
        message_dialog_error (remote->data->window,
                              err_get_message (ERR_LIVE_NOSRC));
        return;
    }
    if ((data = calloc (1, sizeof (SetNowTask))) == nullptr)
        return;

    random = gtk_toggle_button_get_active (
            GTK_TOGGLE_BUTTON (remote->data->random));

    data->rand_mem = remote->rand_mem;
    data->settings = remote_page_get_settings (page);
    data->settings = settings_append (data->settings,
            setting_new_int (get_setting_name (SETT_RANDOM_OPT), random));
    data->settings = settings_append (data->settings,
            setting_new_int (get_setting_name (SETT_SCREEN_WIDTH),
                             remote->screen_width));
    data->settings = settings_append (data->settings,
            setting_new_int (get_setting_name (SETT_SCREEN_HEIGHT),
                             remote->screen_height));

    gtk_widget_set_sensitive (remote->set_button, FALSE);
    gtk_spinner_start (GTK_SPINNER (remote->spinner));

    task = g_task_new (page, nullptr, set_now_done, nullptr);
    g_task_set_task_data (task, data, set_now_task_free);
    g_task_run_in_thread (task, set_now_thread);
    g_object_unref (task);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create the grid with a check button for every service.
 *
 * @param[in,out] remote  Page data
 * @return        Grid widget
 */
static GtkWidget *
create_services_widget (RemotePage *remote)
{
    const LiveSource *source = nullptr;
    GtkWidget        *grid   = gtk_grid_new ();
    GtkWidget        *label  = nullptr;
    int               row    = 0;

    gtk_grid_set_row_spacing    (GTK_GRID (grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 8);

    label = gtk_label_new (nullptr);
    gtk_label_set_markup (GTK_LABEL (label),
                          "<b>Wallpaper services</b>");
    gtk_label_set_xalign (GTK_LABEL (label), 0);
    gtk_grid_attach (GTK_GRID (grid), label, 0, row++, 3, 1);

    for (size_t i = 0; i < live_source_count (); ++i, ++row) {
        source = live_source_get (i);

        remote->checks[i] = gtk_check_button_new_with_label (source->label);
        gtk_widget_set_tooltip_markup (remote->checks[i], source->info);
        gtk_grid_attach (GTK_GRID (grid), remote->checks[i], 0, row, 1, 1);

        if (source->keywords_setting != nullptr) {
            label = gtk_label_new ("Keywords:");
            remote->entries[i] = gtk_entry_new ();
            gtk_entry_set_placeholder_text (GTK_ENTRY (remote->entries[i]),
                                            "any");
            gtk_widget_set_tooltip_text (remote->entries[i],
                    "Words describing wanted wallpapers, leave empty for "
                    "any wallpaper");
            gtk_widget_set_hexpand (remote->entries[i], TRUE);
            gtk_grid_attach (GTK_GRID (grid), label, 1, row, 1, 1);
            gtk_grid_attach (GTK_GRID (grid), remote->entries[i],
                             2, row, 1, 1);
        }
    }
    return grid;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create the remote live wallpaper services page.
 */
GtkWidget *
remote_page_new (DialogData *data)
{
    RemotePage *remote    = nullptr;
    GtkWidget  *page      = nullptr; /* Page to return */
    GtkWidget  *box_left  = nullptr; /* Services and buttons */
    GtkWidget  *box_right = nullptr; /* Preview and image info */
    GtkWidget  *box_btn   = nullptr; /* Button and spinner */
    GtkWidget  *label     = nullptr;
    char       *text      = nullptr;
    size_t      count     = live_source_count ();

    if ((remote = calloc (1, sizeof (RemotePage))) == nullptr ||
        (remote->checks  = calloc (count, sizeof (GtkWidget *))) == nullptr ||
        (remote->entries = calloc (count, sizeof (GtkWidget *))) == nullptr) {
        g_error ("Could not allocate memory");
    }
    remote->data     = data;
    remote->rand_mem = randomm_new ();
    detect_screen_size (&remote->screen_width, &remote->screen_height);

    page = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    g_object_set_data_full (G_OBJECT (page), PAGE_DATA_KEY, remote,
                            remote_page_free);

    /* Services, screen size info and set button */
    box_left = gtk_box_new (GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width (GTK_CONTAINER (box_left), 8);
    gtk_box_pack_start (GTK_BOX (box_left), create_services_widget (remote),
                        FALSE, FALSE, 4);

    label = gtk_label_new (
            "Wallpaper change takes an image from one of the selected "
            "services. With \"Random wallpaper change\" the service is "
            "chosen randomly, without it the services are used one after "
            "another. Images are kept in the cache, an image of the day is "
            "downloaded only once.");
    gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
    gtk_label_set_xalign (GTK_LABEL (label), 0);
    gtk_box_pack_start (GTK_BOX (box_left), label, FALSE, FALSE, 4);

    text = (remote->screen_width > 0 && remote->screen_height > 0) ?
        g_strdup_printf ("Image size is chosen for a %d×%d screen.",
                         remote->screen_width, remote->screen_height) :
        g_strdup ("Screen size is not known, images for a 1920×1080 "
                  "screen will be chosen.");
    label = gtk_label_new (text);
    g_free (text);
    gtk_label_set_xalign (GTK_LABEL (label), 0);
    gtk_box_pack_start (GTK_BOX (box_left), label, FALSE, FALSE, 4);

    remote->set_button = gtk_button_new_with_label ("Set wallpaper now");
    gtk_widget_set_tooltip_text (remote->set_button,
            "Get a wallpaper from the selected services and set it");
    g_signal_connect_swapped (remote->set_button, "clicked",
                              G_CALLBACK (event_set_now_pressed), page);
    remote->spinner = gtk_spinner_new ();

    box_btn = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_pack_start (GTK_BOX (box_btn), remote->set_button,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (box_btn), remote->spinner,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (box_left), box_btn, FALSE, FALSE, 4);

    /* Wallpaper preview and its info */
    remote->preview = gtk_image_new ();
    preview_from_file (remote->preview, nullptr);
    remote->info = gtk_label_new (nullptr);
    gtk_label_set_line_wrap (GTK_LABEL (remote->info), TRUE);
    gtk_label_set_max_width_chars (GTK_LABEL (remote->info), 32);
    gtk_label_set_justify (GTK_LABEL (remote->info), GTK_JUSTIFY_CENTER);

    box_right = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start (GTK_BOX (box_right), remote->preview,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (box_right), remote->info,
                        FALSE, FALSE, 4);

    gtk_box_pack_start (GTK_BOX (page), box_left, TRUE, TRUE, 4);
    gtk_box_pack_start (GTK_BOX (page),
                        gtk_separator_new (GTK_ORIENTATION_VERTICAL),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (page), box_right, FALSE, FALSE, 4);

    return page;
}
/*----------------------------------------------------------------------------*/
