/**
 * @file  setting.c
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
 * @brief  Setting structure and functions
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifdef DEBUG
#include <stdio.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include "hashfun.h"
#include "setting.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting initialization.
 *
 * @param[out] st_set  Setting object
 * @return     none
 */
static void setting_init (Setting *st_set);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create default Setting.
 *
 * @param[in] s_name  Setting's name
 * @return    New Setting item
 */
static Setting * setting_new_default (const char *s_name)
       __attribute__ ((returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @brief Set st_child Setting to be a child of st_parent.
 *
 * @param[out] st_parent  Parent Setting to set child to
 * @param[out] st_child   Child setting to be assigned to st_parent Setting
 * @return     none
 */
static void setting_set_child (Setting *st_parent,
                               Setting *st_child);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting type value
 *
 * @param[in] st_setting  Setting item
 * @param[in] val         Type value to set
 * @return    none
 */
static inline void
setting_set_type (Setting          *st_setting,
                  const SetValType  val)
{
    st_setting->v_type = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting name string
 *
 * @param[in] st_setting  Setting item
 * @param[in] val         Name string to set
 * @return    none
 */
static inline void
setting_set_name (Setting    *st_setting,
                  const char *val)
{
    st_setting->s_name = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting hash value
 *
 * @param[in] st_setting  Setting item
 * @param[in] val         Hash value to set
 * @return    none
 */
static inline void
setting_set_hash (Setting       *st_setting,
                  uint_fast32_t  val)
{
    st_setting->hash = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free string data in Setting
 */
static inline void
setting_free_string (Setting *st_setting)
{
    free (st_setting->data.s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting initialization
 */
static void
setting_init (Setting *st_set)
{
    st_set->prev       = NULL;
    st_set->next       = NULL;
    st_set->parent     = NULL;
    st_set->v_type     = SET_VAL_INT;
    st_set->s_name     = NULL;
    st_set->hash       = 0;
    st_set->data.i_val = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free Setting data
 */
void
setting_free (Setting *st_set)
{
    if (st_set == NULL)
        return;

    free (st_set->s_name);

    if (setting_get_type (st_set) == SET_VAL_STRING) {
        setting_free_string (st_set);
    }
    free (st_set);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free all settings in given Setting
 */
void
settings_free_all (Setting *st_set)
{
    Setting *st_next  = NULL;
    Setting *st_act   = NULL;
    Setting *st_child = NULL;

    st_act = st_set;

    while (st_act != NULL) {
        st_next = st_act->next;

        if (setting_get_type (st_act) == SET_VAL_SETTING ||
            setting_get_type (st_act) == SET_VAL_ARRAY) {

            if ((st_child = setting_get_child (st_act)) != NULL)
                settings_free_all (st_child);
        }
        setting_free (st_act);

        st_act = st_next;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set setting to string type and its content to val.
 */
void
setting_set_string (Setting    *st_set,
                    const char *val)
{
    if (setting_get_type (st_set) == SET_VAL_STRING)
        setting_free_string (st_set);

    setting_set_type (st_set, SET_VAL_STRING);

    st_set->data.s_val = (val == NULL) ? strdup ("") : strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create default Setting.
 */
static Setting *
setting_new_default (const char *s_name)
{
    Setting *st_ret = NULL;

    if ((st_ret = malloc (sizeof (Setting))) == NULL)
        err (EXIT_FAILURE, NULL);

    setting_init (st_ret);

    /*if (s_name != NULL && s_name[0] != '\0') {*/
    if (s_name != NULL) {
        setting_set_name (st_ret, s_name);
        setting_set_hash (st_ret, hash (s_name));
    }
    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_setting (const char *s_name)
{
    Setting *st_ret = NULL;

    st_ret = setting_new_default (s_name);

    setting_set_type (st_ret, SET_VAL_SETTING);
    st_ret->data.st_child = NULL;

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_array (const char *s_name)
{
    Setting *st_ret = NULL;

    st_ret = setting_new_default (s_name);

    setting_set_type (st_ret, SET_VAL_ARRAY);
    st_ret->data.st_child = NULL;

    return st_ret;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_int (const char    *s_name,
                 const int64_t  val)
{
    Setting *st_set = NULL;

    st_set = setting_new_default (s_name);

    setting_set_type (st_set, SET_VAL_INT);
    st_set->data.i_val = val;

    return st_set;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_uint (const char     *s_name,
                  const uint64_t  val)
{
    Setting *st_set = NULL;

    st_set = setting_new_default (s_name);

    setting_set_type (st_set, SET_VAL_UINT);
    st_set->data.ui_val = val;

    return st_set;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_double (const char   *s_name,
                    const double  val)
{
    Setting *st_set = NULL;

    st_set = setting_new_default (s_name);

    setting_set_type (st_set, SET_VAL_DOUBLE);
    st_set->data.d_val = val;

    return st_set;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_string (const char *s_name,
                    const char *val)
{
    Setting *st_set = NULL;

    st_set = setting_new_default (s_name);

    setting_set_string (st_set, val);

    return st_set;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find and return first item in a Setting list.
 */
Setting *
setting_first (Setting *st_settings)
{
    Setting *st_first = NULL;

    if (st_settings == NULL)
        return NULL;

    st_first = st_settings;

    while (st_first->prev != NULL)
        st_first = st_first->prev;

    return st_first;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find and return last item in a Setting list.
 */
Setting *
setting_last (Setting *st_settings)
{
    Setting *st_last = NULL;

    if (st_settings == NULL)
        return NULL;

    st_last = st_settings;

    while (st_last->next != NULL)
        st_last = st_last->next;

    return st_last;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Prepend st_setting item to st_list list.
 */
Setting *
settings_prepend (Setting *st_list,
                  Setting *st_setting)
{
    Setting *st_first    = NULL;
    Setting *st_add_last = NULL;

    if (st_list == NULL)
        return st_setting;

    if (st_setting == NULL)
        return st_list;

    st_add_last = setting_last (st_setting);
    st_first    = setting_first (st_list);

    st_first->prev     = st_add_last;
    st_add_last->next  = st_first;
    st_setting->prev   = NULL;
    st_setting->parent = st_first->parent;
    if (st_setting->parent != NULL) {
        st_setting->parent->data.st_child = st_setting;
    }
    return st_setting;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append st_setting item to st_list list.
 */
Setting *
settings_append (Setting *st_list,
                 Setting *st_setting)
{
    Setting *st_last      = NULL;
    Setting *st_add_first = NULL;

    if (st_list == NULL)
        return st_setting;

    if (st_setting == NULL)
        return st_list;

    st_last      = setting_last (st_list);
    st_add_first = setting_first (st_setting);

    st_last->next        = st_add_first;
    st_add_first->prev   = st_last;
    st_add_first->parent = st_last->parent;

    return setting_first (st_list);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace Setting st_old in list with st_new.
 */
Setting *
setting_replace (Setting *st_old,
                 Setting *st_new)
{
    if (st_old == NULL || st_new == NULL)
        return NULL;

    if (st_old->prev != NULL)
        st_old->prev->next = st_new;
    if (st_old->next != NULL)
        st_old->next->prev = st_new;

    st_new->prev   = st_old->prev;
    st_new->next   = st_old->next;
    st_new->parent = st_old->parent;
    st_old->prev   = NULL;
    st_old->next   = NULL;
    settings_free_all (st_old);

    return setting_first (st_new);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append Setting item to st_list list or ignore append if
 *         setting with same name exists on it.
 */
void
settings_append_or_ignore (Setting *st_list,
                           Setting *st_setting)
{
    Setting *st_item = NULL;

    if (st_setting == NULL)
        return;

    st_item = st_list;

    while (st_item != NULL) {
        if (setting_get_hash (st_item) == setting_get_hash (st_setting)) {
            return;
        }
        st_item = st_item->next;
    }
    settings_append (st_list, st_setting);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append Setting item to st_list list and replace if
 *         setting with same name exists on it.
 */
Setting *
settings_append_or_replace (Setting *st_list,
                            Setting *st_setting)
{
    Setting *st_ret  = NULL;
    Setting *st_item = NULL;

    if (st_setting == NULL)
        return NULL;

    st_ret  = st_list;
    st_item = st_list;

    while (st_item != NULL) {
        if (setting_get_hash (st_item) == setting_get_hash (st_setting)) {
            setting_replace (st_item, st_setting);
            if (st_setting->prev == NULL)
                st_ret = st_setting;
            return st_ret;
        }
        st_item = st_item->next;
    }
    settings_append (st_list, st_setting);
    return st_ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find setting in list and replace.
 */
Setting *
settings_find_replace (Setting   *st_list,
                       Setting   *st_setting,
                       const int  i_multi)
{
    Setting *st_ret  = NULL;
    Setting *st_item = NULL;
    Setting *st_next = NULL;
    int      i_cnt   = 0;

    if (st_setting == NULL)
        return 0;

    st_ret  = st_list;
    st_item = st_list;

    while (st_item != NULL) {
        st_next = st_item->next;
        if (setting_get_hash (st_item) == setting_get_hash (st_setting)) {
            st_ret = setting_replace (st_item, st_setting);
            ++i_cnt;
            if (!i_multi) break;
        }
        st_item = st_next;
    }
    return st_ret;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Set st_child Setting to be a child of st_parent.
 */
static void
setting_set_child (Setting *st_parent,
                   Setting *st_child)
{
    st_parent->data.st_child = st_child;

    if (st_child != NULL) {
        st_child->parent = st_parent;
    }
}
/*----------------------------------------------------------------------------*/
Setting * setting_get_child  (const Setting *st_setting)
{
    if (setting_get_type (st_setting) == SET_VAL_SETTING ||
        setting_get_type (st_setting) == SET_VAL_ARRAY) {

        return st_setting->data.st_child;
    }
    return NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append Setting st_child to child list of Setting st_parent
 */
void
setting_add_child (Setting *st_parent,
                   Setting *st_child)
{
    Setting *st_temp = NULL;

    if (st_child == NULL)
        return;

    if (setting_get_type (st_parent) == SET_VAL_SETTING ||
        setting_get_type (st_parent) == SET_VAL_ARRAY) {

        if ((st_temp = setting_get_child (st_parent)) == NULL)
            setting_set_child (st_parent, st_child);
        else
            settings_append (st_temp, st_child);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get top parent Setting of given Setting item.
 */
Setting *
setting_get_top_parent (Setting *st_setting)
{
    Setting *st_top = NULL;

    if (st_setting == NULL)
        return NULL;

    st_top = st_setting;

    while (st_top->parent != NULL)
        st_top = st_top->parent;

    return st_top;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find Setting with given name in list starting at position of input
 *         Setting item.
 */
Setting *
settings_find (Setting    *st_settings,
               const char *s_name)
{
    Setting       *st_item = NULL;
    uint_fast32_t  ui_hash = 0;

    if (st_settings == NULL)
        return NULL;

    if (s_name != NULL)
        ui_hash = hash (s_name);

    st_item = st_settings;

    while (st_item != NULL) {

        if (ui_hash == setting_get_hash (st_item)) {
            return st_item;
        }
        st_item = st_item->next;
    }
    return NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count number of Setting items present in given st_settings list.
 */
size_t
settings_count (const Setting *st_settings)
{
    const Setting *st_item = NULL;
    size_t         ui_cnt  = 0;

    st_item = st_settings;

    while (st_item != NULL) {
        ++ui_cnt;
        st_item = st_item->next;
    }
    return ui_cnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find and return Setting item at given posision in st_settings
 *         list.
 */
Setting *
setting_get_at_pos (Setting      *st_settings,
                    const size_t  pos)
{
    Setting *st_item = NULL;
    size_t   ui_cnt  = 0;

    st_item = st_settings;

    while (st_item != NULL) {
        if (ui_cnt++ == pos)
            return st_item;
        st_item = st_item->next;
    }
    return NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove Setting from list.
 */
Setting *
setting_remove (Setting *st_setting)
{
    Setting *st_item = NULL;
    Setting *st_ret  = NULL;

    if (st_setting == NULL)
        return NULL;

    if (st_setting->prev != NULL) {
        st_setting->prev->next = st_setting->next;
        st_ret = setting_first (st_setting->prev);
    }
    else
        st_ret = st_setting->next;

    if (st_setting->next != NULL) {
        st_setting->next->prev = st_setting->prev;
    }
    if (st_setting->parent != NULL) {

        st_item = st_setting->prev == NULL ?
                  st_setting->next :
                  setting_first (st_setting);

        setting_set_child (st_setting->parent, st_item);
    }
    if ((st_item = setting_get_child (st_setting)) != NULL) {
        settings_free_all (st_item);
    }
    setting_free (st_setting);
    return st_ret;
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
/*----------------------------------------------------------------------------*/
/*
 * @brief  Print setting info
 */
void
setting_print (const Setting *st_set)
{
    switch (st_set->v_type) {
        case SET_VAL_INT:
            printf ("int, val=%ld, n=%s,",
                    setting_get_int (st_set),
                    setting_get_name (st_set));
            if (st_set->parent != NULL)
                printf (" par=%s\n", st_set->parent->s_name);
            else
                printf ("\n");
            break;
        case SET_VAL_UINT:
            printf ("uint, val=%ld, n=%s\n",
                    setting_get_uint (st_set),
                    setting_get_name (st_set));
            break;
        case SET_VAL_DOUBLE:
            printf ("double, val=%f, n=%s\n",
                    setting_get_double (st_set),
                    setting_get_name (st_set));
            break;
        case SET_VAL_STRING:
            printf ("str, val=%s, n=%s",
                    setting_get_string (st_set),
                    setting_get_name (st_set));
            if (st_set->parent != NULL)
                printf (" par=%s\n", st_set->parent->s_name);
            else
                printf ("\n");
            break;
        case SET_VAL_SETTING:
            printf ("set, val=%s, n=%s",
                    setting_get_string (st_set),
                    setting_get_name (st_set));
            if (st_set->parent != NULL)
                printf (" par=%s\n", st_set->parent->s_name);
            else
                printf ("\n");
            if (setting_get_child (st_set) != NULL) {
                printf ("[\n");
                settings_print (setting_get_child (st_set));
                printf ("]\n");
            }
            break;
        case SET_VAL_ARRAY:
            printf ("array, val=%s, n=%s",
                    setting_get_string (st_set),
                    setting_get_name (st_set));
            if (st_set->parent != NULL)
                printf (" par=%s\n", st_set->parent->s_name);
            else
                printf ("\n");
            printf ("[\n");
            if (setting_get_child (st_set) != NULL)
                settings_print (setting_get_child (st_set));
            printf ("]\n");
            break;
        case SET_VAL_NULL:
            break;
        default:
            break;
    }
}
/*----------------------------------------------------------------------------*/
void
settings_print (const Setting *st_set)
{
    while (st_set != NULL) {
        setting_print (st_set);
        st_set = st_set->next;
    }
}
#endif
/*----------------------------------------------------------------------------*/

