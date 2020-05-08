/**
 * @file  settlist.c
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
 * @brief  SettList structure and functions
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include "settlist.h"
#include "hashfun.h"
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
 * @param[in]   ui_size  Size to reserve
 * @return      none
 */
static void stlist_reserve (SettList     *st_list,
                            const size_t  ui_size);
/*----------------------------------------------------------------------------*/
/**
 * @brief     Get position of setting with specified id
 *
 * @param[in] st_list  SettList list of settings
 * @param[in] ui_id    Setting id number
 * @return    Position in list or -1 if not found
 */
static int_fast32_t stlist_get_setting_with_id_pos (const SettList     *st_list,
                                                    const uint_fast32_t ui_id)
                                                    __attribute__ ((pure));
/*----------------------------------------------------------------------------*/
/**
 * @fn  SettList * stlist_get_settings_owned_by (const SettList *st_list,
 *                                               const uint32_t  ui_oid)
 *
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
 *
 * @param[in]  st_list  SettList list of settings
 * @param[in]  ui_oid   Id of owner array
 * @return     SettList list of pointers to Setting objects
 */
/*----------------------------------------------------------------------------*/
static SettList * stlist_get_settings_owned_by   (const SettList      *st_list,
                                                  const uint_fast32_t  ui_oid);

static SettList * stlist_get_settings_owned_by_p (const SettList      *st_list,
                                                  const uint_fast32_t  ui_oid);
/*----------------------------------------------------------------------------*/
/**
 * @brief  SettList initialization
 */
