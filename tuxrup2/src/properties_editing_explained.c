void on_spin_button_changed(GtkSpinButton* spin_button, gchar* property_name){
    double spin_button_value = gtk_spin_button_get_value(spin_button);

	//create a GValue we will set the property to
    GValue value = G_VALUE_INIT;
    g_value_init(&value, value_type); 
	g_value_set_int(&value, *(gint*)spin_button_value);
	
	g_object_set_property(G_OBJECT(active_widget), property_name, &value)
}
GtkWidget* create_number_editor(gchar* property_name, GType number_type){
    GtkWidget* spin = gtk_spin_button_new_with_range(INT_MIN, INT_MAX, 1);
    g_signal_connect(spin, "changed", G_CALLBACK(on_spin_button_changed), property_name);
    return spin;
}

void create_property_editor(GtkWidget* vbox, GParamSpec* pspec){
    GType typ = G_PARAM_SPEC_VALUE_TYPE(pspec);
    char* property_type = g_type_name(typ);
    char* property_name = g_param_spec_get_name(pspec);

    if     (g_strcmp0(property_type, "gint")       == 0) { return create_number_editor(property_name);   }
    else if(g_strcmp0(property_type, "gchararray") == 0) { return create_string_editor(property_name);   }
    else if(g_strcmp0(property_type, "gboolean")   == 0) { return create_boolean_editor(property_name);  }
    else if(G_TYPE_IS_ENUM(typ)){                          return create_enum_editor(property_name, typ);}
}

void on_edit_properties(GtkWidget* widget, gpointer user_data){
	// Get the class of the active widget
    GObjectClass* klass = G_OBJECT_GET_CLASS(G_OBJECT(active_widget));
	
	// Iterate through list of all properties of this class 
    guint properties_n;
    GParamSpec **properties = g_object_class_list_properties(klass, &properties_n);
    for (guint i = 0; i < properties_n; i++) {
        create_property_editor(property_editor, properties[i]);
		// ... add to the actual window here  
    } 
}
