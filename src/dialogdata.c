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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include "errs.h"
#include "cfgfile.h"
#include "dialogdata.h"
/*----------------------------------------------------------------------------*/
void
dialogdata_free (DialogData *dd_data)
{
    free (dd_data);
}
/*----------------------------------------------------------------------------*/
static int
dialogdata_init (DialogData *dd_data)
{
    dd_data->gw_window      = NULL;
    dd_data->gw_view        = NULL;
    dd_data->gw_random      = NULL;
    dd_data->gw_lastused    = NULL;
    dd_data->gw_command     = NULL;
    dd_data->gw_interval    = NULL;
    dd_data->gw_inter_combo = NULL;

    memset (dd_data->s_cfg_file, 0, sizeof (dd_data->s_cfg_file));

    return set_config_file_path (dd_data->s_cfg_file);
}
/*----------------------------------------------------------------------------*/
DialogData *
dialogdata_new (void)
{
    DialogData *dd_data = NULL;

    dd_data = malloc (sizeof (DialogData));

    if (dd_data == NULL) {
        fputs ("Alloc error\n", stderr);
        return NULL;
    }
    if (dialogdata_init (dd_data) != ERR_OK) {
        dialogdata_free (dd_data);
        return NULL;
    }

    return dd_data;
}
/*----------------------------------------------------------------------------*/
const char *
dialogdata_get_cfg_file (const DialogData *dd_data)
{
    return (const char*) dd_data->s_cfg_file;
}
/*----------------------------------------------------------------------------*/
