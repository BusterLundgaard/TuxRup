#ifndef EDIT_CSS_WINDOW_H
#define EDIT_CSS_WINDOW_H

#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

#include "css_utils.h"   // So we can call changeCss(), getCss(), etc. if needed

extern GHashTable *widget_to_css_filepath_map;

/* This struct holds references for the text editor window. */
typedef struct {
    GtkWidget *text_view;
    GtkWidget *editor_window;
    GtkWidget *target_button;
    GtkWidget *done_button;
} EditorData;

extern GHashTable *widget_to_css_filepath_map;

/* 
 * Called when “Done” is clicked in the text editor window. 
 * Writes to a CSS file, applies the new CSS, etc.
 */
void on_done_clicked(GtkWidget *widget, gpointer user_data);

/* 
 * Opens the text editor window (called from on_right_click). 
 */
void open_css_editor(GtkWidget *widget);

#endif /* EDITOR_UTILS_H */
