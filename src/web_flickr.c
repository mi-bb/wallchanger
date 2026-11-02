/**
 * @file  web_flickr.c
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
 * @brief  Settings for searching the Flickr website.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include "config.h"

#ifdef HAVE_FLICKCURL
#include <gtk/gtk.h>
#include <flickcurl.h>
#include <err.h>

#include "string_functions.h"
#include "errors.h"
#include "message_dialogs.h"
#include "cache_query.h"
#include "search_item.h"
#include "webwidget_common.h"
#include "web_flickr.h"
/*----------------------------------------------------------------------------*/
/**
 * @def   SERV_NAME
 * @brief Service name
 */
#define SERV_NAME "Flickr" 
/*----------------------------------------------------------------------------*/
/**
 * @brief  Indexes of widgets in supporting array.
 */
enum e_flickr_array {
    FLA_CLI_KEY, /**< Index of entry for client key */
    FLA_CLI_SEC, /**< Index of entry for client secret */
    FLA_REQ_TOK, /**< Index of entry for request token */
    FLA_REQ_SEC, /**< Index of entry for request token secret */
    FLA_ATH_TOK, /**< Index of entry for access token */
    FLA_ATH_SEC, /**< Index of entry for access token secret */
    FLA_AUT_LNK, /**< Index of link button for authorization link */
    FLA_VER_CD,  /**< Index of entry for verify code */
    FLA_CNT      /**< Number of items in array */
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  Generate authorization link based on client API keys and request
 *         tokens.
 *
 * @param[in,out] widgets  Array with widgets
 * @return        none
 */
static void
event_flickr_auth_link_generate (GtkWidget **widgets)
{
    const char *cli_key = nullptr; /* Client key */
    const char *cli_sec = nullptr; /* Client secret */
    const char *req_tok = nullptr; /* Request token */
    const char *req_sec = nullptr; /* Request token secret */
    char       *uri     = nullptr; /* Authorization url */
    flickcurl  *fc;
    [[maybe_unused]] int rc = 0;

    cli_key = gtk_entry_get_text (GTK_ENTRY (widgets[FLA_CLI_KEY]));
    cli_sec = gtk_entry_get_text (GTK_ENTRY (widgets[FLA_CLI_SEC]));

    if (str_is_empty_warn (cli_key, "Empty client key"))
        return;
    else if (str_is_empty_warn (cli_sec, "Empty client secret"))
        return;

    flickcurl_init ();

    fc = flickcurl_new ();

    flickcurl_set_oauth_client_key (fc, cli_key);
    flickcurl_set_oauth_client_secret (fc, cli_sec);

    rc = flickcurl_oauth_create_request_token (fc, nullptr);

    req_tok = flickcurl_get_oauth_request_token (fc);
    req_sec = flickcurl_get_oauth_request_token_secret (fc);

    uri = flickcurl_oauth_get_authorize_uri (fc);

    gtk_entry_set_text (GTK_ENTRY (widgets[FLA_REQ_TOK]), req_tok);
    gtk_entry_set_text (GTK_ENTRY (widgets[FLA_REQ_SEC]), req_sec);
    gtk_link_button_set_uri (GTK_LINK_BUTTON (widgets[FLA_AUT_LNK]), uri);
    gtk_button_set_label (GTK_BUTTON (widgets[FLA_AUT_LNK]), uri);

    free (uri);
    flickcurl_free (fc);
    flickcurl_finish ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Generate access keys based on authorization key.
 *
 * @param[in,out] widgets  Array with widgets
 * @return        none
 */
static void
event_flickr_access_keys_generate (GtkWidget **widgets)
{
    const char *cli_key  = nullptr; /* Client key */
    const char *cli_sec  = nullptr; /* Client secret */
    const char *req_tok  = nullptr; /* Request token */
    const char *req_sec  = nullptr; /* Request token secret */
    const char *ath_tok  = nullptr; /* Access token */
    const char *ath_sec  = nullptr; /* Access token secret */
    const char *verifier = nullptr; /* Verify code */
    flickcurl  *fc;
    [[maybe_unused]] int rc = 0;

    cli_key  = gtk_entry_get_text (GTK_ENTRY (widgets[FLA_CLI_KEY]));
    cli_sec  = gtk_entry_get_text (GTK_ENTRY (widgets[FLA_CLI_SEC]));
    req_tok  = gtk_entry_get_text (GTK_ENTRY (widgets[FLA_REQ_TOK]));
    req_sec  = gtk_entry_get_text (GTK_ENTRY (widgets[FLA_REQ_SEC]));
    verifier = gtk_entry_get_text (GTK_ENTRY (widgets[FLA_VER_CD]));

    if      (str_is_empty_warn (cli_key,  "Empty client key"))
        return;
    else if (str_is_empty_warn (cli_sec,  "Empty client secret"))
        return;
    else if (str_is_empty_warn (req_tok,  "Empty request token"))
        return;
    else if (str_is_empty_warn (req_sec,  "Empty request token secret"))
        return;
    else if (str_is_empty_warn (verifier, "Empty verify code"))
        return;

    flickcurl_init ();

    fc = flickcurl_new ();

    flickcurl_set_oauth_client_key (fc, cli_key);
    flickcurl_set_oauth_client_secret (fc, cli_sec);
    flickcurl_set_oauth_request_token (fc, req_tok);
    flickcurl_set_oauth_request_token_secret (fc, req_sec);

    rc = flickcurl_oauth_create_access_token (fc, verifier);

    ath_tok = flickcurl_get_oauth_token (fc);
    ath_sec = flickcurl_get_oauth_token_secret (fc);

    gtk_entry_set_text (GTK_ENTRY (widgets[FLA_ATH_TOK]), ath_tok);
    gtk_entry_set_text (GTK_ENTRY (widgets[FLA_ATH_SEC]), ath_sec);

    flickcurl_free (fc);
    flickcurl_finish ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create file name using image title (display name) and url.
 *
 * @param[in]  disp_name  Display name (image title)
 * @param[in]  image_url  Image url (used for getting extension)
 * @param[in]  image_id   Image id
 * @return     New string with name for saving file. After use it should be
 *             freed using free.
 */
static char *
flickr_create_file_name (const char *disp_name,
                         const char *image_url,
                         const char *image_id)
{
    const char *ext   = nullptr; /* Pointer to extension */
    const char *end   = nullptr; /* Pointer to end of valid string */
    char       *tmp   = nullptr; /* Temporary string */
    char       *first = nullptr; /* Pointer to first char of temp string */
    size_t      inc   = 0;       /* Increment step for char write */
    size_t      slen  = 0;       /* Length of source display name */
    size_t      elen  = 0;       /* Length of extension string */
    size_t      ilen  = 0;       /* Length of id string */
    gunichar    u_c;             /* Uni char to examine */

    slen = strlen (disp_name);
    ilen = strlen (image_id);

    /* Get extension pointer and length */
    ext = strrchr (image_url, '.');
    if (ext != nullptr)
        elen = strlen (ext);

    /* Alloc space for name, 4 times more because of unicode grow possibility */
    tmp = malloc ((4 * slen + ilen + elen + 2) * sizeof (char));
    memset (tmp, '\0', 4 * slen + ilen + elen + 2);

    first = tmp;

    /* Examine name char by char */
    while (*disp_name != '\0') {
        inc = 0;
        u_c = g_utf8_get_char (disp_name);
        /* Copy only alphanumeric chars, convert to lowercase if needed,
         * convert spaces to underscores */
        if (g_unichar_isalnum (u_c)) {
            if (g_unichar_isupper (u_c))
                u_c = g_unichar_tolower (u_c);
            inc = (size_t) g_unichar_to_utf8 (u_c, tmp);
        }
        else if (g_unichar_isspace (u_c))
            inc = (size_t) g_unichar_to_utf8 ('_', tmp);

        tmp += inc;
        disp_name = g_utf8_find_next_char (disp_name, nullptr);
    }
    slen = strlen (first);

    /* Check name length if it is smaller than 255 bytes and shrink if needed */
    if (slen + elen + ilen + 1 >= 255) {
        slen = 255 - elen - ilen - 1;
    }
    g_utf8_validate (first, (gssize) slen, &end);
    first[end - first] = '-';
    memcpy (first + (end - first) + 1, image_id, ilen);
    memcpy (first + (end - first) + ilen + 1, ext, elen + 1);

    return first;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Error handler for Flickr.
 */
static void
my_message_handler([[maybe_unused]] void *user_data,
                   const char *message)
{
    warnx ("Flicr error : %s\n", message);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Flickr flickcurl_photo info to SearchItem item.
 *
 * @param[in] photo  flickcurl_photo object to convert
 * @return    SearchItem item
 */
static SearchItem *
flickrphoto_to_search_item (flickcurl_photo *photo)
{
    SearchItem *item  = nullptr;
    char       *name  = nullptr;
    const char *title = nullptr;

    item = search_item_new ();
    title = photo->fields[PHOTO_FIELD_title].string;

    if (photo->fields[PHOTO_FIELD_owner_realname].string != nullptr) {
        search_item_set_author_name (
                item, photo->fields[PHOTO_FIELD_owner_realname].string);
    }
    else if (photo->fields[PHOTO_FIELD_owner_username].string != nullptr) {
        search_item_set_author_name (
                item, photo->fields[PHOTO_FIELD_owner_username].string);
    }

    name = str_is_empty (title) ? strdup (photo->id) : strdup (title);
    remove_non_alpha_space (name);

    search_item_set_service_name (item, ww_name (WEB_SERV_FLICKR));
    search_item_set_display_name (item, name);
    search_item_set_display_markup (item, name);
    free (name);

    search_item_set_id_string (item, photo->id);

    item->thumb_url = flickcurl_photo_as_source_uri (photo, 'm');
    item->page_url  = flickcurl_photo_as_page_uri (photo);

    if (photo->fields[PHOTO_FIELD_originalformat].string != nullptr)
        item->image_url = flickcurl_photo_as_source_uri (photo, 'o');
    else
        item->image_url = flickcurl_photo_as_source_uri (photo, 'b');

    item->file_name = flickr_create_file_name (item->display_name,
                                                    item->image_url,
                                                    item->id);
#ifdef DEBUG
    printf ("ID : %s\n", photo->id);
    printf ("Title : %s\n", photo->fields[PHOTO_FIELD_title].string);
    printf ("author1 : %s\n",
            photo->fields[PHOTO_FIELD_owner_realname].string);
    printf ("author2 : %s\n",
            photo->fields[PHOTO_FIELD_owner_username].string);
    printf ("Author : %s\n",item->author_name);
    printf ("Page url : %s\n",item->page_url);
    printf ("Thumb url : %s\n",item->thumb_url);
    printf ("Image url : %s\n",item->image_url);
    printf ("File name : %s\n",item->file_name);
    flickcurl_photo_field_type field_type;
    for (field_type = 0; field_type <= PHOTO_FIELD_LAST; field_type++) {
        flickcurl_field_value_type datatype = photo->fields[field_type].type;

        if (datatype != VALUE_TYPE_NONE)
            printf("field %s (%d) with %s value: '%s' / %d\n", 
                   flickcurl_get_photo_field_label(field_type), (int)field_type,
                   flickcurl_get_field_value_type_label(datatype),
                   photo->fields[field_type].string,
                   photo->fields[field_type].integer);
    }

    for(int i = 0; i < photo->tags_count; i++) {
        flickcurl_tag* tag=photo->tags[i];
        printf ("%d) %s tag: id %s author ID %s name %s raw '%s' "
               "cooked '%s' count %d\n",
               i, (tag->machine_tag ? "machine" : "regular"),
               tag->id, tag->author, 
               (tag->authorname ? tag->authorname : "(Unknown)"), 
               tag->raw, tag->cooked,
               tag->count);
    }
#endif
    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search in Flickr database.
 */
void
flickr_search (WebWidget      *widget,
               const NStrings *strings)
{
    flickcurl_photos_list_params list_params;
    flickcurl_search_params      params;
    flickcurl_photos_list       *photos_list = nullptr;
    flickcurl   *fc    = nullptr;
    CacheQuery  *cache = nullptr; /* For cache saving */
    SearchItem  *item  = nullptr; /* Photo information */
    char        *tags  = nullptr; /* Query tags */
    int          err   = 0;       /* Error output */
    int          i     = 0;       /* i */
    char         sort[64];        /* Sorting information */
    char         media[16];       /* Media type to search */

    flickcurl_init ();
    fc = flickcurl_new ();

    flickcurl_set_error_handler (fc, my_message_handler, nullptr);

    flickcurl_set_oauth_client_key    (fc, strings->str[0]);
    flickcurl_set_oauth_client_secret (fc, strings->str[1]);
    flickcurl_set_oauth_token         (fc, strings->str[2]);
    flickcurl_set_oauth_token_secret  (fc, strings->str[3]);

    strcpy (sort,  "interestingness-desc");
    strcpy (media, "photos");
    tags = str_replace_in (widget->query, " ", ",");

    flickcurl_search_params_init (&params);
    params.sort  = sort;
    params.tags  = tags;
    params.media = media;

    flickcurl_photos_list_params_init (&list_params);
    list_params.per_page = 12;
    list_params.page     = widget->page;
    list_params.extras   = "original_format,o_dims,url_o,url_t";

    photos_list = flickcurl_photos_search_params (fc, &params, &list_params);

    gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                    GTK_ICON_VIEW (widget->img_view))));

    if (photos_list != nullptr) {
        cache = cache_query_new (ww_name (WEB_SERV_FLICKR),
                                    widget->query,
                                    widget->search_opts,
                                    widget->page);

        widget->found_cnt = photos_list->total_count;
        cache->found_cnt  = widget->found_cnt;

        for(i = 0; i < photos_list->photos_count; ++i) {
            item = flickrphoto_to_search_item (photos_list->photos[i]);
            add_search_item_to_img_view (widget->img_view,
                                         item,
                                         widget->wallpaper_dir,
                                         ww_name (WEB_SERV_FLICKR),
                                         widget->thumb_quality);
            cache_query_append_item (cache, item);
        }
        err = cache_query_save (cache);

        if (err != ERR_OK) {
            message_dialog_error (nullptr, err_get_message (err));
        }
        flickcurl_free_photos_list (photos_list);
        cache_query_free (cache);
    }
    free (tags);
    flickcurl_free (fc);
    flickcurl_finish ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with Flickr service settings.
 */
int
flickr_settings_dialog (NStrings *strings)
{
    GtkWidget *dialog;         /* Flickr settings dialog */
    GtkWidget *content_box;    /* Dialog's box */
    GtkWidget *cli_key_entry;  /* Entry for API key */
    GtkWidget *cli_sec_entry;  /* Entry for API secret */
    GtkWidget *verify_entry;   /* Entry for verifier */
    GtkWidget *auth_button;    /* Button to authorize app */
    GtkWidget *auth_link;      /* Authorization link */
    GtkWidget *gen_acc_button; /* Button to authorize app */
    GtkWidget *req_tok_entry;  /* Entry for API request token */
    GtkWidget *req_sec_entry;  /* Entry for API request secret */
    GtkWidget *ath_tok_entry;  /* Entry for API access key */
    GtkWidget *ath_sec_entry;  /* Entry for API access secret */
    GtkWidget *client_grid;    /* Grid for client API keys */
    GtkWidget *request_grid;   /* Grid for request keys */
    GtkWidget *verify_grid;    /* Grid for verify code */
    GtkWidget *access_grid;    /* Grid for access keys */
    GtkWidget *widgets[FLA_CNT];   /* Widgets for events */
    const char *cli_key = nullptr; /* Client key */
    const char *cli_sec = nullptr; /* Client secret */
    const char *ath_tok = nullptr; /* Access token */
    const char *ath_sec = nullptr; /* Access token secret */
    int         res     = 0;       /* Dialog result */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    dialog = gtk_dialog_new_with_buttons ("Flickr configuration",
                                             nullptr,
                                             flags,
                                             "_OK",
                                             GTK_RESPONSE_ACCEPT,
                                             "_Cancel",
                                             GTK_RESPONSE_REJECT,
                                             nullptr);

    content_box = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    gtk_container_set_border_width (GTK_CONTAINER (content_box), 8);

    client_grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (client_grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (client_grid), 8);
    gtk_widget_set_halign (client_grid, GTK_ALIGN_CENTER);

    request_grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (request_grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (request_grid), 8);
    gtk_widget_set_halign (request_grid, GTK_ALIGN_CENTER);

    verify_grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (verify_grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (verify_grid), 8);
    gtk_widget_set_halign (verify_grid, GTK_ALIGN_CENTER);

    access_grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (access_grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (access_grid), 8);
    gtk_widget_set_halign (access_grid, GTK_ALIGN_CENTER);

    cli_key_entry = gtk_entry_new ();
    cli_sec_entry = gtk_entry_new ();
    verify_entry  = gtk_entry_new ();
    req_tok_entry = gtk_entry_new ();
    req_sec_entry = gtk_entry_new ();
    ath_tok_entry = gtk_entry_new ();
    ath_sec_entry = gtk_entry_new ();

    gtk_entry_set_width_chars (GTK_ENTRY (cli_key_entry), 35);
    gtk_entry_set_width_chars (GTK_ENTRY (cli_sec_entry), 35);
    gtk_entry_set_width_chars (GTK_ENTRY (req_tok_entry), 35);
    gtk_entry_set_width_chars (GTK_ENTRY (req_sec_entry), 35);
    gtk_entry_set_width_chars (GTK_ENTRY (ath_tok_entry), 35);
    gtk_entry_set_width_chars (GTK_ENTRY (ath_sec_entry), 35);

    gtk_entry_set_text (GTK_ENTRY (cli_key_entry), strings->str[0]);
    gtk_entry_set_text (GTK_ENTRY (cli_sec_entry), strings->str[1]);
    gtk_entry_set_text (GTK_ENTRY (ath_tok_entry), strings->str[2]);
    gtk_entry_set_text (GTK_ENTRY (ath_sec_entry), strings->str[3]);

    gtk_grid_attach (GTK_GRID (client_grid),
                     gtk_label_new ("Flickr API client key:"),
                     0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (client_grid),
                     gtk_label_new ("Flickr API client secret:"),
                     1, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (client_grid),
                     cli_key_entry,
                     0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (client_grid),
                     cli_sec_entry,
                     1, 1, 1, 1);

    gtk_grid_attach (GTK_GRID (request_grid),
                     gtk_label_new ("Flickr request token:"),
                     0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (request_grid),
                     gtk_label_new ("Flickr request secret:"),
                     1, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (request_grid),
                     req_tok_entry,
                     0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (request_grid),
                     req_sec_entry,
                     1, 1, 1, 1);

    gtk_grid_attach (GTK_GRID (verify_grid),
                     gtk_label_new (
                         "Paste here verifier text got after authorization:"),
                     0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (verify_grid),
                     verify_entry,
                     1, 0, 1, 1);

    gtk_grid_attach (GTK_GRID (access_grid),
                     gtk_label_new ("Flickr access token:"),
                     0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (access_grid),
                     gtk_label_new ("Flickr access token secret:"),
                     1, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (access_grid),
                     ath_tok_entry,
                     0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (access_grid),
                     ath_sec_entry,
                     1, 1, 1, 1);

    auth_button = gtk_button_new_with_label ("Generate authorization url");
    auth_link = gtk_link_button_new ("");
    gen_acc_button = gtk_button_new_with_label (
            "Generate access token and access token secret");

    widgets[FLA_CLI_KEY] = cli_key_entry;
    widgets[FLA_CLI_SEC] = cli_sec_entry;
    widgets[FLA_AUT_LNK] = auth_link;
    widgets[FLA_VER_CD]  = verify_entry;
    widgets[FLA_REQ_TOK] = req_tok_entry;
    widgets[FLA_REQ_SEC] = req_sec_entry;
    widgets[FLA_ATH_TOK] = ath_tok_entry;
    widgets[FLA_ATH_SEC] = ath_sec_entry;

    g_signal_connect_swapped (auth_button, "clicked",
            G_CALLBACK (event_flickr_auth_link_generate), widgets);
    g_signal_connect_swapped (gen_acc_button, "clicked",
            G_CALLBACK (event_flickr_access_keys_generate), widgets);

    /* Packing dialog widgets */
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new (
    "To get your API keys, you need to be registered on the Flickr website and "
    " fill the form for a key:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new ("(set application type as \"mobile\")"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_link_button_new (
        "https://www.flickr.com/services/apps/create/apply/"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        client_grid,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new (
    "After you get API keys, you need to generate url to authorize app."),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        auth_button,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        request_grid,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        auth_link,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        verify_grid,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_label_new ("And finally"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gen_acc_button,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        access_grid,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    gtk_widget_show_all (content_box);

    res = gtk_dialog_run (GTK_DIALOG (dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        cli_key = gtk_entry_get_text (GTK_ENTRY (cli_key_entry));
        cli_sec = gtk_entry_get_text (GTK_ENTRY (cli_sec_entry));
        ath_tok = gtk_entry_get_text (GTK_ENTRY (ath_tok_entry));
        ath_sec = gtk_entry_get_text (GTK_ENTRY (ath_sec_entry));

        free (strings->str[0]);
        free (strings->str[1]);
        free (strings->str[2]);
        free (strings->str[3]);

        strings->str[0] = strdup (cli_key);
        strings->str[1] = strdup (cli_sec);
        strings->str[2] = strdup (ath_tok);
        strings->str[3] = strdup (ath_sec);
    }
    gtk_widget_destroy (dialog);

    return res;
}
/*----------------------------------------------------------------------------*/

#endif

