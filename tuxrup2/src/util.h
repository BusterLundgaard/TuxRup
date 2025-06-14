#ifndef UTIL_H
#define UTIL_H
int check_memory_allocated (void *x); 
gpointer* get_original_function_pointer(char* name);
GtkWidget* make_scrolled_window(int width, int height);
void empty_box(GtkWidget* box);
void add_class_to_widget(GtkWidget* widget, char* class);
void remove_class_from_widget(GtkWidget* widget, char* class);
bool contains_class(GtkWidget* widget, char* class_name);
void apply_css(char* css_string, GtkWidget* application_root);
char* get_widget_type_string(GtkWidget* widget);
char* get_widget_label(GtkWidget* widget);
void apply_css_to_widget(GtkWidget *widget, const gchar *css_data);
char* get_text_from_buffer(GtkTextBuffer *buffer);
#endif