static void
stlist_init (SettList *st_list)
{
    st_list->i_cnt      = 0;
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

    st_list = malloc (sizeof (SettList));

    if (st_list == NULL) {
        err (EXIT_FAILURE, NULL);
    }

    stlist_init (st_list);

    return st_list;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief      Reserve space for Setting objects in SettList
 */
static void
stlist_reserve (SettList     *st_list,
                const size_t  ui_size)
{
    Setting **s_tmp = NULL;
    size_t    i     = 0;

    /* No need to resie */
    if (st_list->i_cnt == ui_size)
        return;

    /* if larger free rest */
    while (ui_size < st_list->i_cnt) {
        setting_free (st_list->st_setting[--st_list->i_cnt]);
    }

    /* If size 0 clear list */
    if (ui_size == 0) {
        free (st_list->st_setting);
        stlist_init (st_list);
        return; 
    }

    /* Malloc if null, realloc if not null */
    if (st_list->st_setting == NULL) {
        st_list->st_setting = malloc (ui_size * sizeof (Setting*));
        if (st_list->st_setting == NULL) {
            err (EXIT_FAILURE, NULL);
        }
    }
    else {
        s_tmp = realloc (st_list->st_setting, (ui_size) * sizeof (Setting*));
        if (s_tmp == NULL) {
            for (i = 0; i < st_list->i_cnt; ++i) {
                setting_free (st_list->st_setting[i]);
            }
            free (st_list->st_setting);
            err (EXIT_FAILURE, NULL);
        }
        else {
            st_list->st_setting = s_tmp;
        }
    }
    /* Update file list count */
    st_list->i_cnt = (size_t) ui_size;
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
/**
 * @brief  Free SettList pointer list
 */
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
size_t
stlist_get_length (const SettList *st_list)
{
    return st_list->i_cnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Inserts Settings object to settings list
 */
void
stlist_insert_setting (SettList *st_list,
                       Setting  *st_val)
{
    int_fast32_t  i_pos = 0;
    const char   *s_name = NULL;

    s_name = setting_get_name (st_val);
    i_pos  = (s_name == NULL) ? -1 : stlist_get_setting_pos (st_list, s_name);

    if (i_pos < 0) {
        /* Resize list and append new one */
        stlist_reserve (st_list, st_list->i_cnt+1);
        st_list->st_setting[st_list->i_cnt-1] = st_val;
    }
    else {
        /* Remove old and assing new setting */
        setting_free (st_list->st_setting[i_pos]);
        st_list->st_setting[i_pos] = st_val;
    }
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

    if (s_array_name != NULL && s_array_name[0] != '\0') {

        st_array = stlist_get_setting_with_name (st_list, s_array_name);
        if (st_array == NULL)
            return SET_ER_NO_ARR;

        if (setting_get_type (st_array) != SET_VAL_ARRAY)
            return SET_ER_NOT_ARR;

        setting_assign_to_array (st_val, s_array_name);
        stlist_insert_setting (st_list, st_val);
    }
    else
        stlist_insert_setting (st_list, st_val);
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief     Get position of setting with specified id
 */
static int_fast32_t
stlist_get_setting_with_id_pos (const SettList      *st_list,
                                const uint_fast32_t  ui_id)
{
    Setting *st_act;
    size_t   ui_cnt = 0;
    size_t   i      = 0;

    ui_cnt  = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {

        st_act = stlist_get_setting_at_pos (st_list, i);

        if (st_act != NULL && setting_get_id (st_act) == ui_id) {
            return (int_fast32_t) i;
        }
    }
    return -1;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get position of setting with name s_name on list
 */
int_fast32_t
stlist_get_setting_pos (const SettList *st_list,
                        const char     *s_name)
{
    int_fast32_t  i_pos   = -1;
    uint_fast32_t ui_hash = 0;

    ui_hash = (s_name != NULL && s_name[0] != '\0') ? hash (s_name) : 0;
    i_pos   = stlist_get_setting_with_id_pos (st_list, ui_hash);

    return i_pos;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get position on list of setting with string type and value s_val
 */
int_fast32_t
stlist_get_setting_val_str_pos (const SettList *st_list,
                                const char     *s_val)
{
    Setting      *st_sett;
    size_t        ui_cnt = 0;
    size_t        i      = 0;

    ui_cnt = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {

        st_sett = stlist_get_setting_at_pos (st_list, i);

        if (setting_get_type (st_sett) == SET_VAL_STRING) {

            if (strcmp (s_val, setting_get_string (st_sett)) == 0) {
                return (int_fast32_t) i;
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
                           const size_t    ui_pos)
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
    Setting      *st_ret;
    int_fast32_t  i_pos = -1;

    i_pos = stlist_get_setting_pos (st_list, s_name);

    if (i_pos == -1) {
        return NULL;
    }
    else {
        st_ret = stlist_get_setting_at_pos (st_list, (size_t) i_pos);
    }
    return st_ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of pointers to Setting objects owned by array with owner
 *         id ui_oid
 */
static SettList *
stlist_get_settings_owned_by_p (const SettList      *st_list,
                                const uint_fast32_t  ui_oid) 
{
    SettList      *st_res;
    Setting       *st_val;
    size_t         ui_cnt     = 0;
    uint_fast32_t  ui_set_oid = 0;
    size_t         i          = 0;

    st_res = stlist_new_list ();
    ui_cnt = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {

        ui_set_oid = setting_get_owner_id (st_list->st_setting[i]);

        if (ui_set_oid == ui_oid) {

            st_val = st_list->st_setting[i];
            stlist_insert_setting (st_res, st_val);
        }
    }
    return st_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get list of Setting objects owned by array with owner id ui_oid
 */
static SettList *
stlist_get_settings_owned_by (const SettList      *st_list,
                              const uint_fast32_t  ui_oid) 
{
    SettList      *st_res;
    Setting       *st_val;
    size_t         ui_cnt     = 0;
    uint_fast32_t  ui_set_oid = 0;
    size_t         i          = 0;

    st_res = stlist_new_list ();
    ui_cnt = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {

        ui_set_oid = setting_get_owner_id (st_list->st_setting[i]);

        if (ui_set_oid == ui_oid) {

            st_val = setting_copy (st_list->st_setting[i]);
            stlist_insert_setting (st_res, st_val);
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
    SettList      *st_res;
    Setting       *st_array;
    uint_fast32_t  ui_array_id = 0;

    if (s_name != NULL && s_name[0] != '\0') {
        st_array = stlist_get_setting_with_name (st_list, s_name);
        if (st_array == NULL)
            return NULL;

        if (setting_get_type (st_array) != SET_VAL_ARRAY)
            return NULL;

        ui_array_id = setting_get_id (st_array);
    }
    else
        ui_array_id = 0;

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
    Setting       *st_array;
    uint_fast32_t  ui_array_id = 0;

    if (s_name != NULL && s_name[0] != '\0') {
        st_array = stlist_get_setting_with_name (st_list, s_name);
        if (st_array == NULL)
            return NULL;

        if (setting_get_type (st_array) != SET_VAL_ARRAY)
            return NULL;

        ui_array_id = setting_get_id (st_array);
    }
    else
        ui_array_id = 0;

    return stlist_get_settings_owned_by_p (st_list, ui_array_id);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get SettList list of Setting objects stored in array.
 */
SettList *
stlist_get_settings_in_array_obj (const SettList *st_list,
                                  const Setting  *st_array)
{
    if (setting_get_type (st_array) != SET_VAL_ARRAY)
        return NULL;

    return stlist_get_settings_owned_by (st_list, setting_get_id (st_array));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get SettList list of pointers to Setting objects stored in array.
 */
SettList *
stlist_get_settings_in_array_obj_p (const SettList *st_list,
                                    const Setting  *st_array)
{
    if (setting_get_type (st_array) != SET_VAL_ARRAY)
        return NULL;

    return stlist_get_settings_owned_by_p (st_list,
                                           setting_get_id (st_array));
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
void
stlist_remove_setting_at_pos (SettList     *st_list,
                              const size_t  ui_pos)
{
    size_t   ui_len = 0;
    size_t   i      = 0;
    Setting *st_set;

    ui_len = stlist_get_length (st_list);

    if (ui_pos >= ui_len)
        return;

    st_set = stlist_get_setting_at_pos (st_list, ui_pos);

    for (i = ui_pos; i < ui_len - 1; ++i) {
        st_list->st_setting[i] = st_list->st_setting[i + 1];
    }

    st_list->st_setting[ui_len - 1] = st_set;
    stlist_reserve (st_list, ui_len - 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove Settings from list 
 */
void
stlist_remove_setting (SettList *st_list,
                       Setting  *st_val)
{
    int_fast32_t  i_pos   = 0;
    uint_fast32_t ui_hash = 0;
    size_t        ui_cnt  = 0;

    ui_hash = setting_get_id (st_val);
    i_pos   = stlist_get_setting_with_id_pos (st_list, ui_hash);

    if (i_pos == -1)
        return;

    if (ui_hash == 0) {
        ui_cnt = stlist_get_length (st_list);

        while ((size_t) i_pos < ui_cnt) {
            if (setting_compare (st_val, st_list->st_setting[i_pos]) == 0) {
                stlist_remove_setting_at_pos (st_list, (size_t) i_pos);
                return;
            }
            ++i_pos;
        }
    }
    else {
        stlist_remove_setting_at_pos (st_list, (size_t) i_pos);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove Settings with specified name from list 
 */
void
stlist_remove_setting_with_name (SettList   *st_list,
                                 const char *s_name)
{
    int_fast32_t  i_pos   = 0;
    uint_fast32_t ui_hash = 0;

    ui_hash = (s_name != NULL && s_name[0] != '\0') ? hash (s_name) : 0;
    i_pos   = stlist_get_setting_with_id_pos (st_list, ui_hash);

    if (i_pos == -1)
        return;

    stlist_remove_setting_at_pos (st_list, (size_t) i_pos);
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
/**
 * @brief  Print content of SettList
 */
void
stlist_print_content (const SettList *st_list)
{
    size_t ui_cnt = 0;
    size_t i      = 0;

    ui_cnt = stlist_get_length (st_list);

    for (i = 0; i < ui_cnt; ++i) {
        setting_print (st_list->st_setting[i]);
    }
}
/*----------------------------------------------------------------------------*/
#endif

