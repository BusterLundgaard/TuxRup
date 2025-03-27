#include "css_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* 
 * Calls an external Python script via popen to retrieve 
 * CSS rules specifically matching a given css_name and 
 * list of css_classes.
 */
char *getCss(const char *filepath,
             const char *css_name,
             const char **css_classes,
             size_t num_classes)
{
    FILE *fp;
    static char output[8192];
    char temp_buffer[1024];

    // Build a Python list for classes: e.g. ["class1","class2"]
    char class_list[1024] = "[";
    for (size_t i = 0; i < num_classes; i++) {
        strcat(class_list, "\"");
        strcat(class_list, css_classes[i]);
        strcat(class_list, "\"");
        if (i < num_classes - 1) {
            strcat(class_list, ", ");
        }
    }
    strcat(class_list, "]");

    // Construct Python command
    char command[4096];
    snprintf(command, sizeof(command),
             "python3 -c 'import parsethecss;"
             "print(parsethecss.getTheCss(open(\"%s\").read(), \"%s\", %s))'",
             filepath, css_name, class_list);

    fp = popen(command, "r");
    if (!fp) {
        perror("Failed to run Python function");
        return NULL;
    }

    output[0] = '\0';

    while (fgets(temp_buffer, sizeof(temp_buffer), fp) != NULL) {
        if (strlen(output) + strlen(temp_buffer) < sizeof(output)) {
            strcat(output, temp_buffer);
        } else {
            fprintf(stderr, "Output too large!\n");
            break;
        }
    }
    pclose(fp);

    return output;
}

/* 
 * Like getCss, but retrieves only the CSS properties 
 * as a separate dictionary or string (depending on your Python script).
 */
char *getCssProperties(const char *filepath,
                       const char *css_name,
                       const char **css_classes,
                       size_t num_classes)
{
    FILE *fp;
    static char output[8192];
    char temp_buffer[1024];

    char class_list[1024] = "[";
    for (size_t i = 0; i < num_classes; i++) {
        strcat(class_list, "\"");
        strcat(class_list, css_classes[i]);
        strcat(class_list, "\"");
        if (i < num_classes - 1) {
            strcat(class_list, ", ");
        }
    }
    strcat(class_list, "]");

    char command[4096];
    snprintf(command, sizeof(command),
             "python3 -c 'import parsethecss;"
             "print(parsethecss.get_css_properties(open(\"%s\").read(), \"%s\", %s))'",
             filepath, css_name, class_list);

    fp = popen(command, "r");
    if (!fp) {
        perror("Failed to run Python function");
        return NULL;
    }

    output[0] = '\0';

    while (fgets(temp_buffer, sizeof(temp_buffer), fp) != NULL) {
        if (strlen(output) + strlen(temp_buffer) < sizeof(output)) {
            strcat(output, temp_buffer);
        } else {
            fprintf(stderr, "Output too large!\n");
            break;
        }
    }

    pclose(fp);
    return output;
}

/* 
 * Reads "css-name" and "css-classes" from a GObject (e.g., a GtkWidget),
 * storing them in a CssProps struct for later usage. 
 */
CssProps listCssPropertiesOfGobject(GObject *object)
{
    CssProps result = {NULL, NULL, 0};

    /* Get "css-name" */
    GValue val_name = G_VALUE_INIT;
    g_value_init(&val_name, G_TYPE_STRING);
    g_object_get_property(object, "css-name", &val_name);
    const gchar *css_name = g_value_get_string(&val_name);
    if (css_name) {
        result.css_name = g_strdup(css_name);
        g_print("css-name = \"%s\"\n", result.css_name);
    } else {
        g_print("css-name not found.\n");
    }
    g_value_unset(&val_name);

    /* Get "css-classes" */
    GValue val_classes = G_VALUE_INIT;
    g_value_init(&val_classes, G_TYPE_STRV);
    g_object_get_property(object, "css-classes", &val_classes);
    gchar **css_classes = g_value_get_boxed(&val_classes);
    if (css_classes) {
        result.css_classes = g_strdupv(css_classes);
        result.num_classes = g_strv_length(css_classes);

        g_print("css-classes = [");
        for (guint i = 0; i < result.num_classes; i++) {
            g_print("\"%s\"%s",
                    css_classes[i],
                    (i < result.num_classes - 1) ? ", " : "");
        }
        g_print("]\n");
    } else {
        g_print("css-classes not found.\n");
    }
    g_value_unset(&val_classes);

    return result;
}

/* 
 * Applies a string of CSS rules to a single widget 
 * by creating a new GtkCssProvider and binding it at 
 * priority GTK_STYLE_PROVIDER_PRIORITY_APPLICATION.
 */
void changeCss(GtkWidget *widget, const char *css_data)
{
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, css_data, -1);

    GtkStyleContext *style_context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(style_context,
                                   GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);
}

/* 
 * Loads a CSS file from disk, creates a GtkCssProvider from it, 
 * then applies that provider globally to the default display. 
 */
void apply_css(const gchar *css_file_path)
{
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GFile *css_file = g_file_new_for_path(css_file_path);

    gtk_css_provider_load_from_file(css_provider, css_file);
    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(css_provider);
    g_object_unref(css_file);
}
