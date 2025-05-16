#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>

#include "util.h"

gpointer* get_original_function_pointer(char* name){
	void* p = dlsym(RTLD_NEXT, name);
	if(p == NULL){
		g_print("Could not load function %s\n", name);
		exit(1);
	}
	return p;
}

GtkWidget* make_scrolled_window(int width, int height){
	GtkAdjustment* h_adj = gtk_adjustment_new(0,0,0,0,0,0);
	GtkAdjustment* v_adj = gtk_adjustment_new(0,0,0,0,0,0);
	GtkWidget* scrolled_window = gtk_scrolled_window_new(v_adj, h_adj);
	gtk_widget_set_size_request(scrolled_window, width, height);
	gtk_scrolled_window_set_policy(
			GTK_SCROLLED_WINDOW(scrolled_window), 
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	return scrolled_window;
}

void empty_box(GtkWidget* box){
	GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(box));
    for (iter = children; iter; iter = g_list_next(iter)) {
        gtk_container_remove(GTK_CONTAINER(box), GTK_WIDGET(iter->data));
    }
    g_list_free(children);
}


// CSS methods:
// Adds a CSS class to a widget
void add_class_to_widget(GtkWidget* widget, char* class){
	GtkStyleContext* context = gtk_widget_get_style_context(widget);
	gtk_style_context_add_class(context, class);
}
// Removes a CSS class from a widget
void remove_class_from_widget(GtkWidget* widget, char* class){
	GtkStyleContext* context = gtk_widget_get_style_context(widget);
	gtk_style_context_remove_class(context, class);
}
// Checks if a widget contains a certain class_name
bool contains_class(GtkWidget* widget, char* class_name){
	GtkStyleContext* context = gtk_widget_get_style_context(widget);
	GList* classes = gtk_style_context_list_classes(context);
	for(GList* elem = classes; elem; elem = elem->next){
        if (g_strcmp0(elem->data, class_name) == 0) {
            return true;
        }
    }
	return false;
}

void apply_css(char* css_string, GtkWidget* application_root){
	GtkCssProvider *provider = gtk_css_provider_new();    
    gtk_css_provider_load_from_data(provider, css_string, -1, NULL); 
    GdkScreen *screen = gtk_widget_get_screen(application_root);
    gtk_style_context_add_provider_for_screen(screen,
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider); 
}


// Detect and convert widget types methods:
bool observed_type(GtkWidget* widget){
	return 
		GTK_IS_BUTTON(widget) ||
		GTK_IS_ENTRY(widget) ||
		GTK_IS_TEXT_BUFFER(widget) ||
		GTK_IS_CHECK_BUTTON(widget) ||
		GTK_IS_TOGGLE_BUTTON(widget) ||
		GTK_IS_SPIN_BUTTON(widget) ||
		GTK_IS_SCALE(widget) ||
		GTK_IS_COMBO_BOX(widget) ||
		GTK_IS_COMBO_BOX_TEXT(widget);
}

char* get_widget_type_string(GtkWidget* widget){
	if      (GTK_IS_CHECK_BUTTON(widget))  {return "check_button";} 
	else if (GTK_IS_TOGGLE_BUTTON(widget)) {return "toggle_button";} 
	else if (GTK_IS_SPIN_BUTTON(widget))   {return "spin_button";} 
	else if (GTK_IS_BUTTON(widget))        {return "button";}
	else if (GTK_IS_LABEL(widget))		   {return "label";}
	else if (GTK_IS_ENTRY(widget))         {return "entry";} 
	else if (GTK_IS_TEXT_BUFFER(widget))   {return "text_buffer";} 
	else if (GTK_IS_SCALE(widget))         {return "scale";} 
	else if (GTK_IS_COMBO_BOX_TEXT(widget)){return "combo_box_text";}
	else if (GTK_IS_COMBO_BOX(widget))     {return "combo_box";} 
	return "";
}

char* get_widget_label(GtkWidget* widget){
	char* label = NULL;
	if(GTK_IS_BUTTON(widget)){ 
		label = gtk_button_get_label(GTK_BUTTON(widget));	
	}
	if(GTK_IS_LABEL(widget)){
		label = gtk_label_get_label(GTK_LABEL(widget));
	}
	
	return label ? label : "";
}


