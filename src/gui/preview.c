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
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <gdk/gdk.h>
#include "icons.h"
#include "preview.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static GdkPixbuf * make_prev_screen_pbuf (GdkRectangle *area)
 *
 * @brief  Paint a preview screen.
 *
 * Create preview image (monitor) in GdkPixbuf format.
 *
 * @param[out] area  GdkRectangle with area where preview image can be
 *                   painted
 * @return     Pixbuf with preview screeen
 *
 * @fn  static void paint_pbuf_on_pbuf (GdkPixbuf          *dest,
 *                                      const GdkPixbuf    *src,
 *                                      const GdkRectangle *area)
 *
 * @brief  Paint pixbuf on another pixbuf.
 *
 * Paint GdkPixbuf image src on GdkPixbuf dest.
 * Image will be painted in rectangle described in area GdkRectangle.
 *
 * @param[out] dest  Destination pixpuf to paint on to
 * @param[in]  src   Source pixpuf to paint on destination one
 * @param[in]  area  GdkRectangle with area where preview image can be
 *                   painted
 * @return     none
 *
 * @fn  static GdkPixbuf * make_image_preview (const char *file)
 *
 * @brief  Create pixbuf preview of image from path.
 *
 * Function creates GdkPixbuf with preview image (monitor) and a scaled
 * wallpaper in it. If passed file is null, wallpaper will not be
 * painted inside preview image.
 *
 * @param[in]  file  Image path
 *
 * @return     Pixbuf with preview image.
 */
/*----------------------------------------------------------------------------*/
static GdkPixbuf * make_prev_screen_pbuf (GdkRectangle       *area);

static void        paint_pbuf_on_pbuf    (GdkPixbuf          *dest,
                                          const GdkPixbuf    *src,
                                          const GdkRectangle *area);

static GdkPixbuf * make_image_preview    (const char         *file);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Paint a preview screen.
 */
