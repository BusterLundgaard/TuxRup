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

void set_gtk_version();

enum widget_type_category get_widget_type_category(GtkWidget* w);

GObjectClass* get_widget_class(GtkWidget* widget);

GtkWidget* create_window(GtkWidget* any_widget_from_current_application, const char* title, guint width, guint height);

GtkWidget* create_and_add_scrollable_item_list(GtkWidget* window, guint width, guint height);

GtkWidget* add_widget_to_window(GtkWidget* window, GtkWidget* widget);

GtkWidget* add_widget_to_box(GtkWidget* box, GtkWidget* widget);

char* get_widget_type_category_str(enum widget_type_category c);

char* get_callback_type_category_str(enum gtk_callback_category c);

bool is_callback_remapable(enum widget_type_category widget_c, enum gtk_callback_category callback_c);

int get_child_number(GtkWidget* widget);

char* get_basename_without_extension(const char *filepath);

void copy_file(const char* file_path, const char* copied_path);

bool is_mappable_action(const gchar* event_name);

char* get_working_directory();

char* get_executable_directory();

gboolean file_exists(const gchar *filepath);

char* fix_broken_fucking_css_path(char* path);

unsigned int hash_int(int key);

unsigned long hash_string(const char *str);

char* get_textview_text(GtkWidget* text_view);

void remove_callback (GtkWidget* w, enum gtk_callback_category callback);

char* get_document_path(char* function_name);

GtkWidget* create_textview(const char* text, bool editable);

void print_cursor_location(CXSourceLocation loc);

void add_right_click_action(GtkWidget* widget, right_click_callback_type callback, gpointer user_data);

char* execute_command_and_get_result(char* command);

char* read_file_contents(char* filepath);

void append_text_to_file(char* filepath, char* text);

typedef void(*hash_table_iter_function)(gpointer key, gpointer value, int i, gpointer user_data);
void iterate_hash_table(GHashTable* table, hash_table_iter_function iter_function);

#endif