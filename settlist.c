/**
 * @file  settlist.c
 * @copyright Copyright (C) 2019 Michał Bąbik
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
 * @brief  SettList structure and functions
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "settlist.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  SettList initialization
 *
 * @param[out]  st_list  SettList list of settings
 * @return      none
 */
static void stlist_init (SettList *st_list);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reserve space for Setting objects in SettList
 *
 * @param[out]  st_list  SettList list of settings
 * @param[in]   i_size   Size to reserve
 * @return      Reserve result 0 if OK, 1 if error occurred
 */
static int stlist_reserve (SettList    *st_list,
                           const size_t i_size);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of Setting objects owned by array with owner id ui_oid
 *
 * @param[out]  st_list  SettList list of settings
 * @param[in]   ui_oid   Id of owner array
 * @return      SettList list of Setting objects
 */
static SettList * stlist_get_settings_owned_by (SettList      *st_list,
                                                const uint64_t ui_oid);
/*----------------------------------------------------------------------------*/
static void
stlist_init (SettList *st_list)
{
    st_list->i_cnt = 0;
    st_list->st_setting = NULL;
}
/*----------------------------------------------------------------------------*/
SettList *
stlist_new_list (void)
{
    SettList *st_list; 

    st_list = calloc (1, sizeof (SettList));

    if (st_list == NULL)
        return NULL;

    stlist_init (st_list);

    return st_list;
}
/*----------------------------------------------------------------------------*/
uint32_t
stlist_get_length (SettList *st_list)
{
    if (st_list == NULL)
        return 0;

    return st_list->i_cnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Reserve space for Setting objects in SettList
 */
static int
stlist_reserve (SettList    *st_list,
                const size_t i_size)
{
    Setting **s_tmp = NULL;
    uint32_t  i     = 0;

    if (st_list == NULL)
        return SET_ER_OK;

    /* No need to resie */
    if (st_list->i_cnt == i_size)
        return SET_ER_OK;

    /* if larger free rest */
    while (i_size < st_list->i_cnt) {
        setting_free (st_list->st_setting[--st_list->i_cnt]);
    }

    /* If size 0 clear list */
    if (i_size == 0) {
        free (st_list->st_setting);
        stlist_init (st_list);
        return SET_ER_OK; 
    }

    /* Malloc if null, realloc if not null */
    if (st_list->st_setting == NULL) {
        st_list->st_setting = calloc (i_size, sizeof (Setting*));
    }
    else {
        s_tmp = realloc (st_list->st_setting, (i_size) * sizeof (Setting*));
        if (s_tmp == NULL) {
            for (i = 0; i < st_list->i_cnt; ++i)
                setting_free (st_list->st_setting[i]);
            free (st_list->st_setting);
            /*return ERR_ALLOC;*/
        }
        else {
            st_list->st_setting = s_tmp;
        }
    }

    if (i_size != 0 && st_list->st_setting == NULL) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
        /*return ERR_ALLOC;*/
    }

    /* Update file list count */
    st_list->i_cnt = i_size;

    return SET_ER_OK;
}
/*----------------------------------------------------------------------------*/
void
stlist_free (SettList *st_list)
{
    if (st_list == NULL)
        return;

    stlist_reserve (st_list, 0);
    free (st_list);
}
/*----------------------------------------------------------------------------*/
void
stlist_clear (SettList *st_list)
{
    stlist_reserve (st_list, 0);
}
/*----------------------------------------------------------------------------*/
int
stlist_insert_setting (SettList *st_list,
                       Setting  *st_val)
{
    int     i_res = SET_ER_OK;
    int32_t i_pos = 0;

    if (st_val == NULL)
        return SET_ER_OK;

    if (setting_get_name (st_val) == NULL)
        return SET_ER_OK;

    i_pos = stlist_get_setting_pos (st_list,
                                    setting_get_name (st_val));
    if (i_pos >= 0) {
        /* Remove old and assing new setting */
        setting_free (st_list->st_setting[i_pos]);
        st_list->st_setting[i_pos] = st_val;
    }
    else {
        /* Resize list and append new one */
        i_res = stlist_reserve (st_list, st_list->i_cnt+1);
        if (i_res)
            return i_res;

        st_list->st_setting[st_list->i_cnt-1] = st_val;
    }

    return i_res;
}
/*----------------------------------------------------------------------------*/
int
stlist_insert_setting_to_array (SettList   *st_list,
                                Setting    *st_val,
                                const char *s_array_name)
{
    Setting *st_array;
    int      i_res = SET_ER_OK;

    if (st_list == NULL || st_val == NULL || s_array_name == NULL)
        return SET_ER_OK;

    if (strcmp (s_array_name, "") == 0) {
        i_res = stlist_insert_setting (st_list, st_val);
    }
    else {

        st_array = stlist_get_setting_with_name (st_list, s_array_name);
        if (st_array == NULL)
            return SET_ER_NO_ARR;

        if (setting_get_type (st_array) != SET_VAL_ARRAY)
            return SET_ER_NOT_ARR;

        setting_assign_to_array (st_val, s_array_name);

        i_res = stlist_insert_setting (st_list, st_val);
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
int32_t
stlist_get_setting_pos (SettList   *st_list,
                        const char *s_name)
{
    Setting  *st_act;
    int32_t   i_pos   = -1;
    uint32_t  ui_cnt  = 0;
    uint64_t  ui_hash = 0;
    uint32_t  i       = 0;

    if (s_name == NULL || st_list == NULL)
        return i_pos;

    ui_hash = setting_hashfun (s_name);
    ui_cnt  = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {
        st_act = stlist_get_setting_at_pos (st_list, i);
        if (st_act != NULL && setting_get_id (st_act) == ui_hash) {
            i_pos = i;
            break;   
        }
    }
    return i_pos;
}
/*----------------------------------------------------------------------------*/
Setting *
stlist_get_setting_at_pos (SettList      *st_list,
                           const uint32_t ui_pos)
{
    if (st_list == NULL)
        return NULL;

    if (ui_pos > st_list->i_cnt - 1) 
        return NULL;

    return st_list->st_setting[ui_pos];
}
/*----------------------------------------------------------------------------*/
Setting *
stlist_get_setting_with_name (SettList   *st_list,
                              const char *s_name)
{
    Setting *st_ret;
    int32_t  i_pos = 0;

    if (st_list == NULL || s_name == NULL)
        return NULL;

    if (strcmp (s_name, "") == 0)
        return NULL;

    i_pos = stlist_get_setting_pos (st_list, s_name);

    if (i_pos == -1) {
        return NULL;
    }
    else {
        st_ret = stlist_get_setting_at_pos (st_list, (uint32_t) i_pos);
    }
    return st_ret;
}
/*----------------------------------------------------------------------------*/
static SettList *
stlist_get_settings_owned_by (SettList      *st_list,
                              const uint64_t ui_oid) 
{
    SettList *st_res;
    Setting *st_val;
    uint32_t ui_cnt     = 0;
    uint64_t ui_set_oid = 0;
    int      i_res      = 0;
    uint32_t i          = 0;

    st_res = stlist_new_list ();

    ui_cnt = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {
        ui_set_oid = setting_get_owner_id (st_list->st_setting[i]);
        if (ui_set_oid == ui_oid) {

            st_val = setting_copy (st_list->st_setting[i]);

            i_res = stlist_insert_setting (st_res, st_val);
            if (i_res != SET_ER_OK) {
                stlist_free (st_res);
                return NULL;
            }
        }
    }
    return st_res;
}
/*----------------------------------------------------------------------------*/
SettList *
stlist_get_settings_in_array_name (SettList   *st_list,
                                   const char *s_name) 
{
    SettList *st_res;
    Setting  *st_array;
    uint64_t  ui_array_id = 0;

    if (st_list == NULL || s_name == NULL)
        return NULL;

    if (strcmp (s_name, "") == 0) {
        ui_array_id = 0;
    }
    else {
        st_array = stlist_get_setting_with_name (st_list, s_name);
        if (st_array == NULL)
            return NULL;

        if (setting_get_type (st_array) != SET_VAL_ARRAY)
            return NULL;

        ui_array_id = setting_get_id (st_array);
    }

    st_res = stlist_get_settings_owned_by (st_list, ui_array_id);

    return st_res;
}
/*----------------------------------------------------------------------------*/
SettList *
stlist_get_settings_in_array_obj (SettList *st_list,
                                  Setting  *st_array)
{
    SettList *st_res;

    if (st_list == NULL || st_array == NULL)
        return NULL;

    if (setting_get_type (st_array) != SET_VAL_ARRAY)
        return NULL;

    st_res = stlist_get_settings_owned_by (st_list, setting_get_id (st_array));

    return st_res;
}
/*----------------------------------------------------------------------------*/
SettList *
stlist_get_settings_main (SettList *st_list) 
{
    return stlist_get_settings_owned_by (st_list, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print content of SettList list
 */
void
stlist_print_content (SettList *st_list)
{
    uint32_t ui_cnt = 0;
    uint32_t i      = 0;

    if (st_list == NULL)
        return;

    ui_cnt = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {
        setting_print (st_list->st_setting[i]);
    }
}
/*----------------------------------------------------------------------------*/

