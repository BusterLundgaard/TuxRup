void make_children_modifiable(GtkWidget* widget){
	if(editable_type(widget)){
		gtk_widget_add_events(widget, GDK_BUTTON_PRESS_MASK);
		g_signal_connect(widget, "button-press-event", G_CALLBACK(on_widget_click));
	}
	
	if(!GTK_IS_CONTAINER(widget)){return;}
	GList *children = gtk_container_get_children(GTK_CONTAINER(widget));
	for (GList *l = children; l; l = l->next){
		find_all_modifiable_children(GTK_WIDGET(l->data));
	}
}

GList* make_widgets_modifiable(){
	// We use our refference to the application (application_root) to get a list of windows
	GList* application_windows = gtk_application_get_windows(gtk_window_get_application(GTK_WINDOW(application_root)));
	for(GList* elem = application_windows; elem; elem = elem->next){
		GtkWidget* root_widget = GTK_WIDGET((GtkWindow*)elem->data);
		find_all_modifiable_children(root_widget);
	}
}
