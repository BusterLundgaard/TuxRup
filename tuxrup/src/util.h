#ifndef UTIL_H
#define UTIL_H

#include <gtk/gtk.h>
#include "globals.h"

int get_child_number(GtkWidget* widget);

char* get_basename_without_extension(const char *filepath);

void remove_signal (GtkWidget* w, char* signal_name);

bool is_mappable_action(const gchar* event_name);

char* get_working_directory();

#endif