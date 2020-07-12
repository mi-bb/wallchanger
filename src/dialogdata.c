/**
 * @file  dialogdata.c
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
 * @brief  DialogData structure and functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <err.h>
#include "errs.h"
#include "cfgfile.h"
#include "dialogdata.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  DialogData initialization.
 *
 * @param[out]  dd_data  DialogData item
 * @return      none
 */
static void dialogdata_init (DialogData *dd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free DialogData object.
 */
void
dialogdata_free (DialogData *dd_data)
{
    free (dd_data->s_cfg_file);
    free (dd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  DialogData initialization.
 */
static void
dialogdata_init (DialogData *dd_data)
{
    dd_data->gw_window      = NULL;
    dd_data->gw_view        = NULL;
    dd_data->gw_random      = NULL;
    dd_data->gw_lastused    = NULL;
    dd_data->gw_timealign   = NULL;
    dd_data->gw_command     = NULL;
    dd_data->gw_interval    = NULL;
    dd_data->gw_inter_combo = NULL;
    dd_data->gw_dm_label    = NULL;
    dd_data->gw_statusbar   = NULL;
    dd_data->gw_imgprev     = NULL;
    dd_data->s_cfg_file     = NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new DialogData object.
 */
DialogData *
dialogdata_new (void)
{
    DialogData *dd_data = NULL;   /* DialogData item to return */

    if ((dd_data = malloc (sizeof (DialogData))) == NULL)
        err (EXIT_FAILURE, NULL);

    dialogdata_init (dd_data);

    return dd_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Look for config file, try to create it if not found. When
 *         s_cfg_file from dd_data is not null, checks it. Exits on fail.
 */
void
dialogdata_do_config_file_stuff (DialogData *dd_data)
{
    if (cfgfile_config_file_stuff (&dd_data->s_cfg_file, 1) != ERR_OK) {
        dialogdata_free (dd_data);
        exit (EXIT_FAILURE);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get information about config file for status bar.
 */
char *
dialogdata_get_status_config_info (const DialogData *dd_data)
{
    char   *s_ret   = NULL; /* Result string */
    size_t  ui_plen = 0;    /* Prefix text length */
    size_t  ui_clen = 0;    /* Config file name length */

    ui_plen = strlen (STATUS_CONFIG);
    ui_clen = strlen (dd_data->s_cfg_file);

    if ((s_ret = malloc ((ui_plen + ui_clen + 1) * sizeof (char))) == NULL)
        err (EXIT_FAILURE, NULL);

    memcpy (s_ret, STATUS_CONFIG, ui_plen);
    memcpy (s_ret + ui_plen, dd_data->s_cfg_file, ui_clen);
    s_ret[ui_plen + ui_clen] = '\0';

    return s_ret;
}
/*----------------------------------------------------------------------------*/