static GdkPixbuf *
make_prev_screen_pbuf (GdkRectangle *area)
{
    GdkPixbuf *result = nullptr; /* Result preview image (screen + wall) */
    GdkPixbuf *top    = nullptr; /* Preview (monitor) top part image */
    GdkPixbuf *middle = nullptr; /* Preview (monitor) middle part image */
    GdkPixbuf *bottom = nullptr; /* Preview (monitor) bottom part image */
    int screen_w      = 0;   /* Screen width */
    int screen_h      = 0;   /* Screen height */
    int prev_w        = 138; /* Preview width (inside screen) */
    int prev_h        = 0;   /* Preview height (inside screen) */
    int img_w         = 0;   /* Preview (monitor) image width */
    int top_h         = 0;   /* Preview (monitor) top part image height */
    int middle_h      = 0;   /* Preview (monitor) middle part image height */
    int bottom_h      = 0;   /* Preview (monitor) bottom part image height */

    get_screen_size (&screen_w, &screen_h);

    /* Count the height of a inside monitor preview screen */
    prev_h = (int) ((double) screen_h / (double) screen_w * (double) prev_w);

    area->x      = 7;      /* Preview left point (inside screen) */
    area->y      = 7;      /* Preview top point (inside screen) */
    area->width  = prev_w; /* Preview width (inside screen) */
    area->height = prev_h; /* Preview height (inside screen) */

    top    = get_icon (W_IMG_SCREEN_1);  /* top screen image (152x7) */
    middle = get_icon (W_IMG_SCREEN_2);  /* middle screen image (152x1) */
    bottom = get_icon (W_IMG_SCREEN_3);  /* bottom screen image (152x45) */

    /* Get dimensions of monitor part images */
    img_w    = gdk_pixbuf_get_width (top);
    top_h    = gdk_pixbuf_get_height (top);
    middle_h = gdk_pixbuf_get_height (middle);
    bottom_h = gdk_pixbuf_get_height (bottom);

    /* Create pixbuf for the whole preview image */
    result = gdk_pixbuf_new (GDK_COLORSPACE_RGB,
                             TRUE,
                             8,
                             img_w,
                             prev_h + top_h + bottom_h);

    /* Paint on the preview image upper first part of monitor */
    gdk_pixbuf_scale (top,
                      result,
                      0,
                      0,
                      img_w,
                      top_h,
                      0.0,
                      0.0,
                      1.0,
                      1.0,
                      GDK_INTERP_HYPER);

    /* Paint on preview image middle parts of a monitor */
    for (int row = 0; row < prev_h; ++row) {
        gdk_pixbuf_scale (middle,
                          result,
                          0,
                          top_h + row,
                          img_w,
                          middle_h,
                          0.0,
                          top_h + row,
                          1.0,
                          1.0,
                          GDK_INTERP_HYPER);
    }

    /* Paint on the preview image bottom last part of monitor */
    gdk_pixbuf_scale (bottom,
                      result,
                      0,
                      top_h + prev_h,
                      img_w,
                      bottom_h,
                      0.0,
                      top_h + prev_h,
                      1.0,
                      1.0,
                      GDK_INTERP_HYPER);

    g_object_unref (top);
    g_object_unref (middle);
    g_object_unref (bottom);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Paint pixbuf on another pixbuf.
 */
static void
paint_pbuf_on_pbuf (GdkPixbuf          *dest,
                    const GdkPixbuf    *src,
                    const GdkRectangle *area)
{
    double scale_x = 0;  /* x direction scale factor */
    double scale_y = 0;  /* y direction scale factor */

    scale_x = (double) area->width / (double) gdk_pixbuf_get_width (src);
    scale_y = (double) area->height / (double) gdk_pixbuf_get_height (src);

    gdk_pixbuf_scale (src,
                      dest,
                      area->x,
                      area->y,
                      area->width,
                      area->height,
                      area->x,
                      area->y,
                      scale_x,
                      scale_y,
                      GDK_INTERP_HYPER);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create pixbuf preview of image from path.
 */
static GdkPixbuf *
make_image_preview (const char *file)
{
    GdkPixbuf   *screen    = nullptr; /* Preview image (screen) */
    GdkPixbuf   *wallpaper = nullptr; /* Wallpaper preview image */
    GdkRectangle area      = {0};     /* Wallpaper preview area */

    /* Create the monitor preview pixbuf */
    screen = make_prev_screen_pbuf (&area);

    if (file != nullptr) {
        /* Load wallpaper image to pixbuf */
        wallpaper = gdk_pixbuf_new_from_file (file, nullptr);

        /* Paint wallpaper preview on the background (monitor) pixbuf */
        paint_pbuf_on_pbuf (screen, wallpaper, &area);
        g_object_unref (wallpaper);
    }
    return screen;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get screen size.
 */
void
get_screen_size (int *width,
                 int *height)
{
    GdkRectangle workarea = {0};
    GdkDisplay  *display  = gdk_display_get_default ();
    GdkMonitor  *monitor  = gdk_display_get_primary_monitor (display);

    /* Some window managers (notably under Wayland/XWayland) don't flag
     * any monitor as "primary" - fall back to the first available one. */
    if (monitor == nullptr) {
        monitor = gdk_display_get_monitor (display, 0);
    }

    if (monitor != nullptr) {
        gdk_monitor_get_workarea (monitor, &workarea);
    }

    *width  = workarea.width;
    *height = workarea.height;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Make preview image widget of image (file path).
 */
void
preview_from_file (GtkWidget  *image,
                   const char *file)
{
    /* Create peview pixbuf (monitor + wallpaper inside) */
    GdkPixbuf *preview = make_image_preview (file);

    if (preview != nullptr) {
        gtk_image_clear (GTK_IMAGE (image));
        /* Set new pixbuf to the image */
        gtk_image_set_from_pixbuf (GTK_IMAGE (image), preview);
        g_object_unref (preview);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create labels with screen resolution info.
 */
GtkWidget *
create_preview_label (void)
{
    GtkWidget *box;            /* Box with peview labels to return */
    GtkWidget *title;          /* "Screen resolution" label */
    GtkWidget *resolution;     /* Label with resolution value */
    int        width      = 0; /* Screen width */
    int        height     = 0; /* Screen height */
    char       markup[50];     /* Buffer for snprintf */

    box        = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
    title      = gtk_label_new (nullptr);
    resolution = gtk_label_new (nullptr);

    gtk_label_set_xalign (GTK_LABEL (title), 0.5);
    gtk_label_set_xalign (GTK_LABEL (resolution), 0.5);
    get_screen_size (&width, &height);
    snprintf (markup, sizeof markup, "<small>%dx%d</small>", width, height);

    gtk_label_set_markup (GTK_LABEL (title),
                          "<small>Screen resolution :</small>");
    gtk_label_set_markup (GTK_LABEL (resolution), markup);

    gtk_box_pack_start (GTK_BOX (box), title, FALSE, FALSE, 1);
    gtk_box_pack_start (GTK_BOX (box), resolution, FALSE, FALSE, 1);

    return box;
}
/*----------------------------------------------------------------------------*/

