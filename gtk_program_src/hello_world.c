#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>



#define stupid_ass_macro char**

char** second_var = NULL;
void (*my_func_ptr)         (int, stupid_ass_macro);

char* some_complicated_function(
  int ass, 
  stupid_ass_macro ass2, 
  gpointer ass3){}

int some_original_variable = 20;
void some_original_function(int var) {
  g_print("Hi! I am some_original_function and now i will print: %d\n", var);
}

typedef struct {
  int wow;
  long int more_wow;
} my_epic_struct;

typedef int my_own_int;

static void
button_A_callback 
(GtkWidget *widget,
 gpointer   data)
{
    some_original_variable++;
    my_own_int local_var_fuck_you = some_original_variable;
    some_original_function(local_var_fuck_you);
}

static void
button_B_callback 
(GtkWidget *widget,
 gpointer   data)
{
  some_original_function(some_original_variable);
}

static void show_popup_menu(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data) {
  GtkPopoverMenu* popover = GTK_POPOVER_MENU(user_data);
  gtk_popover_popup(GTK_POPOVER(popover));
}

static void on_menu_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    const gchar *action_name = g_action_get_name(G_ACTION(action));
    g_print("Selected action: %s\n", action_name);
}

static void add_right_click_menu_to_widget(GtkWidget* w){
  // Initialize the context menu and attach it to the right click action of the button
  GMenu* menu = g_menu_new();
  GtkWidget* popover = gtk_popover_menu_new_from_model(G_MENU_MODEL(menu));
  gtk_widget_set_parent(popover, w);

  GtkGesture* gesture = gtk_gesture_click_new();
  gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3); 
  //gtk_gesture_single_get_button()
  gtk_widget_add_controller(GTK_WIDGET(w), GTK_EVENT_CONTROLLER(gesture));
  
  //GObject* object = G_OBJECT(gesture);
  //GObjectClass* klass = G_OBJECT_GET_CLASS(object);
  //GTK_IS_GESTURE_CLICK()
  //GTK_IS_EVENT_C
  //GTK_IS_EVENT_CONTROLLER_MOTION()

  //gchar* gesture_type_name = G_OBJECT_CLASS_NAME(klass);
  //printf("This gesture is of type: %s\n", gesture_type_name);

  //GdkDevice* device = gtk_gesture_get_device(gesture);
  //g_print("the name of this device is: %s\n", gdk_device_get_name(device));
  //GtkWidget* the_widget = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

  g_signal_connect(gesture, "pressed", G_CALLBACK(show_popup_menu), popover);
  
  // Populate the menu with an item, two sections, and one submenu
  GMenuItem* item0 = g_menu_item_new("This is item0", "our_cool_app.item0");
  GMenu* menu_section1 = g_menu_new();
  GMenu* menu_section2 = g_menu_new();
  GMenu* submenu = g_menu_new();

  g_menu_append_item(menu, item0);
  g_menu_append_section(menu, "section1", G_MENU_MODEL(menu_section1));
  g_menu_append_section(menu, "section2", G_MENU_MODEL(menu_section2));
  g_menu_append_submenu(menu, "the first submenu", G_MENU_MODEL(submenu));

  // Populate those sections and the submenu with items
  GMenuItem* item1 = g_menu_item_new("This is item1", "our_cool_app.item1");
  GMenuItem* item2 = g_menu_item_new("This is item2", "our_cool_app.item2");
  GMenuItem* item3 = g_menu_item_new("This is item3", "our_cool_app.item3");
  GMenuItem* item4 = g_menu_item_new("This is item4", "our_cool_app.item4");
  g_menu_append_item(menu_section1, G_MENU_ITEM(item1));
  g_menu_append_item(menu_section1, G_MENU_ITEM(item2));
  g_menu_append_item(menu_section2, G_MENU_ITEM(item3));
  g_menu_append_item(submenu, G_MENU_ITEM(item4));

  // Add actions to those items
  GSimpleAction *action1 = g_simple_action_new("item1", NULL);
  GSimpleAction *action2 = g_simple_action_new("item2", NULL);
  g_signal_connect(action1, "activate", G_CALLBACK(on_menu_action), NULL);
  g_signal_connect(action2, "activate", G_CALLBACK(on_menu_action), NULL);
  GSimpleActionGroup *action_group = g_simple_action_group_new();
  g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(action1));
  g_action_map_add_action(G_ACTION_MAP(action_group), G_ACTION(action2));
  gtk_widget_insert_action_group(w, "our_cool_app", G_ACTION_GROUP(action_group));
}

