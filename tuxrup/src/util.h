#ifndef UTIL_H
#define UTIL_H


#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else 
    #include <gtk-4.0/gtk/gtk.h>
#endif

#include "globals.h"
#include <clang-c/Index.h>
#include <stdbool.h>

enum widget_type_category get_widget_type_category(GtkWidget* w);

char* get_widget_type_category_str(enum widget_type_category c);

char* get_callback_type_category_str(enum gtk_callback_category c);

bool is_callback_remapable(enum widget_type_category widget_c, enum gtk_callback_category callback_c);

int get_child_number(GtkWidget* widget);

char* get_basename_without_extension(const char *filepath);

bool is_mappable_action(const gchar* event_name);

char* get_working_directory();

unsigned int hash_int(int key);

unsigned long hash_string(const char *str);

char* get_textview_text(GtkWidget* text_view);

void remove_callback (GtkWidget* w, char* callback_name);

char* get_document_path(char* function_name);

GtkWidget* create_textview(const char* text, bool editable);

void print_cursor_location(CXSourceLocation loc);

#endif