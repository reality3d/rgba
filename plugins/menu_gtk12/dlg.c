#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <glib-1.2/glib.h>   // Fixme! ;)
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "dlg.h"

// GLT
#include <glt/image.h>


/* This is an internally used function to create pixmaps. */
/*
static GtkWidget *create_pixmap_xpm (GtkWidget       *widget,
                                     const gchar     *filename)
{
    GdkColormap *colormap;
    GdkPixmap *gdkpixmap;
    GdkBitmap *mask;
    GtkWidget *pixmap;

    colormap = gtk_widget_get_colormap (widget);
    gdkpixmap = gdk_pixmap_colormap_create_from_xpm (NULL, colormap, &mask,
                                                     NULL, filename);
    if (gdkpixmap == NULL)
    {
        g_warning (_("Error loading pixmap file: %s"), filename);
        exit (1);
    }

    pixmap = gtk_pixmap_new (gdkpixmap, mask);
    gdk_pixmap_unref (gdkpixmap);
    gdk_bitmap_unref (mask);
    return pixmap;
}
*/
/*
static void PreMult (TImage image)
{
    int i;
    unsigned char *p;

    p = (unsigned char *) image->data;

    for (i=0; i<image->width*image->height*4; i+=4, p+=4)
    {
        p[0] = (p[0] * p[3] >> 8) + ((0xFF - p[3]) * 0xFF >> 8);
        p[1] = (p[1] * p[3] >> 8) + ((0xFF - p[3]) * 0xFF >> 8);
        p[2] = (p[2] * p[3] >> 8) + ((0xFF - p[3]) * 0xFF >> 8);
        p[3] = 0xFF;
    }
}

static GtkWidget *create_pixmap_png (GtkWidget       *widget,
                                     const gchar     *filename)
{
    TImage image;
    GtkWidget *pixmap;
    GdkPixmap *gdkpixmap;
    GdkGC     *gc;

    image = IMG_Read ((char *)filename);
    if (!image)
    {
        printf ("IMG_Read error: can't load %s\n", filename);
        exit (1);
    }
    PreMult (image);
    //printf ("%ix%i\n", image->width, image->height);
    gdkpixmap = gdk_pixmap_new (NULL,
                                IMG_GetWidth(image),
                                IMG_GetHeight(image),
                                (gtk_widget_get_visual (widget))->depth);
    gc = gdk_gc_new (gdkpixmap);
    gdk_rgb_init ();
    gdk_draw_rgb_32_image (gdkpixmap, gc, 0, 0,
                           IMG_GetWidth (image),
                           IMG_GetHeight (image),
                           GDK_RGB_DITHER_NONE,
                           (unsigned char *) IMG_GetData (image),
                           IMG_GetWidth (image) * 4);

    pixmap = gtk_pixmap_new (gdkpixmap, NULL);
    gdk_gc_unref (gc);
    gdk_pixmap_unref (gdkpixmap);
    IMG_Delete (image);

    return pixmap;
}
*/
void DLG_Show (TDialogData *dd, const char *demo_name)
{
    GtkWidget *window1;
    GtkWidget *fixed1;
    GtkWidget *button1;
//    GtkWidget *pixmap1;
    GtkWidget *button2;
    GtkWidget *frame1;
    GtkWidget *fixed2;
    GtkWidget *label1;
    GtkWidget *label2;
    GtkWidget *combo1;
    GtkWidget *combo_entry1;
    GtkWidget *checkbutton1;
    GList     *glist;
    int        dest_signal;
    char      *res;

    gtk_init (NULL, NULL);

    window1 = gtk_window_new (GTK_WINDOW_DIALOG);
    gtk_object_set_data (GTK_OBJECT (window1), "window1", window1);
    gtk_widget_set_usize (window1, 256+16*2, 150);
    gtk_window_set_title (GTK_WINDOW (window1), _(demo_name));
    gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
    gtk_window_set_modal (GTK_WINDOW (window1), TRUE);
    gtk_window_set_policy (GTK_WINDOW (window1), FALSE, FALSE, FALSE);

    fixed1 = gtk_fixed_new ();
    gtk_widget_ref (fixed1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed1", fixed1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed1);
    gtk_container_add (GTK_CONTAINER (window1), fixed1);

    button1 = gtk_button_new_with_label (_("run"));
    gtk_widget_ref (button1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button1", button1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button1);
    gtk_fixed_put (GTK_FIXED (fixed1), button1, 64, 112);
    gtk_widget_set_uposition (button1, 64, 112);
    gtk_widget_set_usize (button1, 47, 22);
    gtk_signal_connect (GTK_OBJECT (button1), "clicked", GTK_SIGNAL_FUNC (gtk_main_quit), NULL);

    //pixmap1 = create_pixmap_xpm (window1, "navmo.xpm");
/*
    pixmap1 = create_pixmap_png (window1, "menu/navmo.png");
    gtk_widget_ref (pixmap1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "pixmap1", pixmap1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (pixmap1);
    gtk_fixed_put (GTK_FIXED (fixed1), pixmap1, 128, 64);
    gtk_widget_set_uposition (pixmap1, 128, 64);
    gtk_widget_set_usize (pixmap1, 21, 21);
*/
    button2 = gtk_button_new_with_label (_("exit"));
    gtk_widget_ref (button2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "button2", button2,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button2);
    gtk_fixed_put (GTK_FIXED (fixed1), button2, 174, 112);
    gtk_widget_set_uposition (button2, 174, 112);
    gtk_widget_set_usize (button2, 47, 22);
    gtk_signal_connect (GTK_OBJECT (button2), "clicked", GTK_SIGNAL_FUNC (gtk_exit), NULL);

    frame1 = gtk_frame_new (_(demo_name));
    gtk_widget_ref (frame1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "frame1", frame1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame1);
    gtk_fixed_put (GTK_FIXED (fixed1), frame1, 16, 11);
    gtk_widget_set_uposition (frame1, 16, 11);
    gtk_widget_set_usize (frame1, 256, 95);
    gtk_frame_set_label_align (GTK_FRAME (frame1), 0.5, 0.5);
    gtk_frame_set_shadow_type (GTK_FRAME (frame1), GTK_SHADOW_ETCHED_OUT);

    fixed2 = gtk_fixed_new ();
    gtk_widget_ref (fixed2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "fixed2", fixed2,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (fixed2);
    gtk_container_add (GTK_CONTAINER (frame1), fixed2);

    label1 = gtk_label_new (_(">> windowed ::"));
    gtk_widget_ref (label1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label1", label1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label1);
    gtk_fixed_put (GTK_FIXED (fixed2), label1, 16, 12);
    gtk_widget_set_uposition (label1, 16, 12);
    gtk_widget_set_usize (label1, 100, 16);

    label2 = gtk_label_new (_(">> resolution ::"));
    gtk_widget_ref (label2);
    gtk_object_set_data_full (GTK_OBJECT (window1), "label2", label2,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label2);
    gtk_fixed_put (GTK_FIXED (fixed2), label2, 16, 44);
    gtk_widget_set_uposition (label2, 16, 44);
    gtk_widget_set_usize (label2, 100, 16);

    combo1 = gtk_combo_new ();
    gtk_widget_ref (combo1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo1", combo1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo1);
    gtk_fixed_put (GTK_FIXED (fixed2), combo1, 112, 41);
    gtk_widget_set_uposition (combo1, 112, 41);
    gtk_widget_set_usize (combo1, 108, 22);

    combo_entry1 = GTK_COMBO (combo1)->entry;
    gtk_widget_ref (combo_entry1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "combo_entry1", combo_entry1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (combo_entry1);

    checkbutton1 = gtk_check_button_new_with_label ("");
    gtk_widget_ref (checkbutton1);
    gtk_object_set_data_full (GTK_OBJECT (window1), "checkbutton1", checkbutton1,
                              (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (checkbutton1);
    gtk_fixed_put (GTK_FIXED (fixed2), checkbutton1, 203, 7);
    gtk_widget_set_uposition (checkbutton1, 203, 7);
    gtk_widget_set_usize (checkbutton1, 16, 24);
    GTK_WIDGET_UNSET_FLAGS (checkbutton1, GTK_CAN_FOCUS);


    glist = NULL;
    glist = g_list_append (glist, "320x200");
    glist = g_list_append (glist, "320x240");
    glist = g_list_append (glist, "640x400");
    glist = g_list_append (glist, "640x480");
    glist = g_list_append (glist, "800x600");
    glist = g_list_append (glist, "1024x768");
    glist = g_list_append (glist, "1280x1024");
    gtk_combo_set_popdown_strings (GTK_COMBO(combo1), glist);
    gtk_entry_set_text (GTK_ENTRY(combo_entry1), "800x600");

    dest_signal = gtk_signal_connect (GTK_OBJECT (window1), "destroy", GTK_SIGNAL_FUNC (gtk_exit), NULL);
    gtk_widget_show (window1);

    gtk_main ( );


    // Get resolution
    res = gtk_entry_get_text (GTK_ENTRY(combo_entry1));
    if (!strcmp (res, "320x200"))
    {
        dd->width  = 320;
        dd->height = 200;
    }
    else if (!strcmp (res, "320x240"))
    {
        dd->width  = 320;
        dd->height = 240;
    }
    else if (!strcmp (res, "640x400"))
    {
        dd->width  = 640;
        dd->height = 400;
    }
    else if (!strcmp (res, "640x480"))
    {
        dd->width  = 640;
        dd->height = 480;
    }
    else if (!strcmp (res, "800x600"))
    {
        dd->width  = 800;
        dd->height = 600;
    }
    else if (!strcmp (res, "1024x768"))
    {
        dd->width  = 1024;
        dd->height = 768;
    }
    else if (!strcmp (res, "1280x1024"))
    {
        dd->width  = 1280;
        dd->height = 1024;
    }
    else
    {
        // default 800x600
        dd->width  = 800;
        dd->height = 600;
    }

    dd->windowed = (int) GTK_TOGGLE_BUTTON (checkbutton1)->active;

    gtk_signal_disconnect (GTK_OBJECT (window1), dest_signal);
    gtk_widget_hide (window1);
    gtk_widget_destroy (window1);
    gtk_main_iteration_do (0);
}

