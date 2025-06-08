#include "css_utils.h"

#include "../utilities/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* 
 * Calls an external Python script via popen to retrieve 
 * CSS rules specifically matching a given css_name and 
 * list of css_classes.
 */
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// GPT4o was used to help construct parse_applicable_css_rules
// It was used to generate a dummy parser, to see if it was a viable design route
// It was not a viable design route.
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
char* parse_applicable_css_rules(const char *filepath, CssProps* widget_css_properties) {
    // Build a Python list for classes: e.g. ["class1","class2"]
    char class_list[1024] = "[";
    for (size_t i = 0; i < widget_css_properties->num_classes; i++) {
        strcat(class_list, "\"");
        strcat(class_list, widget_css_properties->css_classes[i]);
        strcat(class_list, "\"");
        if (i < widget_css_properties->num_classes - 1) {
            strcat(class_list, ", ");
        }
    }
    strcat(class_list, "]");

    // Construct Python command
    char command[4096];
    snprintf(command, sizeof(command),
             "python3 -c 'import parsethecss;"
             "print(parsethecss.getTheCss(open(\"%s\").read(), \"%s\", %s))'",
             filepath, widget_css_properties->css_name, class_list);

    return execute_command_and_get_result(command);
}

#ifdef USE_GTK3
// TODO
// This does not work properly in Gtk3
// We need to make a function that converts between the GObject Type name like "GtkButton" and the css-class name like "button"
void get_css_name(GtkWidget* widget, CssProps* result){
    GObject *object = G_OBJECT(widget);
    GType type = G_OBJECT_TYPE (object);
	result->css_name = g_type_name(type);
	result->css_name = "button";
	g_print("css_name was %s\n", result->css_name);
}
#else
void get_css_name(GObject* object, CssProps* result){
    GValue val_name = G_VALUE_INIT;
    g_value_init(&val_name, G_TYPE_STRING);
    g_object_get_property(object, "css-name", &val_name);
    const gchar *css_name = g_value_get_string(&val_name);
    if (!css_name) {
        g_print("css-name not found.\n");
    }
    result->css_name = g_strdup(css_name);
    g_value_unset(&val_name);
}
#endif

#ifdef USE_GTK3
void get_css_classes(GtkWidget* widget, CssProps* result){
	GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(widget));
	GList *classes = gtk_style_context_list_classes(context);

	result->num_classes = g_list_length(classes);
	result->css_classes = malloc(sizeof(gchar*)*result->num_classes);

	int i = 0;
	for (GList *l = classes; l != NULL; l = l->next, i++) {
		result->css_classes[i] = (const char *)l->data;
	}
}
#else
void get_css_classes(GObject* object, CssProps* result){
    GValue val_classes = G_VALUE_INIT;
    g_value_init(&val_classes, G_TYPE_STRV);
    g_object_get_property(object, "css-classes", &val_classes);
    gchar **css_classes = g_value_get_boxed(&val_classes);
    if (css_classes) {
        result->css_classes = g_strdupv(css_classes);
        result->num_classes = g_strv_length(css_classes);
    } else {
        g_print("css-classes not found.\n");
    }
    g_value_unset(&val_classes);
}
#endif

/* 
 * Gets all attributes about css of a given GtkWidget
 */
CssProps get_css_class_information_of_widget(GtkWidget* widget){
    CssProps result = {NULL, NULL, 0};
#ifdef USE_GTK3
    get_css_name(widget, &result);
    get_css_classes(widget, &result);
#else
    get_css_name(G_OBJECT(widget), &result);
    get_css_classes(G_OBJECT(widget), &result);
#endif
    return result;
}

/* 
 * Applies a string of CSS rules to a single widget 
 * by creating a new GtkCssProvider and binding it at 
 * priority GTK_STYLE_PROVIDER_PRIORITY_APPLICATION.
 */
// TODO Radically change this function so that it works with GTK3 - there is no add_provier function in GTK3 that only targets a specific widget. 
// We need to instead put all the css into a single custom class unique to the widget, then add that custom class to the widget
// The custom class could just be its hash
void apply_css_to_widget(GtkWidget *widget, const char *css_data)
{
    GtkCssProvider *css_provider = gtk_css_provider_new();

    #ifdef USE_GTK3
	gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);
    GdkScreen *screen = gtk_widget_get_screen(widget);
    gtk_style_context_add_provider_for_screen(screen,
                                   GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    #else
	gtk_css_provider_load_from_data(css_provider, css_data, -1);
	GtkStyleContext *style_context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    #endif
    g_object_unref(css_provider);
}

/* 
 * Loads a CSS file from disk, creates a GtkCssProvider from it, 
 * then applies that provider globally to the default display. 
 */
void apply_css_globally(const gchar *css_file_path)
{
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GFile *css_file = g_file_new_for_path(css_file_path);

    #ifdef USE_GTK3
    gtk_css_provider_load_from_file(css_provider, css_file, NULL);
    #else
    gtk_css_provider_load_from_file(css_provider, css_file);
    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(css_provider);
    g_object_unref(css_file);
    #endif
}
