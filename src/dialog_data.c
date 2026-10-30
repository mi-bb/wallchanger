/**
 * @file  dialog_data.c
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
 * @brief  DialogData structure and functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <err.h>
#include "errors.h"
#include "cfg_file.h"
#include "dialog_data.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  DialogData initialization.
 *
 * @param[out]  data  DialogData item
 * @return      none
 */
static void dialog_data_init (DialogData *data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free DialogData object.
 */
void
dialog_data_free (DialogData *data)
{
    free (data->config_file);
    free (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  DialogData initialization.
 */
static void
dialog_data_init (DialogData *data)
{
    data->window         = nullptr;
    data->view           = nullptr;
    data->random         = nullptr;
    data->last_used      = nullptr;
    data->time_align     = nullptr;
    data->command        = nullptr;
    data->interval       = nullptr;
    data->interval_combo = nullptr;
    data->daemon_label   = nullptr;
    data->status_bar     = nullptr;
    data->img_preview    = nullptr;
    data->config_file    = nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new DialogData object.
 */
DialogData *
dialog_data_new (void)
{
    DialogData *data = nullptr;   /* DialogData item to return */

    if ((data = malloc (sizeof (DialogData))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    dialog_data_init (data);

    return data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config file, try to create it if not found. When
 *         config_file from data is not null, checks it. Exits on fail.
 */
void
dialog_data_do_config_file_stuff (DialogData *data)
{
    if (cfg_file_config_file_stuff (&data->config_file, 1) != ERR_OK) {
        dialog_data_free (data);
        exit (EXIT_FAILURE);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get information about config file for status bar.
 */
char *
dialog_data_get_status_config_info (const DialogData *data)
{
    char   *result      = nullptr; /* Result string */
    size_t  prefix_len  = 0;       /* Prefix text length */
    size_t  config_len  = 0;       /* Config file name length */

    prefix_len = strlen (STATUS_CONFIG);
    config_len = strlen (data->config_file);

    if ((result = malloc ((prefix_len + config_len + 1) * sizeof (char))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    memcpy (result, STATUS_CONFIG, prefix_len);
    memcpy (result + prefix_len, data->config_file, config_len);
    result[prefix_len + config_len] = '\0';

    return result;
}
/*----------------------------------------------------------------------------*/

