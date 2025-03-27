#ifndef CSS_UTILS_H
#define CSS_UTILS_H

#include <gtk/gtk.h>

/* Holds a widget's css-name and css-classes */
typedef struct {
    gchar *css_name;
    gchar **css_classes;
    guint num_classes;
} CssProps;

/* 
 * 
 Input: 
 filepath: path to a css file
 widget_css_properties: css name and class for a given widget
 Returns:
 All css rules in the given file relevant to the given widget 
 */
char* parse_applicable_css_rules(const char *filepath, CssProps* widget_css_properties);

/* 
 * Gets all attributes about css of a given Gobject
 */
CssProps get_css_class_information_of_widget(GtkWidget* widget);

/*
Applies css rules from a string to a widget
*/
void apply_css_to_widget(GtkWidget *widget, const char *css_data);

/*
Applies css rules from a given filepath to the entire application
*/
void apply_css_globally(const gchar *css_file_path);

#endif // CSS_UTILS_H
