/**
 * @file  setting_item.c
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
 * @brief  Setting structure and functions
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifdef DEBUG
#include <stdio.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include "hash_djb2.h"
#include "setting_item.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting initialization.
 *
 * @param[out] setting  Setting object
 * @return     none
 */
static void setting_init (Setting *setting);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create default Setting.
 *
 * @param[in] name  Setting's name
 * @return    New Setting item
 */
static Setting * setting_new_default (const char *name)
       __attribute__ ((returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @brief Set child Setting to be a child of parent.
 *
 * @param[out] parent  Parent Setting to set child to
 * @param[out] child   Child setting to be assigned to parent Setting
 * @return     none
 */
static void setting_set_child (Setting *parent,
                               Setting *child);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting type value
 *
 * @param[in] setting  Setting item
 * @param[in] val      Type value to set
 * @return    none
 */
static inline void
setting_set_type (Setting          *setting,
                  const SetValType  val)
{
    setting->v_type = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting name string
 *
 * @param[in] setting  Setting item
 * @param[in] val      Name string to set
 * @return    none
 */
static inline void
setting_set_name (Setting    *setting,
                  const char *val)
{
    setting->s_name = strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set Setting hash value
 *
 * @param[in] setting  Setting item
 * @param[in] val      Hash value to set
 * @return    none
 */
static inline void
setting_set_hash (Setting       *setting,
                  uint_fast32_t  val)
{
    setting->hash = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free string data in Setting
 */
static inline void
setting_free_string (Setting *setting)
{
    free (setting->data.s_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Setting initialization
 */
static void
setting_init (Setting *setting)
{
    setting->prev       = nullptr;
    setting->next       = nullptr;
    setting->parent     = nullptr;
    setting->v_type     = SET_VAL_INT;
    setting->s_name     = nullptr;
    setting->hash       = 0;
    setting->data.i_val = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free Setting data
 */
void
setting_free (Setting *setting)
{
    if (setting == nullptr)
        return;

    free (setting->s_name);

    if (setting_get_type (setting) == SET_VAL_STRING) {
        setting_free_string (setting);
    }
    free (setting);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free all settings in given Setting
 */
void
settings_free_all (Setting *setting)
{
    Setting *next    = nullptr;
    Setting *current = nullptr;
    Setting *child   = nullptr;

    current = setting;

    while (current != nullptr) {
        next = current->next;

        if (setting_get_type (current) == SET_VAL_SETTING ||
            setting_get_type (current) == SET_VAL_ARRAY) {

            if ((child = setting_get_child (current)) != nullptr)
                settings_free_all (child);
        }
        setting_free (current);

        current = next;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set setting to string type and its content to val.
 */
void
setting_set_string (Setting    *setting,
                    const char *val)
{
    if (setting_get_type (setting) == SET_VAL_STRING)
        setting_free_string (setting);

    setting_set_type (setting, SET_VAL_STRING);

    setting->data.s_val = (val == nullptr) ? strdup ("") : strdup (val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create default Setting.
 */
static Setting *
setting_new_default (const char *name)
{
    Setting *result = nullptr;

    if ((result = malloc (sizeof (Setting))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    setting_init (result);

    /*if (name != NULL && name[0] != '\0') {*/
    if (name != nullptr) {
        setting_set_name (result, name);
        setting_set_hash (result, hash (name));
    }
    return result;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_setting (const char *name)
{
    Setting *result = nullptr;

    result = setting_new_default (name);

    setting_set_type (result, SET_VAL_SETTING);
    result->data.st_child = nullptr;

    return result;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_array (const char *name)
{
    Setting *result = nullptr;

    result = setting_new_default (name);

    setting_set_type (result, SET_VAL_ARRAY);
    result->data.st_child = nullptr;

    return result;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_int (const char    *name,
                 const int64_t  val)
{
    Setting *setting = nullptr;

    setting = setting_new_default (name);

    setting_set_type (setting, SET_VAL_INT);
    setting->data.i_val = val;

    return setting;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_uint (const char     *name,
                  const uint64_t  val)
{
    Setting *setting = nullptr;

    setting = setting_new_default (name);

    setting_set_type (setting, SET_VAL_UINT);
    setting->data.ui_val = val;

    return setting;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_double (const char   *name,
                    const double  val)
{
    Setting *setting = nullptr;

    setting = setting_new_default (name);

    setting_set_type (setting, SET_VAL_DOUBLE);
    setting->data.d_val = val;

    return setting;
}
/*----------------------------------------------------------------------------*/
Setting *
setting_new_string (const char *name,
                    const char *val)
{
    Setting *setting = nullptr;

    setting = setting_new_default (name);

    setting_set_string (setting, val);

    return setting;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find and return first item in a Setting list.
 */
Setting *
setting_first (Setting *settings)
{
    Setting *first = nullptr;

    if (settings == nullptr)
        return nullptr;

    first = settings;

    while (first->prev != nullptr)
        first = first->prev;

    return first;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find and return last item in a Setting list.
 */
Setting *
setting_last (Setting *settings)
{
    Setting *last = nullptr;

    if (settings == nullptr)
        return nullptr;

    last = settings;

    while (last->next != nullptr)
        last = last->next;

    return last;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Prepend setting item to the list.
 */
Setting *
settings_prepend (Setting *list,
                  Setting *setting)
{
    Setting *first    = nullptr;
    Setting *add_last = nullptr;

    if (list == nullptr)
        return setting;

    if (setting == nullptr)
        return list;

    add_last = setting_last (setting);
    first    = setting_first (list);

    first->prev     = add_last;
    add_last->next  = first;
    setting->prev   = nullptr;
    setting->parent = first->parent;
    if (setting->parent != nullptr) {
        setting->parent->data.st_child = setting;
    }
    return setting;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append setting item to the list.
 */
Setting *
settings_append (Setting *list,
                 Setting *setting)
{
    Setting *last      = nullptr;
    Setting *add_first = nullptr;

    if (list == nullptr)
        return setting;

    if (setting == nullptr)
        return list;

    last      = setting_last (list);
    add_first = setting_first (setting);

    last->next        = add_first;
    add_first->prev   = last;
    add_first->parent = last->parent;

    return setting_first (list);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace Setting old_item in list with new_item.
 */
Setting *
setting_replace (Setting *old_item,
                 Setting *new_item)
{
    if (old_item == nullptr || new_item == nullptr)
        return nullptr;

    if (old_item->prev != nullptr)
        old_item->prev->next = new_item;
    if (old_item->next != nullptr)
        old_item->next->prev = new_item;

    new_item->prev   = old_item->prev;
    new_item->next   = old_item->next;
    new_item->parent = old_item->parent;
    old_item->prev   = nullptr;
    old_item->next   = nullptr;
    settings_free_all (old_item);

    return setting_first (new_item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append Setting item to the list or ignore append if
 *         setting with same name exists on it.
 */
void
settings_append_or_ignore (Setting *list,
                           Setting *setting)
{
    Setting *item = nullptr;

    if (setting == nullptr)
        return;

    item = list;

    while (item != nullptr) {
        if (setting_get_hash (item) == setting_get_hash (setting)) {
            return;
        }
        item = item->next;
    }
    settings_append (list, setting);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append Setting item to the list and replace if
 *         setting with same name exists on it.
 */
Setting *
settings_append_or_replace (Setting *list,
                            Setting *setting)
{
    Setting *result = nullptr;
    Setting *item   = nullptr;

    if (setting == nullptr)
        return nullptr;

    result = list;
    item   = list;

    while (item != nullptr) {
        if (setting_get_hash (item) == setting_get_hash (setting)) {
            setting_replace (item, setting);
            if (setting->prev == nullptr)
                result = setting;
            return result;
        }
        item = item->next;
    }
    settings_append (list, setting);
    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find setting in list and replace.
 */
Setting *
settings_find_replace (Setting *list,
                       Setting *setting)
{
    Setting *result = nullptr;
    Setting *item   = nullptr;

    if (setting == nullptr)
        return nullptr;

    result = list;
    item   = list;

    while (item != nullptr) {
        if (setting_get_hash (item) == setting_get_hash (setting)) {
            result = setting_replace (item, setting);
            break;
        }
        item = item->next;
    }
    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Set child Setting to be a child of parent.
 */
static void
setting_set_child (Setting *parent,
                   Setting *child)
{
    parent->data.st_child = child;

    if (child != nullptr) {
        child->parent = parent;
    }
}
/*----------------------------------------------------------------------------*/
Setting *
setting_get_child (const Setting *setting)
{
    if (setting_get_type (setting) == SET_VAL_SETTING ||
        setting_get_type (setting) == SET_VAL_ARRAY) {

        return setting->data.st_child;
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append Setting child to child list of Setting parent
 */
void
setting_add_child (Setting *parent,
                   Setting *child)
{
    Setting *children = nullptr;

    if (parent == nullptr || child == nullptr)
        return;

    if (setting_get_type (parent) == SET_VAL_SETTING ||
        setting_get_type (parent) == SET_VAL_ARRAY) {

        if ((children = setting_get_child (parent)) == nullptr)
            setting_set_child (parent, child);
        else
            settings_append (children, child);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get top parent Setting of given Setting item.
 */
Setting *
setting_get_top_parent (Setting *setting)
{
    Setting *top = nullptr; /* Top setting to return */

    if (setting == nullptr)
        return nullptr;

    top = setting;

    while (top->parent != nullptr)
        top = top->parent;

    return top;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find Setting with given name in list starting at position of input
 *         Setting item.
 */
Setting *
settings_find (Setting    *settings,
               const char *name)
{
    Setting       *item      = nullptr; /* Setting item to return */
    uint_fast32_t  name_hash = 0;    /* Hash of name to compare */

    if (settings == nullptr)
        return nullptr;

    if (name != nullptr)
        name_hash = hash (name);

    item = settings;

    while (item != nullptr) {

        if (name_hash == setting_get_hash (item)) {
            return item;
        }
        item = item->next;
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Count number of Setting items present in given settings list.
 */
size_t
settings_count (const Setting *settings)
{
    size_t count = 0; /* Settings count to return */

    while (settings != nullptr) {
        ++count;
        settings = settings->next;
    }
    return count;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find and return Setting item at given posision in settings
 *         list.
 */
Setting *
setting_get_at_pos (Setting      *settings,
                    const size_t  pos)
{
    Setting *item  = nullptr; /* Setting item to return */
    size_t   count = 0;    /* Number of setting in iteration */

    item = settings;

    while (item != nullptr) {
        if (count++ == pos)
            return item;
        item = item->next;
    }
    return nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove Setting from list.
 */
Setting *
setting_remove (Setting *setting)
{
    Setting *item   = nullptr;
    Setting *result = nullptr;

    if (setting == nullptr)
        return nullptr;

    if (setting->prev != nullptr) {
        setting->prev->next = setting->next;
        result = setting_first (setting->prev);
    }
    else
        result = setting->next;

    if (setting->next != nullptr) {
        setting->next->prev = setting->prev;
    }
    if (setting->parent != nullptr) {

        item = setting->prev == nullptr ?
                  setting->next :
                  setting_first (setting);

        setting_set_child (setting->parent, item);
    }
    if ((item = setting_get_child (setting)) != nullptr) {
        settings_free_all (item);
    }
    setting_free (setting);
    return result;
}
/*----------------------------------------------------------------------------*/
#ifdef DEBUG
/*----------------------------------------------------------------------------*/
/*
 * @brief  Print setting info
 */
void
setting_print (const Setting *setting)
{
    switch (setting->v_type) {
        case SET_VAL_INT:
            printf ("int, val=%ld, n=%s,",
                    setting_get_int (setting),
                    setting_get_name (setting));
            if (setting->parent != nullptr)
                printf (" par=%s\n", setting->parent->s_name);
            else
                printf ("\n");
            break;
        case SET_VAL_UINT:
            printf ("uint, val=%ld, n=%s\n",
                    setting_get_uint (setting),
                    setting_get_name (setting));
            break;
        case SET_VAL_DOUBLE:
            printf ("double, val=%f, n=%s\n",
                    setting_get_double (setting),
                    setting_get_name (setting));
            break;
        case SET_VAL_STRING:
            printf ("str, val=%s, n=%s",
                    setting_get_string (setting),
                    setting_get_name (setting));
            if (setting->parent != nullptr)
                printf (" par=%s\n", setting->parent->s_name);
            else
                printf ("\n");
            break;
        case SET_VAL_SETTING:
            printf ("set, val=%s, n=%s",
                    setting_get_string (setting),
                    setting_get_name (setting));
            if (setting->parent != nullptr)
                printf (" par=%s\n", setting->parent->s_name);
            else
                printf ("\n");
            if (setting_get_child (setting) != nullptr) {
                printf ("[\n");
                settings_print (setting_get_child (setting));
                printf ("]\n");
            }
            break;
        case SET_VAL_ARRAY:
            printf ("array, val=%s, n=%s",
                    setting_get_string (setting),
                    setting_get_name (setting));
            if (setting->parent != nullptr)
                printf (" par=%s\n", setting->parent->s_name);
            else
                printf ("\n");
            printf ("[\n");
            if (setting_get_child (setting) != nullptr)
                settings_print (setting_get_child (setting));
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
settings_print (const Setting *setting)
{
    while (setting != nullptr) {
        setting_print (setting);
        setting = setting->next;
    }
}
#endif
/*----------------------------------------------------------------------------*/

