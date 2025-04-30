#ifndef TOOLBAR_H
#define TOOLBAR_H
#include <gtk/gtk.h>

GMenu* create_toolbar(GApplication* app);
void enable_toolbar(GtkApplication* app, GtkWindow* win, GMenu* menu_bar);

#endif
