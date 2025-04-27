#define _GNU_SOURCE
#ifdef USE_GTK3
    #include <gtk-3.0/gtk/gtk.h>
    #include <gdk/gdk.h>
#else
    #include <gtk-4.0/gtk/gtk.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#define mymacrotype(t) t**
#define mymacroexp(varname) varname+2
typedef int my_own_int;
typedef int(*foo_type)(int);

typedef struct {
  my_own_int some_int;
  mymacrotype(char) some_char_pointer;
} my_struct;

my_own_int some_int = 20;
int some_function(int x){
	return x-1;
}

static void
button_A_callback 
(GtkWidget *widget,
 gpointer   data)
{
	foo_type foo = &some_function;
	int local_var = mymacroexp(some_int);
	some_int = foo(local_var);
}

static void
button_B_callback 
(GtkWidget *widget,
 gpointer   data)
{
	g_print("some_int = %d\n", some_int);
}

void apply_css_globally(const gchar *css_file_path)
{
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GFile *css_file = g_file_new_for_path(css_file_path);

    gchar *path = g_file_get_path(css_file);
    g_print("The hello world program loads a css file with a path = %s\n", path);  

#ifdef USE_GTK3	
    gtk_css_provider_load_from_file(css_provider, css_file, NULL);
    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
#else
    gtk_css_provider_load_from_file(css_provider, css_file);
    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
#endif

    g_object_unref(css_provider);
    g_object_unref(css_file);
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *box;
  GtkWidget *button_A;
  GtkWidget *button_B;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "HelloWorldGtkApplication");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

  button_A = gtk_button_new_with_label ("Button_A");
  gtk_widget_set_name(button_A, "button_A");
  g_signal_connect (button_A, "clicked", G_CALLBACK (button_A_callback), NULL);

  button_B = gtk_button_new_with_label ("Button_B");
  gtk_widget_set_name(button_B, "button_B");
  g_signal_connect (button_B, "clicked", G_CALLBACK (button_B_callback), NULL);
  
#ifdef USE_GTK3
  gtk_container_add(GTK_CONTAINER(box), button_A);
  gtk_container_add(GTK_CONTAINER(box), button_B);
  gtk_container_add(GTK_CONTAINER(window), box);	
#else
  gtk_box_append(GTK_BOX(box), button_A);
  gtk_box_append(GTK_BOX(box), button_B);
  gtk_window_set_child(GTK_WINDOW(window), box);
#endif

  apply_css_globally("example_css.css");

#ifdef USE_GTK3
  gtk_widget_show_all(window);
#else
  gtk_window_present (GTK_WINDOW (window));
#endif
}

int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    int status;
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