static void create_and_add_input_items(GtkWidget* box){
  // 1. One-line Textbox (GtkEntry)
  GtkWidget *entry = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter text...");

  // 2. Number input (GtkSpinButton)
  GtkWidget *spin = gtk_spin_button_new_with_range(0, 100, 1); // Min: 0, Max: 100, Step: 1

  // 3. Radio button / true-false tick (GtkCheckButton)
  GtkWidget *check = gtk_check_button_new_with_label("Enable option");

  // 4. Dropdown
  const char* const dropdown_items[] = {"albert", "dagmar", "buster", NULL};  
  GtkWidget* dropdown = gtk_drop_down_new_from_strings(dropdown_items);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(dropdown), 0);
  
  // Add them:
  gtk_box_append(GTK_BOX(box), entry);
  gtk_box_append(GTK_BOX(box), spin);
  gtk_box_append(GTK_BOX(box), check);
  gtk_box_append(GTK_BOX(box), dropdown);
}

void list_enum_properties(GtkWidget *widget) {
    guint n_properties;
    GParamSpec **properties = g_object_class_list_properties(G_OBJECT_GET_CLASS(widget), &n_properties);

    for (guint i = 0; i < n_properties; i++) {
        GParamSpec *pspec = properties[i];

        // Get the GType of the property
        GType prop_type = G_PARAM_SPEC_VALUE_TYPE(pspec);

        // Check if it's an enum type
        if (G_TYPE_IS_ENUM(prop_type)) {
            GEnumClass *enum_class = g_type_class_ref(prop_type);
            g_print("Property: %s (Enum Type: %s)\n", pspec->name, g_type_name(prop_type));

            // Iterate over the enum values
            for (guint j = 0; j < enum_class->n_values; j++) {
                GEnumValue *value = &enum_class->values[j];
                g_print("  %d -> %s\n", value->value, value->value_nick);
            }

            g_type_class_unref(enum_class);
        }
    }

    g_free(properties);
}

static void list_button_properties(GtkWidget* button){
  GObject* object = G_OBJECT(button);
  GObjectClass* klass = G_OBJECT_GET_CLASS(G_OBJECT(button));
  
  // Get the list of properties
  guint n_properties;
  GParamSpec **properties = g_object_class_list_properties(klass, &n_properties);

  printf("Properties of %s:\n", G_OBJECT_TYPE_NAME(object));
  for (guint i = 0; i < n_properties; i++) {
      GParamSpec *pspec = properties[i];
      printf(" - %s (Type: %s)\n", 
              g_param_spec_get_name(pspec), 
              g_type_name(G_PARAM_SPEC_VALUE_TYPE(pspec)));
  }

  GValue new_label = G_VALUE_INIT;
  g_value_init(&new_label, G_TYPE_STRING);
  g_value_set_static_string(&new_label, "sus");
  g_object_set_property(object, "label", &new_label);

  g_free(properties);  // Must be freed after use
}

void apply_css_globally(const gchar *css_file_path)
{
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GFile *css_file = g_file_new_for_path(css_file_path);

    gchar *path = g_file_get_path(css_file);
    printf("The hello world program loads a css file with a path = %s\n", path);  

    gtk_css_provider_load_from_file(css_provider, css_file);
    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(css_provider);
    g_object_unref(css_file);
}


void on_added_to_dom(GtkWidget* widget, gpointer data){
  printf("yiha!\n");
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button_A;
  GtkWidget *button_B;
  GtkWidget* button_C;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "HelloWorldGtkApplication");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  button_A = gtk_button_new_with_label ("Button_A");
  gtk_widget_set_name(button_A, "button_A");
  g_signal_connect (button_A, "clicked", G_CALLBACK (button_A_callback), NULL);
  //add_right_click_menu_to_widget(button_A);

  button_B = gtk_button_new_with_label ("Button_B");
  gtk_widget_set_name(button_B, "button_B");
  g_signal_connect (button_B, "clicked", G_CALLBACK (button_B_callback), NULL);

  button_C = gtk_button_new_with_label ("Button_C");
  g_signal_connect (button_C, "notify::root", G_CALLBACK (on_added_to_dom), NULL);
  gtk_widget_set_name(button_C, "button_C");
  
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

  gtk_box_append(GTK_BOX(box), button_A);
  gtk_box_append(GTK_BOX(box), button_B);
  gtk_box_append(GTK_BOX(box), button_C);

  create_and_add_input_items(box);

  //list_button_properties(button_B);
  //list_enum_properties(button_B);

  apply_css_globally("example_css.css");

  gtk_window_set_child(GTK_WINDOW(window), box);

  gtk_window_present (GTK_WINDOW (window));
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
