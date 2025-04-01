#include "edit_properties_window.h"
#include "globals.h"
#include "util.h"
#include <limits.h>

// The widget we are changing
static GtkWidget* active_widget = NULL;

GValue get_current_value(gchar* property_name, GType value_type){
    GValue value = G_VALUE_INIT;
    g_value_init(&value, value_type); 
    g_object_get_property(G_OBJECT(active_widget), property_name, &value);
    return value;
}

void set_value(gchar* property_name, GType value_type, gpointer new_value){
    GValue value = G_VALUE_INIT;
    g_value_init(&value, value_type); 
    switch(value_type){
        case G_TYPE_INT: g_value_set_int(&value, *(gint*)new_value); break;
        case G_TYPE_UINT: g_value_set_uint(&value, *(guint*)new_value); break;
        case G_TYPE_BOOLEAN: g_value_set_boolean(&value, *(gboolean*)new_value); break;
        case G_TYPE_STRING: g_value_set_string(&value, (gchar*)new_value); break;
        case G_TYPE_ENUM: g_value_set_enum(&value, *(gint*)new_value); break;
    }
    g_object_set_property(G_OBJECT(active_widget), property_name, &value);
}


void on_check_button_changed(GtkCheckButton* check_button, gpointer user_data){
    gchar* property_name = (gchar*)user_data;
    gboolean current_boolean = gtk_check_button_get_active(check_button);
    set_value(property_name, G_TYPE_BOOLEAN, &current_boolean);
}
GtkWidget* create_boolean_editor(gchar* property_name){
    GValue current_value = get_current_value(property_name, G_TYPE_BOOLEAN);
    gboolean current_boolean = g_value_get_boolean(&current_value);

    GtkWidget* check = gtk_check_button_new();
    gtk_check_button_set_active(GTK_CHECK_BUTTON(check), current_boolean);
    normal_g_signal_connect_data(check, "toggled", G_CALLBACK(on_check_button_changed), property_name, NULL, (GConnectFlags)0);

    return check;
}


void on_spin_button_changed(GtkSpinButton* spin_button, gpointer user_data){
    gchar* property_name = (gchar*)user_data;
    double spin_button_value = gtk_spin_button_get_value(spin_button);
    set_value(property_name, G_TYPE_DOUBLE, &spin_button_value);
}
GtkWidget* create_number_editor(gchar* property_name, GType number_type){
    printf("Creating number editor for the property: %s\n", property_name);
    
    GValue current_value = get_current_value(property_name, number_type);
    
    gdouble current_number;
    double min, max, step;
    switch(number_type){
        case G_TYPE_INT: min=INT_MIN; max = INT_MAX; step=1; current_number=(gdouble)g_value_get_int(&current_value); break;
        case G_TYPE_UINT: min=-UINT_MAX; max = UINT_MAX, step=1; current_number=(gdouble)g_value_get_uint(&current_value); break;
        case G_TYPE_DOUBLE: min=-DBL_MAX, max=DBL_MAX; step = 0.01; current_number=g_value_get_double(&current_value); break;
    }

    GtkWidget* spin = gtk_spin_button_new_with_range(min, max, step);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), current_number);

    normal_g_signal_connect_data(spin, "changed", G_CALLBACK(on_spin_button_changed), property_name, NULL, (GConnectFlags)0);

    return spin;
}


void on_text_entry_changed(GtkEntry* entry, gpointer user_data){
    gchar* property_name = (gchar*)user_data;
    
    GtkEntryBuffer* buffer = gtk_entry_get_buffer(entry);
    const char* current_text = gtk_entry_buffer_get_text(buffer);
    set_value(property_name, G_TYPE_STRING, current_text);
}
GtkWidget* create_string_editor(gchar* property_name){
    GValue current_value = G_VALUE_INIT;
    g_value_init(&current_value, G_TYPE_STRING); 
    g_object_get_property(G_OBJECT(active_widget), property_name, &current_value);
    gchar* current_string = g_value_get_string(&current_value);
    if(current_string == NULL){
        current_string="";
    }

    GtkWidget *entry = gtk_entry_new();
    GtkEntryBuffer* buffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
    gtk_entry_buffer_set_text(buffer, current_string, strlen(current_string));
    
    normal_g_signal_connect_data(entry, "changed", G_CALLBACK(on_text_entry_changed), property_name, NULL, (GConnectFlags)0);

    return entry;
}


