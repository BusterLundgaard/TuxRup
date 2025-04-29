#include <gtk/gtk.h>

typedef struct{
    char* menu_name;
    char* menu_items[5];
} Toolbar_menu_data;

Toolbar_menu_data menus[5] = {
    {"File",  {"Save", "Save As", "Import", "Export", "Quit"}},
    {"Edit",  {"Undo", "Redo", "Cut", "Copy", "Paste"}},
    {"Image", {"Resize", "Crop", "Rotate", "Flip", "Adjust"}},
    {"Layer", {"New Layer", "Duplicate Layer", "Merge Down", "Delete Layer", "Layer Properties"}},
    {"Help",  {"Documentation", "Keyboard Shortcuts", "Check for Updates", "Report Bug", "About"}},
};
char* icons[25] = {"audio-volume-high","audio-volume-low","audio-volume-medium","audio-volume-muted",
    "go-previous","go-next",
    "edit-cut","edit-find-replace","edit-paste","object-rotate-left","object-rotate-right","insert-image","insert-text","mail-attachment","call-stop","audio-input-microphone","input-tablet","x-office-calendar","face-cool","document-print","dialog-password","semi-starred","dialog-information","help-browser","contact-new"};

int move_left_tool = 4;
int move_right_tool = 5;

int layers[3] = {0, 1, 2};
GtkWidget* layer_buttons[3] = {};
int selected_layer = 0;

int tools[25] = {0};
int selected_tool_number = 0;
GtkWidget* selected_tool;

int images_numbers[3] = {0, 1, 2};
GtkWidget* images[3] = {};
int image_margins[3] = {0, 1, 2};

static void on_layer_select(GtkWidget* self, gpointer data){
    selected_layer = *((int*)data);
    for(int i = 0; i < 3; i++){
        GtkStyleContext *context = gtk_widget_get_style_context(layer_buttons[i]);
        if(i == selected_layer){
            gtk_style_context_add_class(context, "selected");
        } else {
            gtk_style_context_remove_class(context, "selected");
        }
    }
}

static void on_tool_select(GtkWidget* self, gpointer data){
    selected_tool_number = *((int*)data);
    if(selected_tool != NULL){
        gtk_style_context_remove_class(gtk_widget_get_style_context(selected_tool), "selected");
    }
    selected_tool = self;
    gtk_style_context_add_class(gtk_widget_get_style_context(selected_tool), "selected");
}

static void on_image_click(GtkGestureClick *gesture, gint n_press, gdouble x, gdouble y, gpointer user_data){
    int clicked_image = *((int*)user_data);
    if(clicked_image != selected_layer){return;}

    if(selected_tool_number == move_left_tool){
        image_margins[clicked_image] += 20;
        gtk_widget_set_margin_start(images[clicked_image], image_margins[selected_layer]);
    }
    else if(selected_tool_number == move_right_tool){
        image_margins[clicked_image] -= 20;
        gtk_widget_set_margin_start(images[clicked_image], image_margins[selected_layer]);
    }
}

static void on_zoom_changed(GtkSpinButton* spinbutton, gpointer data){
    double spin_button_value = gtk_spin_button_get_value(spinbutton);
    for (int i = 0; i < 3; i++) {
        gtk_image_set_pixel_size(GTK_IMAGE(images[i]), 300 + 50*spin_button_value);
    }
}

