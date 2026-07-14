/**
 * @file  preview.c
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
 * @brief  Wallpaper preview
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <gdk/gdk.h>
#include "imgs.h"
#include "preview.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static GdkPixbuf * make_prev_screen_pbuf (GdkRectangle *gr_rect)
 *
 * @brief  Paint a preview screen.
 *
 * Create preview image (monitor) in GdkPixbuf format.
 *
 * @param[out] gr_rect  GdkRectangle with area where preview image can be 
 *                      painted
 * @return     Pixbuf with preview screeen
 *
 * @fn  static void paint_pbuf_on_pbuf (GdkPixbuf          *gp_dest,
 *                                      const GdkPixbuf    *gp_src,
 *                                      const GdkRectangle *gr_area)
 *
 * @brief  Paint pixbuf on another pixbuf.
 *
 * Paint GdkPixbuf image gp_src on GdkPixbuf gp_dest.
 * Image will be painted in rectangle described in gr_area GdkRectangle.
 *
 * @param[out] gp_dest  Destination pixpuf to paint on to
 * @param[in]  gp_src   Source pixpuf to paint on destination one
 * @param[in]  gr_area  GdkRectangle with area where preview image can be 
 *                      painted
 * @return     none
 *
 * @fn  static GdkPixbuf * make_image_preview (const char *s_fname)
 *
 * @brief  Create pixbuf preview of image from path.
 *
 * Function creates GdkPixbuf with preview image (monitor) and a scalled
 * wallpaper in it. If passed s_fname is null, wallpaper will not be
 * painted inside preview image.
 *
 * @param[in]  s_fname   Image path
 *
 * @return     Pixbuf with preview image.
 */
/*----------------------------------------------------------------------------*/
static GdkPixbuf * make_prev_screen_pbuf (GdkRectangle       *gr_rect);

static void        paint_pbuf_on_pbuf    (GdkPixbuf          *gp_dest,
                                          const GdkPixbuf    *gp_src,
                                          const GdkRectangle *gr_area);

static GdkPixbuf * make_image_preview    (const char         *s_fname);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Paint a preview screen.
 */
