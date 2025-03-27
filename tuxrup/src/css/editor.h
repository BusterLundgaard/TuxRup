#ifndef EDITOR_UTILS_H
#define EDITOR_UTILS_H

#include <gtk/gtk.h>
#include "css_utils.h"   // So we can call changeCss(), getCss(), etc. if needed

extern GHashTable *widget_to_filepath_map;

/* This struct holds references for the text editor window. */
typedef struct {
    GtkWidget *text_view;
    GtkWidget *editor_window;
    GtkWidget *target_button;
} EditorData;

extern GHashTable *widget_to_filepath_map;

/* 
 * Called when “Done” is clicked in the text editor window. 
 * Writes to a CSS file, applies the new CSS, etc.
 */
void on_done_clicked(GtkWidget *widget, gpointer user_data);

/* 
 * Opens the text editor window (called from on_right_click). 
 */
void open_text_editor(GtkWidget *widget, gpointer user_data);

/*
 * A convenience function to attach a right-click gesture to a given widget,
 * so that if the user right-clicks, it calls open_text_editor().
 */
void on_right_click(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data);

#endif /* EDITOR_UTILS_H */
