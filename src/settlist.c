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
 * @param[in]   ul_size  Size to reserve
 * @return      Reserve result
 */
static int stlist_reserve (SettList     *st_list,
                           const size_t  ul_size);
/*----------------------------------------------------------------------------*/
/**
 * @brief     Get position of setting with specified id
 *
 * @param[in] st_list  SettList list of settings
 * @param[in] ui_id    Setting id number
 * @return    Position in list or -1 if not found
 */
static int32_t stlist_get_setting_with_id_pos (const SettList *st_list,
                                               uint32_t        ui_id);
/*----------------------------------------------------------------------------*/
/**
 * @fn  SettList * stlist_get_settings_owned_by (const SettList *st_list,
 *                                               const uint32_t  ui_oid)
 * @brief  Get list of Setting objects owned by array with owner id ui_oid
 *
 * @param[out]  st_list  SettList list of settings
 * @param[in]   ui_oid   Id of owner array
 * @return      SettList list of Setting objects
 *
 * @fn  SettList * stlist_get_settings_owned_by_p (const SettList *st_list,
                                                   const uint32_t  ui_oid)
 * @brief  Get list of pointers to Setting objects owned by array with owner
 *         id ui_oid
 * @param[in]  st_list  SettList list of settings
 * @param[in]  ui_oid   Id of owner array
 * @return     SettList list of pointers to Setting objects
 */
/*----------------------------------------------------------------------------*/
static SettList * stlist_get_settings_owned_by   (const SettList *st_list,
                                                  const uint32_t  ui_oid);

static SettList * stlist_get_settings_owned_by_p (const SettList *st_list,
                                                  const uint32_t  ui_oid);
/*----------------------------------------------------------------------------*/
/**
 * @brief  SettList initialization
 */
