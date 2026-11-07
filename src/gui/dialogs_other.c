/**
 * @file  dialogs_other.c
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
 * @brief  Other dialogs
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <inttypes.h>
#include <err.h>
#include "defs.h"
#include "image_dialogs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with information about application.
 */
void
about_app_dialog (GtkWindow *parent)
{
    GtkWidget  *dialog;
    const char *authors[] = {"Michal Babik <michal.babik@protonmail.com> "
                               "https://init6.pomorze.pl", nullptr};
    const char *art[]     = {
        "https://fr.wikisource.org/wiki/Le_Tour_de_la_France_par_deux_enfants",
        nullptr};

    dialog = gtk_about_dialog_new ();
    gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);

    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), APP_VER);

    gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), 
            "Copyright (C) 2019-2026 Michal Babik");

    gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), APP_DSC);

    gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG (dialog),
                                       GTK_LICENSE_GPL_3_0);

    gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), APP_WEB);

    gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
    gtk_about_dialog_set_artists (GTK_ABOUT_DIALOG (dialog), art);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}
/*----------------------------------------------------------------------------*/

