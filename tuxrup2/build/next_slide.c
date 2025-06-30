		#include <gtk-3.0/gtk/gtk.h>
		void slide7(GtkWidget* slide_box){
			GtkWidget* title = gtk_label_new("Yeah!!!!!!");
			gtk_style_context_add_class(gtk_widget_get_style_context(title), "title");
			gtk_container_add(GTK_CONTAINER(slide_box), title);
		}