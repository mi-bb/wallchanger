/**
 * @file  webflickr.c
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
 * @brief  Settings for searching the Flickr website.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include "../config.h"

#ifdef HAVE_FLICKCURL
#include <gtk/gtk.h>
#include <flickcurl.h>
#include <err.h>

#include "strfun.h"
#include "errs.h"
#include "dlgsmsg.h"
#include "chquery.h"
#include "searchitem.h"
#include "webwidget_c.h"
#include "webflickr.h"
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
 * @param[in,out] gw_array  Array with widgets
 * @return        none
 */
static void
event_flickr_auth_link_generate (GtkWidget **gw_array)
{
    const char *s_cli_key = NULL; /* Client key */
    const char *s_cli_sec = NULL; /* Client secret */
    const char *s_req_tok = NULL; /* Request token */
    const char *s_req_sec = NULL; /* Request token secret */
    char       *s_uri     = NULL; /* Authorization url */
    flickcurl  *fc;
    int rc __attribute__ ((unused)) = 0 ;

    s_cli_key = gtk_entry_get_text (GTK_ENTRY (gw_array[FLA_CLI_KEY]));
    s_cli_sec = gtk_entry_get_text (GTK_ENTRY (gw_array[FLA_CLI_SEC]));

    if (str_is_empty_msg (s_cli_key, "Empty client key"))
        return;
    else if (str_is_empty_msg (s_cli_sec, "Empty client secret"))
        return;

    flickcurl_init ();

    fc = flickcurl_new ();

    flickcurl_set_oauth_client_key (fc, s_cli_key);
    flickcurl_set_oauth_client_secret (fc, s_cli_sec);

    rc = flickcurl_oauth_create_request_token (fc, NULL);

    s_req_tok = flickcurl_get_oauth_request_token (fc);
    s_req_sec = flickcurl_get_oauth_request_token_secret (fc);

    s_uri = flickcurl_oauth_get_authorize_uri (fc);

    gtk_entry_set_text (GTK_ENTRY (gw_array[FLA_REQ_TOK]), s_req_tok);
    gtk_entry_set_text (GTK_ENTRY (gw_array[FLA_REQ_SEC]), s_req_sec);
    gtk_link_button_set_uri (GTK_LINK_BUTTON (gw_array[FLA_AUT_LNK]), s_uri);
    gtk_button_set_label (GTK_BUTTON (gw_array[FLA_AUT_LNK]), s_uri);

    free (s_uri);
    flickcurl_free (fc);
    flickcurl_finish ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Generate access keys based on authorization key.
 *
 * @param[in,out] gw_array  Array with widgets
 * @return        none
 */
static void
event_flickr_access_keys_generate (GtkWidget **gw_array)
{
    const char *s_cli_key  = NULL; /* Client key */
    const char *s_cli_sec  = NULL; /* Client secret */
    const char *s_req_tok  = NULL; /* Request token */
    const char *s_req_sec  = NULL; /* Request token secret */
    const char *s_ath_tok  = NULL; /* Access token */
    const char *s_ath_sec  = NULL; /* Access token secret */
    const char *s_verifier = NULL; /* Verify code */
    flickcurl  *fc;
    int rc __attribute__ ((unused)) = 0 ;

    s_cli_key  = gtk_entry_get_text (GTK_ENTRY (gw_array[FLA_CLI_KEY]));
    s_cli_sec  = gtk_entry_get_text (GTK_ENTRY (gw_array[FLA_CLI_SEC]));
    s_req_tok  = gtk_entry_get_text (GTK_ENTRY (gw_array[FLA_REQ_TOK]));
    s_req_sec  = gtk_entry_get_text (GTK_ENTRY (gw_array[FLA_REQ_SEC]));
    s_verifier = gtk_entry_get_text (GTK_ENTRY (gw_array[FLA_VER_CD]));

    if      (str_is_empty_msg (s_cli_key,  "Empty client key"))
        return;
    else if (str_is_empty_msg (s_cli_sec,  "Empty client secret"))
        return;
    else if (str_is_empty_msg (s_req_tok,  "Empty request token"))
        return;
    else if (str_is_empty_msg (s_req_sec,  "Empty request token secret"))
        return;
    else if (str_is_empty_msg (s_verifier, "Empty verify code"))
        return;

    flickcurl_init ();

    fc = flickcurl_new ();

    flickcurl_set_oauth_client_key (fc, s_cli_key);
    flickcurl_set_oauth_client_secret (fc, s_cli_sec);
    flickcurl_set_oauth_request_token (fc, s_req_tok);
    flickcurl_set_oauth_request_token_secret (fc, s_req_sec);

    rc = flickcurl_oauth_create_access_token (fc, s_verifier);

    s_ath_tok = flickcurl_get_oauth_token (fc);
    s_ath_sec = flickcurl_get_oauth_token_secret (fc);

    gtk_entry_set_text (GTK_ENTRY (gw_array[FLA_ATH_TOK]), s_ath_tok);
    gtk_entry_set_text (GTK_ENTRY (gw_array[FLA_ATH_SEC]), s_ath_sec);

    flickcurl_free (fc);
    flickcurl_finish ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create file name using image title (display name) and url.
 *
 * @param[in]  s_disp_name  Diaplay name (image title)
 * @param[in]  s_image_url  Image url (used for getting extension)
 * @param[in]  s_image_id   Image id
 * @return     New string with name for saving file. After use it should be
 *             freed using free.
 */
static char *
flickr_create_file_name (const char *s_disp_name,
                         const char *s_image_url,
                         const char *s_image_id)
{
    const char *s_ext   = NULL; /* Pointer to extension */
    const char *s_end   = NULL; /* Pointer to end of valid string */
    char       *s_tmp   = NULL; /* Temporary string */
    char       *s_first = NULL; /* Pointer to first char of temp string */
    size_t      ui_inc  = 0;    /* Increment step for char write */
    size_t      ui_slen = 0;    /* Length of source display name */
    size_t      ui_elen = 0;    /* Length of extension string */
    size_t      ui_ilen = 0;    /* Length of id string */
    gunichar    u_c;            /* Uni char to examine */

    ui_slen = strlen (s_disp_name);
    ui_ilen = strlen (s_image_id);

    /* Get extension pointer and length */
    s_ext = strrchr (s_image_url, '.');
    if (s_ext != NULL)
        ui_elen = strlen (s_ext);

    /* Alloc space for name, 4 times more because of unicode grow possibility */
    s_tmp = malloc ((4 * ui_slen + ui_ilen + ui_elen + 2) * sizeof (char));
    memset (s_tmp, '\0', 4 * ui_slen + ui_ilen + ui_elen + 2);

    s_first = s_tmp;

    /* Examine name char by char */
    while (*s_disp_name != '\0') {
        ui_inc = 0;
        u_c = g_utf8_get_char (s_disp_name);
        /* Copy only alphanumeric chars, convert to lowercase if needed,
         * convert spaces to underscores */
        if (g_unichar_isalnum (u_c)) {
            if (g_unichar_isupper (u_c))
                u_c = g_unichar_tolower (u_c);
            ui_inc = (size_t) g_unichar_to_utf8 (u_c, s_tmp);
        }
        else if (g_unichar_isspace (u_c))
            ui_inc = (size_t) g_unichar_to_utf8 ('_', s_tmp);

        s_tmp += ui_inc;
        s_disp_name = g_utf8_find_next_char (s_disp_name, NULL);
    }
    ui_slen = strlen (s_first);

    /* Check name length if it is smaller than 255 bytes and shrink if needed */
    if (ui_slen + ui_elen + ui_ilen + 1 >= 255) {
        ui_slen = 255 - ui_elen - ui_ilen - 1;
    }
    g_utf8_validate (s_first, (gssize) ui_slen, &s_end);
    s_first[s_end - s_first] = '-';
    memcpy (s_first + (s_end - s_first) + 1, s_image_id, ui_ilen);
    memcpy (s_first + (s_end - s_first) + ui_ilen + 1, s_ext, ui_elen + 1);

    return s_first;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Error handler for Flickr.
 */
static void
my_message_handler(void *user_data __attribute__ ((unused)),
                   const char *message)
{
    warnx ("Flicr error : %s\n", message);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert Flickr flickcurl_photo info to SearchItem item.
 *
 * @param[in] fp_photo  flickcurl_photo object to convert
 * @return    SearchItem item
 */
static SearchItem *
flickrphoto_to_searchitem (flickcurl_photo *fp_photo)
{
    SearchItem *si_item = NULL;
    char       *s_name  = NULL;
    const char *s_title = NULL;

    si_item = searchitem_new ();
    s_title = fp_photo->fields[PHOTO_FIELD_title].string;

    if (fp_photo->fields[PHOTO_FIELD_owner_realname].string != NULL) {
        searchitem_set_author_name (
                si_item, fp_photo->fields[PHOTO_FIELD_owner_realname].string);
    }
    else if (fp_photo->fields[PHOTO_FIELD_owner_username].string != NULL) {
        searchitem_set_author_name (
                si_item, fp_photo->fields[PHOTO_FIELD_owner_username].string);
    }

    s_name = str_is_empty (s_title) ? strdup (fp_photo->id) : strdup (s_title);
    remove_non_alpha_space (s_name);

    searchitem_set_service_name (si_item, "Flickr");
    searchitem_set_display_name (si_item, s_name);
    searchitem_set_display_markup (si_item, s_name);
    free (s_name);

    searchitem_set_id_string (si_item, fp_photo->id);

    si_item->s_thumb_url = flickcurl_photo_as_source_uri (fp_photo, 'm');
    si_item->s_page_url  = flickcurl_photo_as_page_uri (fp_photo);

    if (fp_photo->fields[PHOTO_FIELD_originalformat].string != NULL)
        si_item->s_image_url = flickcurl_photo_as_source_uri (fp_photo, 'o');
    else
        si_item->s_image_url = flickcurl_photo_as_source_uri (fp_photo, 'b');

    si_item->s_file_name = flickr_create_file_name (si_item->s_display_name,
                                                    si_item->s_image_url,
                                                    si_item->s_id);
#ifdef DEBUG
    printf ("ID : %s\n", fp_photo->id);
    printf ("Title : %s\n", fp_photo->fields[PHOTO_FIELD_title].string);
    printf ("author1 : %s\n",
            fp_photo->fields[PHOTO_FIELD_owner_realname].string);
    printf ("author2 : %s\n",
            fp_photo->fields[PHOTO_FIELD_owner_username].string);
    printf ("Author : %s\n",si_item->s_author_name);
    printf ("Page url : %s\n",si_item->s_page_url);
    printf ("Thumb url : %s\n",si_item->s_thumb_url);
    printf ("Image url : %s\n",si_item->s_image_url);
    printf ("File name : %s\n",si_item->s_file_name);
    flickcurl_photo_field_type field_type;
    for (field_type = 0; field_type <= PHOTO_FIELD_LAST; field_type++) {
        flickcurl_field_value_type datatype = fp_photo->fields[field_type].type;

        if (datatype != VALUE_TYPE_NONE)
            printf("field %s (%d) with %s value: '%s' / %d\n", 
                   flickcurl_get_photo_field_label(field_type), (int)field_type,
                   flickcurl_get_field_value_type_label(datatype),
                   fp_photo->fields[field_type].string,
                   fp_photo->fields[field_type].integer);
    }

    for(int i = 0; i < fp_photo->tags_count; i++) {
        flickcurl_tag* tag=fp_photo->tags[i];
        printf ("%d) %s tag: id %s author ID %s name %s raw '%s' "
               "cooked '%s' count %d\n",
               i, (tag->machine_tag ? "machine" : "regular"),
               tag->id, tag->author, 
               (tag->authorname ? tag->authorname : "(Unknown)"), 
               tag->raw, tag->cooked,
               tag->count);
    }
#endif
    return si_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Search in Flickr database.
 */
void
flickr_search (WebWidget         *ww_widget,
               const FourStrings *fs_data)
{
    flickcurl_photos_list_params list_params;
    flickcurl_search_params      params;
    flickcurl_photos_list       *photos_list = NULL;
    flickcurl   *fc       = NULL;
    CacheQuery  *cq_query = NULL; /* For cache saving */
    SearchItem **si_items = NULL; /* List of item with photo info */
    SearchItem  *si_item  = NULL; /* Photo information */
    char        *s_tags   = NULL; /* Query tags */
    int          i_err    = 0;    /* Error output */
    int          i        = 0;    /* i */
    char         s_sort[64];      /* Sorting information */
    char         s_media[16];     /* Media type to search */

    flickcurl_init ();
    fc = flickcurl_new ();

    flickcurl_set_error_handler (fc, my_message_handler, NULL);

    flickcurl_set_oauth_client_key    (fc, fs_data->s_str1);
    flickcurl_set_oauth_client_secret (fc, fs_data->s_str2);
    flickcurl_set_oauth_token         (fc, fs_data->s_str3);
    flickcurl_set_oauth_token_secret  (fc, fs_data->s_str4);

    strcpy (s_sort,  "interestingness-desc");
    strcpy (s_media, "photos");
    s_tags = str_replace_in (ww_widget->s_query, " ", ",");

    flickcurl_search_params_init (&params);
    params.sort  = s_sort;
    params.tags  = s_tags;
    params.media = s_media;

    flickcurl_photos_list_params_init (&list_params);
    list_params.per_page = 12;
    list_params.page     = ww_widget->i_page;
    list_params.extras   = "original_format,o_dims,url_o,url_t";

    photos_list = flickcurl_photos_search_params (fc, &params, &list_params);

    gtk_list_store_clear (GTK_LIST_STORE (gtk_icon_view_get_model (
                    GTK_ICON_VIEW (ww_widget->gw_img_view))));

    if (photos_list != NULL) {
        cq_query = cachequery_new ("Flickr",
                                   ww_widget->s_query,
                                   ww_widget->s_search_opts,
                                   ww_widget->i_page);
        si_items = cq_query->si_items;

        ww_widget->i_found_cnt = photos_list->total_count;
        cq_query->i_found_cnt  = ww_widget->i_found_cnt;

        for(i = 0; i < photos_list->photos_count; ++i) {
            si_item = flickrphoto_to_searchitem (photos_list->photos[i]);
            add_searchitem_to_img_view (ww_widget->gw_img_view,
                                        si_item,
                                        ww_widget->s_wallp_dir,
                                        "Flickr",
                                        ww_widget->i_thumb_quality);
            *si_items++ = si_item;
        }
        i_err = cachequery_save (cq_query);

        if (i_err != ERR_OK) {
            message_dialog_error (NULL, err_get_message (i_err));
        }
        flickcurl_free_photos_list (photos_list);
        cachequery_free (cq_query);
    }
    free (s_tags);
    flickcurl_free (fc);
    flickcurl_finish ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with Flickr service settings.
 */
int
flickr_settings_dialog (FourStrings *fs_data)
{
    GtkWidget *gw_dialog;         /* Flickr settings dialog */
    GtkWidget *gw_content_box;    /* Dialog's box */
    GtkWidget *gw_cli_key_entry;  /* Entry for API key */
    GtkWidget *gw_cli_sec_entry;  /* Entry for API secret */
    GtkWidget *gw_verify_entry;   /* Entry for verifier */
    GtkWidget *gw_auth_button;    /* Button to authorize app */
    GtkWidget *gw_auth_link;      /* Authorization link */
    GtkWidget *gw_gen_acc_button; /* Button to authorize app */
    GtkWidget *gw_req_tok_entry;  /* Entry for API request token */
    GtkWidget *gw_req_sec_entry;  /* Entry for API request secret */
    GtkWidget *gw_ath_tok_entry;  /* Entry for API access key */
    GtkWidget *gw_ath_sec_entry;  /* Entry for API access secret */
    GtkWidget *gw_client_grid;    /* Grid for client API keys */
    GtkWidget *gw_request_grid;   /* Grid for request keys */
    GtkWidget *gw_verify_grid;    /* Grid for verify code */
    GtkWidget *gw_access_grid;    /* Grid for access keys */
    GtkWidget *gw_array[FLA_CNT]; /* Widgets for events */
    const char *s_cli_key = NULL; /* Client key */
    const char *s_cli_sec = NULL; /* Client secret */
    const char *s_ath_tok = NULL; /* Access token */
    const char *s_ath_sec = NULL; /* Access token secret */
    int         i_res     = 0;    /* Dialog result */

    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    gw_dialog = gtk_dialog_new_with_buttons ("Flickr configuration",
                                             NULL,
                                             flags,
                                             "_OK",
                                             GTK_RESPONSE_ACCEPT,
                                             "_Cancel",
                                             GTK_RESPONSE_REJECT,
                                             NULL);

    gw_content_box = gtk_dialog_get_content_area (GTK_DIALOG (gw_dialog));
    gtk_container_set_border_width (GTK_CONTAINER (gw_content_box), 8);

    gw_client_grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (gw_client_grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (gw_client_grid), 8);
    gtk_widget_set_halign (gw_client_grid, GTK_ALIGN_CENTER);

    gw_request_grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (gw_request_grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (gw_request_grid), 8);
    gtk_widget_set_halign (gw_request_grid, GTK_ALIGN_CENTER);

    gw_verify_grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (gw_verify_grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (gw_verify_grid), 8);
    gtk_widget_set_halign (gw_verify_grid, GTK_ALIGN_CENTER);

    gw_access_grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (gw_access_grid), 8);
    gtk_grid_set_column_spacing (GTK_GRID (gw_access_grid), 8);
    gtk_widget_set_halign (gw_access_grid, GTK_ALIGN_CENTER);

    gw_cli_key_entry = gtk_entry_new ();
    gw_cli_sec_entry = gtk_entry_new ();
    gw_verify_entry  = gtk_entry_new ();
    gw_req_tok_entry = gtk_entry_new ();
    gw_req_sec_entry = gtk_entry_new ();
    gw_ath_tok_entry = gtk_entry_new ();
    gw_ath_sec_entry = gtk_entry_new ();

    gtk_entry_set_width_chars (GTK_ENTRY (gw_cli_key_entry), 35);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_cli_sec_entry), 35);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_req_tok_entry), 35);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_req_sec_entry), 35);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_ath_tok_entry), 35);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_ath_sec_entry), 35);

    gtk_entry_set_text (GTK_ENTRY (gw_cli_key_entry), fs_data->s_str1);
    gtk_entry_set_text (GTK_ENTRY (gw_cli_sec_entry), fs_data->s_str2);
    gtk_entry_set_text (GTK_ENTRY (gw_ath_tok_entry), fs_data->s_str3);
    gtk_entry_set_text (GTK_ENTRY (gw_ath_sec_entry), fs_data->s_str4);

    gtk_grid_attach (GTK_GRID (gw_client_grid),
                     gtk_label_new ("Flickr API client key:"),
                     0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (gw_client_grid),
                     gtk_label_new ("Flickr API client secret:"),
                     1, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (gw_client_grid),
                     gw_cli_key_entry,
                     0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (gw_client_grid),
                     gw_cli_sec_entry,
                     1, 1, 1, 1);

    gtk_grid_attach (GTK_GRID (gw_request_grid),
                     gtk_label_new ("Flickr request token:"),
                     0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (gw_request_grid),
                     gtk_label_new ("Flickr request secret:"),
                     1, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (gw_request_grid),
                     gw_req_tok_entry,
                     0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (gw_request_grid),
                     gw_req_sec_entry,
                     1, 1, 1, 1);

    gtk_grid_attach (GTK_GRID (gw_verify_grid),
                     gtk_label_new (
                         "Paste here verifier text got after authorization:"),
                     0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (gw_verify_grid),
                     gw_verify_entry,
                     1, 0, 1, 1);

    gtk_grid_attach (GTK_GRID (gw_access_grid),
                     gtk_label_new ("Flickr access token:"),
                     0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (gw_access_grid),
                     gtk_label_new ("Flickr access token secret:"),
                     1, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (gw_access_grid),
                     gw_ath_tok_entry,
                     0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (gw_access_grid),
                     gw_ath_sec_entry,
                     1, 1, 1, 1);

    gw_auth_button = gtk_button_new_with_label ("Generate authorization url");
    gw_auth_link = gtk_link_button_new ("");
    gw_gen_acc_button = gtk_button_new_with_label (
            "Generate access token and access token secret");

    gw_array[FLA_CLI_KEY] = gw_cli_key_entry;
    gw_array[FLA_CLI_SEC] = gw_cli_sec_entry;
    gw_array[FLA_AUT_LNK] = gw_auth_link;
    gw_array[FLA_VER_CD]  = gw_verify_entry;
    gw_array[FLA_REQ_TOK] = gw_req_tok_entry;
    gw_array[FLA_REQ_SEC] = gw_req_sec_entry;
    gw_array[FLA_ATH_TOK] = gw_ath_tok_entry;
    gw_array[FLA_ATH_SEC] = gw_ath_sec_entry;

    g_signal_connect_swapped (gw_auth_button, "clicked",
            G_CALLBACK (event_flickr_auth_link_generate), gw_array);
    g_signal_connect_swapped (gw_gen_acc_button, "clicked",
            G_CALLBACK (event_flickr_access_keys_generate), gw_array);

    /* Packing dialog widgets */
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new (
    "To get your API keys, you need to be registered on the Flickr website and "
    " fill the form for a key:"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new ("(set application type as \"mobile\")"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_link_button_new (
        "https://www.flickr.com/services/apps/create/apply/"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_client_grid,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new (
    "After you get API keys, you need to generate url to authorize app."),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_auth_button,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_request_grid,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_auth_link,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_verify_grid,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_label_new ("And finally"),
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_gen_acc_button,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gw_access_grid,
                        FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_content_box),
                        gtk_separator_new (GTK_ORIENTATION_HORIZONTAL),
                        FALSE, FALSE, 4);

    gtk_widget_show_all (gw_content_box);

    i_res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (i_res == GTK_RESPONSE_ACCEPT) {
        s_cli_key = gtk_entry_get_text (GTK_ENTRY (gw_cli_key_entry));
        s_cli_sec = gtk_entry_get_text (GTK_ENTRY (gw_cli_sec_entry));
        s_ath_tok = gtk_entry_get_text (GTK_ENTRY (gw_ath_tok_entry));
        s_ath_sec = gtk_entry_get_text (GTK_ENTRY (gw_ath_sec_entry));

        free (fs_data->s_str1);
        free (fs_data->s_str2);
        free (fs_data->s_str3);
        free (fs_data->s_str4);

        fs_data->s_str1 = strdup (s_cli_key);
        fs_data->s_str2 = strdup (s_cli_sec);
        fs_data->s_str3 = strdup (s_ath_tok);
        fs_data->s_str4 = strdup (s_ath_sec);
    }
    gtk_widget_destroy (gw_dialog);

    return i_res;
}
/*----------------------------------------------------------------------------*/

#endif

