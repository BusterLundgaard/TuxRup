#include <gtk-3.0/gtk/gtk.h>
#include <gdk/gdk.h>

#include "globals.h"
#include "image_viewer.h"
#include "layers.h"
#include "toolbar.h"
#include "toolpicker.h"

static void set_css(GtkWidget* widget){
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, ".selected { color: red; }", -1, NULL);
	GdkScreen* screen = gtk_widget_get_screen(widget); 
	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	g_object_unref(provider);
}

static void activate ( GApplication *app, G_GNUC_UNUSED gpointer *data )
{
    GtkWidget* win = gtk_application_window_new ( GTK_APPLICATION ( app ) );
    gtk_window_set_title ( GTK_WINDOW ( win ), "IRC Client" );
    gtk_window_set_default_size ( GTK_WINDOW ( win ), 400, 400 );

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(win), vbox);

	GtkWidget* toolbar = create_toolbar(app);
	gtk_container_add(GTK_CONTAINER(vbox), toolbar);

    // Main horizontal split (A and B)
    GtkWidget* main_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand(main_paned, TRUE);
    gtk_widget_set_vexpand(main_paned, TRUE);
	gtk_container_add(GTK_CONTAINER(vbox), main_paned);

    GtkWidget* left_paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_hexpand(left_paned, TRUE);
    gtk_widget_set_vexpand(left_paned, TRUE);
	gtk_paned_add1(GTK_PANED(main_paned), left_paned);

    // Part A
	GtkWidget* toolpicker = create_toolpicker();
	gtk_paned_add1(GTK_PANED(left_paned), toolpicker);

    GtkWidget* layerpicker = create_layerpicker();
	gtk_paned_add2(GTK_PANED(left_paned), layerpicker);

    // Part B
	GtkWidget* imageviewer = create_imageviewer();
	gtk_paned_add2(GTK_PANED(main_paned), imageviewer);

	// Finalize
	set_css(win);
	gtk_widget_show_all(win);
}

int main ( int argc, char **argv )
{
    GtkApplication *app;
    int stat;

    /// ***
    app = gtk_application_new ( "com.example.app", G_APPLICATION_FLAGS_NONE );
    g_signal_connect ( app, "activate", G_CALLBACK ( activate ), NULL );

    /// ***
    stat = g_application_run ( G_APPLICATION ( app ), argc, argv );
    g_object_unref ( app );

    /// ***
    return stat;
}