void on_dropdown_menu_changed(GtkDropDown* dropdown, GParamSpec *pspec, gpointer user_data){
    gchar* property_name = (gchar*)user_data;

    gint current_selected = gtk_drop_down_get_selected(dropdown);
    set_value(property_name, G_TYPE_ENUM, &current_selected);
}
GtkWidget* create_enum_editor(gchar* property_name, GType typ){
    GEnumClass* enum_class = g_type_class_ref(typ);

    GValue current_value = get_current_value(property_name, G_TYPE_ENUM);
    gint current_enum_value = g_value_get_enum(&current_value);

    const gchar** dropdown_items = malloc((enum_class->n_values+1)*sizeof(gchar*));
    dropdown_items[enum_class->n_values] = NULL; //Add NULL-terminator to list
    for(guint i = 0; i < enum_class->n_values; i++){
        GEnumValue *value = &enum_class->values[i];
        dropdown_items[i] = value->value_nick;
    }
  
    GtkWidget* dropdown = gtk_drop_down_new_from_strings(dropdown_items);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), current_enum_value);
    
    normal_g_signal_connect_data(dropdown, "notify::selected", G_CALLBACK(on_dropdown_menu_changed), property_name, NULL, (GConnectFlags)0);
    
    g_type_class_unref(enum_class);
    free(dropdown_items);

    return dropdown;
}


// Creates a field for editing a given widget property
void create_and_add_property_editor(GtkWidget* vbox, GParamSpec* pspec){
    if (!(pspec->flags & G_PARAM_WRITABLE)) 
    {return;} //Return if property isn't writeable

    GType typ = G_PARAM_SPEC_VALUE_TYPE(pspec);
    char* property_type = g_type_name(typ);
    char* property_name = g_param_spec_get_name(pspec);

    GtkWidget* editor_widget = NULL;
    if(g_strcmp0(property_type, "gint") == 0){editor_widget = create_number_editor(property_name, G_TYPE_INT);}
    else if(g_strcmp0(property_type, "guint") == 0){editor_widget = create_number_editor(property_name, G_TYPE_UINT);}
    else if(g_strcmp0(property_type, "gdouble") == 0){editor_widget = create_number_editor(property_name, G_TYPE_DOUBLE);}
    else if(g_strcmp0(property_type, "gchararray") == 0){editor_widget = create_string_editor(property_name);}
    else if(g_strcmp0(property_type, "gboolean") == 0){editor_widget = create_boolean_editor(property_name);}
    else if(G_TYPE_IS_ENUM(typ)){editor_widget = create_enum_editor(property_name, typ);}
    else 
    {return;}

    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);;
    GtkWidget* label = gtk_label_new(property_name);
    add_widget_to_box(hbox, label);
    add_widget_to_box(hbox, editor_widget);
    gtk_widget_set_halign(hbox, GTK_ALIGN_END);

    add_widget_to_box(vbox, hbox);
}


gboolean cleanup_property_editor_window(GtkWidget* closed_window, gpointer user_data){
    GParamSpec** properties = (GParamSpec**)user_data;
    free(properties);
    active_widget = NULL;
}

void open_edit_properties_window(GtkWidget* widget){
    //Create window with item list
    GtkWidget* window = create_window(widget, "Edit widget properties", 200, 800);
    GtkWidget* vbox = create_and_add_scrollable_item_list(window,200, 750);

    // Set active widget:
    active_widget = widget;   

    // Get widget properties
    GObjectClass* klass = get_widget_class(widget);
    guint properties_n;
    GParamSpec **properties = g_object_class_list_properties(klass, &properties_n);

    // Iterate through every widget property and add an entry to the list for editing this property (if it is of a valid type users should be able to edit like int, bool, string or enum)
    for (guint i = 0; i < properties_n; i++) {
        create_and_add_property_editor(vbox, properties[i]);
    } 

    normal_g_signal_connect_data(window, "destroy", G_CALLBACK(cleanup_property_editor_window), properties, NULL, (GConnectFlags)0);
    gtk_window_present(GTK_WINDOW(window));
}