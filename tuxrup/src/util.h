#ifndef UTIL_H
#define UTIL_H

#include <gtk/gtk.h>
#include "globals.h"
#include <clang-c/Index.h>

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