static void
stlist_init (SettList *st_list)
{
    st_list->i_cnt = 0;
    st_list->st_setting = NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new empty list for settings
 */
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
/**
 * @brief      Reserve space for Setting objects in SettList
 */
static int
stlist_reserve (SettList     *st_list,
                const size_t  ul_size)
{
    Setting **s_tmp = NULL;
    uint32_t  i     = 0;

    /* No need to resie */
    if (st_list->i_cnt == ul_size)
        return SET_ER_OK;

    /* if larger free rest */
    while (ul_size < st_list->i_cnt) {
        setting_free (st_list->st_setting[--st_list->i_cnt]);
    }

    /* If size 0 clear list */
    if (ul_size == 0) {
        free (st_list->st_setting);
        stlist_init (st_list);
        return SET_ER_OK; 
    }

    /* Malloc if null, realloc if not null */
    if (st_list->st_setting == NULL) {
        st_list->st_setting = calloc (ul_size, sizeof (Setting*));
    }
    else {
        s_tmp = realloc (st_list->st_setting, (ul_size) * sizeof (Setting*));
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

    if (st_list->st_setting == NULL && ul_size != 0) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
        /*return ERR_ALLOC;*/
    }

    /* Update file list count */
    st_list->i_cnt = (uint32_t) ul_size;

    return SET_ER_OK;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free SettList list
 */
void
stlist_free (SettList *st_list)
{
    stlist_reserve (st_list, 0);
    free (st_list);
}
/*----------------------------------------------------------------------------*/
void
stlist_free_p (SettList *st_list)
{
    free (st_list->st_setting);
    free (st_list);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Clear SettList list of Setting items
 */
void
stlist_clear (SettList *st_list)
{
    stlist_reserve (st_list, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get number of settings in list
 */
uint32_t
stlist_get_length (const SettList *st_list)
{
    return st_list->i_cnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Inserts Settings object to settings list
 */
int
stlist_insert_setting (SettList *st_list,
                       Setting  *st_val)
{
    int     i_res = SET_ER_OK;
    int32_t i_pos = 0;

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
/**
 * @brief  Inserts Settings object to array in settings list
 */
int
stlist_insert_setting_to_array (SettList   *st_list,
                                Setting    *st_val,
                                const char *s_array_name)
{
    Setting *st_array;
    int      i_res = SET_ER_OK;

    if (s_array_name == NULL || strcmp (s_array_name, "") == 0) {
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
/**
 * @brief     Get position of setting with specified id
 */
static int32_t
stlist_get_setting_with_id_pos (const SettList *st_list,
                                uint32_t        ui_id)
{
    int32_t   i_pos   = -1;
    uint32_t  ui_cnt  = 0;
    uint32_t  i       = 0;
    Setting  *st_act;

    ui_cnt  = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {

        st_act = stlist_get_setting_at_pos (st_list, i);

        if (st_act != NULL && setting_get_id (st_act) == ui_id) {
            i_pos = (int32_t) i;
            break;
        }
    }
    return i_pos;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get position of setting with name s_name on list
 */
int32_t
stlist_get_setting_pos (const SettList *st_list,
                        const char     *s_name)
{
    int32_t   i_pos   = -1;
    uint32_t  ui_hash = 0;

    ui_hash = setting_hashfun (s_name);

    i_pos = stlist_get_setting_with_id_pos (st_list, ui_hash);

    return i_pos;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get position on list of setting with string type and value s_val
 */
int32_t
stlist_get_setting_val_str_pos (const SettList *st_list,
                                const char     *s_val)
{
    Setting *st_sett;
    uint32_t ui_cnt = stlist_get_length (st_list);

    for (uint32_t i = 0; i < ui_cnt; ++i) {

        st_sett = stlist_get_setting_at_pos (st_list, i);

        if (setting_get_type (st_sett) == SET_VAL_STRING) {

            if (strcmp (s_val, setting_get_string (st_sett)) == 0) {
                return (int32_t) i;
            }
        }
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get pointer to Setting object at given position in SettList
 */
Setting *
stlist_get_setting_at_pos (const SettList *st_list,
                           const uint32_t  ui_pos)
{
    if (ui_pos >= st_list->i_cnt) 
        return NULL;

    return st_list->st_setting[ui_pos];
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get pointer to Setting object with name provided in s_name
 */
Setting *
stlist_get_setting_with_name (const SettList *st_list,
                              const char     *s_name)
{
    Setting *st_ret;
    int32_t  i_pos = 0;

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
/**
 * @brief  Get list of pointers to Setting objects owned by array with owner
 *         id ui_oid
 */
static SettList *
stlist_get_settings_owned_by_p (const SettList *st_list,
                                const uint32_t  ui_oid) 
{
    SettList *st_res;
    Setting  *st_val;
    uint32_t  ui_cnt     = 0;
    uint32_t  ui_set_oid = 0;
    int       i_res      = 0;
    uint32_t  i          = 0;

    st_res = stlist_new_list ();

    ui_cnt = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {

        ui_set_oid = setting_get_owner_id (st_list->st_setting[i]);

        if (ui_set_oid == ui_oid) {

            st_val = st_list->st_setting[i];

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
/**
 * @brief  Get list of Setting objects owned by array with owner id ui_oid
 */
static SettList *
stlist_get_settings_owned_by (const SettList *st_list,
                              const uint32_t  ui_oid) 
{
    SettList *st_res;
    Setting  *st_val;
    uint32_t  ui_cnt     = 0;
    uint32_t  ui_set_oid = 0;
    int       i_res      = 0;
    uint32_t  i          = 0;

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
/**
 * @brief  Get SettList list of Setting objects stored in array.
 */
SettList *
stlist_get_settings_in_array_name (const SettList *st_list,
                                   const char     *s_name) 
{
    SettList *st_res;
    Setting  *st_array;
    uint32_t  ui_array_id = 0;

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
/**
 * @brief  Get SettList list of pointers to Setting objects stored in array.
 */
SettList *
stlist_get_settings_in_array_name_p (const SettList *st_list,
                                     const char     *s_name) 
{
    SettList *st_res;
    Setting  *st_array;
    uint32_t  ui_array_id = 0;

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

    st_res = stlist_get_settings_owned_by_p (st_list, ui_array_id);

    return st_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get SettList list of Setting objects stored in array.
 */
SettList *
stlist_get_settings_in_array_obj (const SettList *st_list,
                                  const Setting  *st_array)
{
    SettList *st_res;

    if (setting_get_type (st_array) != SET_VAL_ARRAY)
        return NULL;

    st_res = stlist_get_settings_owned_by (st_list, setting_get_id (st_array));

    return st_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get SettList list of pointers to Setting objects stored in array.
 */
SettList *
stlist_get_settings_in_array_obj_p (const SettList *st_list,
                                    const Setting  *st_array)
{
    SettList *st_res;

    if (setting_get_type (st_array) != SET_VAL_ARRAY)
        return NULL;

    st_res = stlist_get_settings_owned_by_p (st_list,
                                             setting_get_id (st_array));

    return st_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get Setting objects (vals and arrays) stord in SettList top level.
 */
SettList *
stlist_get_settings_main (const SettList *st_list) 
{
    return stlist_get_settings_owned_by (st_list, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get pointers to Setting objects (vals and arrays) stord in
 *         SettList top level.
 */
SettList *
stlist_get_settings_main_p (const SettList *st_list) 
{
    return stlist_get_settings_owned_by_p (st_list, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove Settings from list at specified position
 */
int
stlist_remove_setting_at_pos (SettList *st_list,
                              uint32_t  ui_pos)
{
    int      i_res = SET_ER_OK;
    uint32_t ui_len = 0;
    Setting *st_set;

    ui_len = stlist_get_length (st_list);

    if (ui_pos >= ui_len)
        return SET_ER_NO_SET;

    st_set = stlist_get_setting_at_pos (st_list, ui_pos);

    for (uint32_t i = ui_pos; i < ui_len - 1; ++i) {
        st_list->st_setting[i] = st_list->st_setting[i + 1];
    }

    st_list->st_setting[ui_len - 1] = st_set;

    i_res = stlist_reserve (st_list, ui_len - 1);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove Settings from list 
 */
int
stlist_remove_setting (SettList *st_list,
                       Setting  *st_val)
{
    int      i_res   = SET_ER_OK;
    int32_t  i_pos   = 0;
    uint32_t ui_hash = 0;

    ui_hash = setting_get_id (st_val);

    i_pos = stlist_get_setting_with_id_pos (st_list, ui_hash);

    if (i_pos == -1)
        return SET_ER_NO_SET;

    i_res = stlist_remove_setting_at_pos (st_list, (uint32_t) i_pos);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove Settings with specified name from list 
 */
int
stlist_remove_setting_with_name (SettList   *st_list,
                                 const char *s_name)
{
    int      i_res   = SET_ER_OK;
    int32_t  i_pos   = 0;
    uint32_t ui_hash = 0;

    ui_hash = setting_hashfun (s_name);

    i_pos = stlist_get_setting_with_id_pos (st_list, ui_hash);

    if (i_pos == -1)
        return SET_ER_NO_SET;

    i_res = stlist_remove_setting_at_pos (st_list, (uint32_t) i_pos);

    return i_res;
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
/**
 * @brief  Print content of SettList list
 */
void
stlist_print_content (const SettList *st_list)
{
    uint32_t ui_cnt = 0;
    uint32_t i      = 0;

    ui_cnt = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {
        setting_print (st_list->st_setting[i]);
    }
}
/*----------------------------------------------------------------------------*/
#endif