static GdkPixbuf *
make_prev_screen_pbuf (GdkRectangle *gr_rect)
{
    GdkPixbuf *gp_bg    = NULL; /* Result preview image (screen + wall) */
    GdkPixbuf *gp_scr_1 = NULL; /* Preview (monitor) part 1 (top) image*/
    GdkPixbuf *gp_scr_2 = NULL; /* Preview (monitor) part 1 (top) image*/
    GdkPixbuf *gp_scr_3 = NULL; /* Preview (monitor) part 1 (top) image*/
    int i_screen_w = 0;   /* Screen width */
    int i_screen_h = 0;   /* Screen height */
    int i_pr_scr_w = 138; /* Preview width (inside screen) */
    int i_pr_scr_h = 0;   /* Preview height (inside screen) */
    int i_img_w    = 0;   /* Preview (monitor) image width */
    int i_img_1_h  = 0;   /* Preview (monitor) part 1 (top) image height */
    int i_img_2_h  = 0;   /* Preview (monitor) part 2 (middle) image height */
    int i_img_3_h  = 0;   /* Preview (monitor) part 3 (bottom) image height */
    int i          = 0;   /* The i */

    get_screen_size (&i_screen_w, &i_screen_h);

    /* Count the height of a inside monitor preview screen */
    i_pr_scr_h = (int) ((double) i_screen_h /
                        (double) i_screen_w * (double) i_pr_scr_w);

    gr_rect->x = 7;               /* Preview left point (inside screen) */
    gr_rect->y = 7;               /* Preview top point (inside screen) */
    gr_rect->width = i_pr_scr_w;  /* Preview width (inside screen) */
    gr_rect->height = i_pr_scr_h; /* Preview height (inside screen) */

    gp_scr_1 = get_image (W_IMG_SCREEN_1);  /* top screen image (152x7) */
    gp_scr_2 = get_image (W_IMG_SCREEN_2);  /* middle screen image (152x1) */
    gp_scr_3 = get_image (W_IMG_SCREEN_3);  /* bottom screen image (152x45) */

    /* Get dimensions of monitor part images */
    i_img_w = gdk_pixbuf_get_width (gp_scr_1);
    i_img_1_h = gdk_pixbuf_get_height (gp_scr_1);
    i_img_2_h = gdk_pixbuf_get_height (gp_scr_2);
    i_img_3_h = gdk_pixbuf_get_height (gp_scr_3);

    /* Create pixbuf for the whole preview image */
    gp_bg = gdk_pixbuf_new (GDK_COLORSPACE_RGB,
                            TRUE,
                            8,
                            i_img_w,
                            i_pr_scr_h + i_img_1_h + i_img_3_h);

    /* Paint on the preview image upper first part of monitor */
    gdk_pixbuf_scale (gp_scr_1,
                      gp_bg,
                      0,
                      0,
                      i_img_w,
                      i_img_1_h,
                      0.0,
                      0.0,
                      1.0,
                      1.0,
                      GDK_INTERP_HYPER);

    /* Paint on preview image middle parts of a monitor */
    for (i = 0; i < i_pr_scr_h; ++i) {
        gdk_pixbuf_scale (gp_scr_2,
                          gp_bg,
                          0,
                          i_img_1_h + i,
                          i_img_w,
                          i_img_2_h,
                          0.0,
                          i_img_1_h + i,
                          1.0,
                          1.0,
                          GDK_INTERP_HYPER);
    }

    /* Paint on the preview image bottom last part of monitor */
    gdk_pixbuf_scale (gp_scr_3,
                      gp_bg,
                      0,
                      i_img_1_h + i_pr_scr_h,
                      i_img_w,
                      i_img_3_h,
                      0.0,
                      i_img_1_h + i_pr_scr_h,
                      1.0,
                      1.0,
                      GDK_INTERP_HYPER);

    g_object_unref (gp_scr_1);
    g_object_unref (gp_scr_2);
    g_object_unref (gp_scr_3);

    return gp_bg;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Paint pixbuf on another pixbuf.
 */
static void
paint_pbuf_on_pbuf (GdkPixbuf          *gp_dest,
                    const GdkPixbuf    *gp_src,
                    const GdkRectangle *gr_area)
{
    double d_sc_x = 0;  /* x direction scale factor */
    double d_sc_y = 0;  /* y direction scale factor */

    d_sc_x = (double) gr_area->width / (double) gdk_pixbuf_get_width (gp_src);
    d_sc_y = (double) gr_area->height / (double) gdk_pixbuf_get_height (gp_src);

    gdk_pixbuf_scale (gp_src,
                      gp_dest,
                      gr_area->x,
                      gr_area->y,
                      gr_area->width,
                      gr_area->height,
                      gr_area->x,
                      gr_area->y,
                      d_sc_x, 
                      d_sc_y,
                      GDK_INTERP_HYPER);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create pixbuf preview of image from path.
 */
static GdkPixbuf *
make_image_preview (const char *s_fname)
{
    GdkPixbuf   *gp_prev_screen = NULL; /* Preview image (screen) */
    GdkPixbuf   *gp_prev        = NULL; /* Wallpaper preview image */
    GdkRectangle gr_area        = {0};  /* Wallpaper preview area */

    /* Create the monitor preview pixbuf */
    gp_prev_screen = make_prev_screen_pbuf (&gr_area);

    if (s_fname != NULL) {
        /* Load wallpaper image to pixbuf */
        gp_prev = gdk_pixbuf_new_from_file (s_fname, NULL);

        /* Paint wallpaper preview on the background (monitor) pixbuf */
        paint_pbuf_on_pbuf (gp_prev_screen,
                            gp_prev,
                            &gr_area);
        g_object_unref (gp_prev);
    }
    return gp_prev_screen;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get screen size.
 */
void
get_screen_size (int *i_scr_w,
                 int *i_scr_h)
{
    GdkRectangle workarea = {0};
    GdkDisplay  *gd_disp  = gdk_display_get_default ();
    GdkMonitor  *gm_mon   = gdk_display_get_primary_monitor (gd_disp);

    /* Some window managers (notably under Wayland/XWayland) don't flag
     * any monitor as "primary" - fall back to the first available one. */
    if (gm_mon == NULL) {
        gm_mon = gdk_display_get_monitor (gd_disp, 0);
    }

    if (gm_mon != NULL) {
        gdk_monitor_get_workarea (gm_mon, &workarea);
    }

    *i_scr_w = workarea.width;
    *i_scr_h = workarea.height;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make preview image widget of image (file path).
 */
void
preview_from_file (GtkWidget  *gw_img_prev,
                   const char *s_file)
{
    /* Create peview pixbuf (monitor + wallpaper inside) */
    GdkPixbuf *gp_prev = make_image_preview (s_file);

    if (gp_prev != NULL) {
        gtk_image_clear (GTK_IMAGE (gw_img_prev));
        /* Set new pixbuf to the image */
        gtk_image_set_from_pixbuf (GTK_IMAGE (gw_img_prev), gp_prev);
        g_object_unref (gp_prev);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create labels with screen resolution info.
 */
GtkWidget *
create_preview_label (void)
{
    GtkWidget *gw_label_box;     /* Box with peview labels to return */
    GtkWidget *gw_label_1;       /* Desctiption label 1 */
    GtkWidget *gw_label_2;       /* Description label 2 */
    int        i_w          = 0; /* Screen width */
    int        i_h          = 0; /* screen height */
    char       s_markup[50];     /* Buffer for snprintf */

    gw_label_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
    gw_label_1   = gtk_label_new (NULL);
    gw_label_2   = gtk_label_new (NULL);

    gtk_label_set_xalign (GTK_LABEL (gw_label_1), 0.5);
    gtk_label_set_xalign (GTK_LABEL (gw_label_2), 0.5);
    get_screen_size (&i_w, &i_h);
    snprintf (s_markup, 50, "<small>%dx%d</small>", i_w, i_h);

    gtk_label_set_markup (GTK_LABEL (gw_label_1), 
                          "<small>Screen resolution :</small>");
    gtk_label_set_markup (GTK_LABEL (gw_label_2), s_markup);

    gtk_box_pack_start (GTK_BOX (gw_label_box), gw_label_1, FALSE, FALSE, 1);
    gtk_box_pack_start (GTK_BOX (gw_label_box), gw_label_2, FALSE, FALSE, 1);

    return gw_label_box;
}
/*----------------------------------------------------------------------------*/