static void activate ( GApplication *app, G_GNUC_UNUSED gpointer *data )
{
    GtkWidget *main_paned;
    GtkWidget *left_paned;
    GtkWidget *partA1;
    GtkWidget *partA2;
    GtkWidget *spinbutton;
    GtkWidget *win;
    GtkWidget *vbox;
    GMenu *menu_bar;

    /// *** Window
    win = gtk_application_window_new ( GTK_APPLICATION ( app ) );
    gtk_window_set_title ( GTK_WINDOW ( win ), "IRC Client" );
    gtk_window_set_default_size ( GTK_WINDOW ( win ), 400, 400 );

    // *** Vbox
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(win), vbox);

    /// *** Menu Bar
    menu_bar = g_menu_new();
    gtk_application_set_menubar             ( GTK_APPLICATION ( app ), G_MENU_MODEL ( menu_bar ) );
    gtk_application_window_set_show_menubar ( GTK_APPLICATION_WINDOW ( win ), TRUE );

    for(int i = 0; i < 5; i++){
        GMenu* menu = g_menu_new();
        g_menu_append_submenu ( menu_bar, menus[i].menu_name, G_MENU_MODEL(menu));
    
        for(int j = 0; j < 5; j++){
            GMenuItem* menu_item = g_menu_item_new(menus[i].menu_items[j], "app.connect");
            g_menu_append_item(menu, menu_item);
        }
    }

    // Main horizontal split (A and B)
    main_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand(main_paned, TRUE);
    gtk_widget_set_vexpand(main_paned, TRUE);
    gtk_box_append(GTK_BOX(vbox), main_paned);

    // Left side (Part A)
    // ========================================================================
    left_paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_hexpand(left_paned, TRUE);
    gtk_widget_set_vexpand(left_paned, TRUE);
    gtk_paned_set_start_child(GTK_PANED(main_paned), left_paned);

    // Part A1
    // -------------------------------------------------------------------------------
    partA1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_vexpand(partA1, TRUE);
    gtk_paned_set_start_child(GTK_PANED(left_paned), partA1);

    // Buttons inside A1
    for(int i = 0; i < 5; i++){
        GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        for(int j = 0; j < 5; j++){
            tools[5*i + j] = 5*i + j;
            GtkWidget* tool = gtk_button_new();
            gtk_button_set_icon_name(GTK_BUTTON(tool), icons[i*5+j]);
            g_signal_connect(tool, "clicked", G_CALLBACK(on_tool_select), &tools[5*i+j]);
            gtk_box_append(GTK_BOX(hbox), tool);
        }
        gtk_box_append(GTK_BOX(partA1),hbox);
    }

    // -------------------------------------------------------------------------------

    // Part A2
    // -------------------------------------------------------------------------------
    partA2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_vexpand(partA2, TRUE);
    gtk_paned_set_end_child(GTK_PANED(left_paned), partA2);

    // Buttons inside A2
    layer_buttons[0] = gtk_button_new_with_label("Layer 1");
    layer_buttons[1] = gtk_button_new_with_label("Layer 2");
    layer_buttons[2] = gtk_button_new_with_label("Layer 3");
   
    g_signal_connect(layer_buttons[0], "clicked", G_CALLBACK(on_layer_select), &layers[0]);
    g_signal_connect(layer_buttons[1], "clicked", G_CALLBACK(on_layer_select), &layers[1]);
    g_signal_connect(layer_buttons[2], "clicked", G_CALLBACK(on_layer_select), &layers[2]);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        ".selected { color: red; }",
        -1);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);

    gtk_box_append(GTK_BOX(partA2), layer_buttons[0]);
    gtk_box_append(GTK_BOX(partA2), layer_buttons[1]);
    gtk_box_append(GTK_BOX(partA2), layer_buttons[2]);
    // -------------------------------------------------------------------------------
    // =================================================================================

    // Right side (Part B)
    // =================================================================================
    GtkWidget* right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_vexpand(right_box, true);
    gtk_paned_set_end_child(GTK_PANED(main_paned), right_box);

    // Images at the top of the right box
    GtkWidget *overlay = gtk_overlay_new();
    gtk_widget_set_halign(overlay, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(overlay, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(right_box), overlay); // Add the overlay to your layout box
    
    for (int i = 0; i < 3; i++) {
        char* image_name = g_strdup_printf("jump%d.png", i+1);
        images[i] = gtk_image_new_from_file(image_name);
        gtk_image_set_pixel_size(GTK_IMAGE(images[i]), 300);
    
        gtk_overlay_add_overlay(GTK_OVERLAY(overlay), images[i]);
    
        gtk_widget_set_halign(images[i], GTK_ALIGN_CENTER);
        gtk_widget_set_valign(images[i], GTK_ALIGN_CENTER);
    
        GtkGesture *gesture = gtk_gesture_click_new();
        gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture), 1);
        gtk_widget_add_controller(images[i], GTK_EVENT_CONTROLLER(gesture));
        g_signal_connect_data(gesture, "pressed", G_CALLBACK(on_image_click),
                              &images_numbers[i], NULL, (GConnectFlags)0);
    }
    
    // Spinbutton at the bottom of the box (fixed height)
    GtkAdjustment *adjustment = gtk_adjustment_new(0, 0, 10, 1, 10, 0);
    spinbutton = gtk_spin_button_new(adjustment, 1, 0);
    gtk_widget_set_size_request(spinbutton, -1, 40);  // Fixed height (40px)

    g_signal_connect(spinbutton, "changed", G_CALLBACK(on_zoom_changed), NULL);

    gtk_box_append(GTK_BOX(right_box), spinbutton);
    gtk_widget_set_valign(spinbutton, GTK_ALIGN_END);
    gtk_widget_set_vexpand(spinbutton, true);
    // ================================================================================

    gtk_window_present ( GTK_WINDOW ( win ) );
}

int main ( int argc, char **argv )
{
    GtkApplication *app;
    int stat;

    /// ***
    app = gtk_application_new ( "com.ircclient", G_APPLICATION_FLAGS_NONE );
    g_signal_connect ( app, "activate", G_CALLBACK ( activate ), NULL );

    /// ***
    stat = g_application_run ( G_APPLICATION ( app ), argc, argv );
    g_object_unref ( app );

    /// ***
    return stat;
}