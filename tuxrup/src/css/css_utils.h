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
 * Uses python to parse css code
 */
char *getCss(const char *filepath,
             const char *css_name,
             const char **css_classes,
             size_t num_classes);

/* 
 * does the same as getcss but returns only properties (useless)
 */
char *getCssProperties(const char *filepath,
                       const char *css_name,
                       const char **css_classes,
                       size_t num_classes);

/* 
 * Gets all attributes about css of a given Gobject
 */
CssProps listCssPropertiesOfGobject(GObject *object);
// changes css duh
void changeCss(GtkWidget *widget, const char *css_data);
// applies css from file
void apply_css(const gchar *css_file_path);

#endif // CSS_UTILS_H
