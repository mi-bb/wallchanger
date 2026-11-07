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
 * @brief  wchangercfg entry point: application setup and signals
 *
 * Automatic wallpaper changer
 *
 * @version 1.7.2
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <string.h>
#include <gtk/gtk.h>
#include "dialog_data.h"
#include "main_window.h"
#include "defs.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static void shutdown (GtkApplication *application, DialogData *data);
 *
 * @brief  Application shutdown signal.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @param[in,out] data         DialogData object with widgets and settings info
 * @return        none
 *
 * @fn  static gint local_options (GApplication *application,
 *                                 GVariantDict *options,
 *                                 DialogData   *data)
 *
 * @brief  Handling command line options.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @param[in]     options      Options dictionary
 * @param[in,out] data         DialogData object with widgets and settings info
 *
 * @fn  static void activate (GtkApplication *app, DialogData *data)
 *
 * @brief  Application activate signal.
 *
 * @param[in,out] app   Pointer to GtkApplication
 * @param[in,out] data  DialogData object with settings and widget data
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static void     shutdown        (GtkApplication   *application,
                                 DialogData       *data);

static gint     local_options   (GApplication     *application,
                                 GVariantDict     *options,
                                 DialogData       *data);

static void     activate        (GtkApplication   *app,
                                 DialogData       *data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application activate signal.
 */
static void
activate (GtkApplication *app,
          DialogData     *data)
{
    main_window_create (app, data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application shutdown signal.
 */
static void
shutdown ([[maybe_unused]] GtkApplication *application,
          DialogData *data)
{
    dialog_data_free (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Handling command line options.
 */
static gint
local_options ([[maybe_unused]] GApplication *application,
               GVariantDict *options,
               DialogData   *data)
{
    char *str = nullptr;

    if (g_variant_dict_lookup (options, "config", "s", &str)) {
        data->config_file = strdup (str);
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
    DialogData     *data;
    int             status;

    data = dialog_data_new ();
    app = gtk_application_new ("org.nongnu.WallChanger",
                               G_APPLICATION_DEFAULT_FLAGS);
    g_application_add_main_option (G_APPLICATION (app),
                                   "config",
                                   0,
                                   G_OPTION_FLAG_IN_MAIN,
                                   G_OPTION_ARG_STRING,
                                   "Path to config file",
                                   "FILENAME");
    g_signal_connect (app, "activate", G_CALLBACK (activate), data);
    g_signal_connect (app, "shutdown", G_CALLBACK (shutdown), data);
    g_signal_connect (app, "handle-local-options",
            G_CALLBACK (local_options), data);
    g_set_application_name (APP_NAME);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
/*----------------------------------------------------------------------------*/

