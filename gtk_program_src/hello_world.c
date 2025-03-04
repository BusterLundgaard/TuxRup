#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

// typedef void (*callback_type)(GtkWidget*, gpointer);

// void free_custom_data(void* data){
//     dlclose(data);
// }

// GHashTable *dl_table; 

// void remove_click_signal (GtkWidget* w){
//     g_signal_handlers_disconnect_matched(
//         w, 
//         G_SIGNAL_MATCH_ID, 
//         g_signal_lookup("clicked", G_OBJECT_TYPE(w)), 
//         0, 
//         NULL, 
//         NULL, 
//         NULL);  
// }

// static void 
// function_dispatcher
// (GtkWidget* widget, gpointer data){

//   // Use the name id of the clicked widget to find the relevant .so file
//   void *handle = g_hash_table_lookup(dl_table, gtk_widget_get_name(widget));
//   if(!handle){
//       printf("Couldn't find a handle with this name in the dl_table");
//       fprintf(stderr, "Error: %s\n", dlerror());
//       return;
//   }

//   callback_type their_foo = (callback_type) dlsym(handle, "custom_callback");
//   char *error = dlerror();
//   if (error) {
//       printf("There was a problem in opening the so file");
//       fprintf(stderr, "Error: %s\n", error);
//       dlclose(handle);
//       return;
//   }

//   //Actually call their function here:
//   their_foo(widget, data);
// }

// static void generate_code_file(const gchar *new_code, const gchar *code_file_name){
//     FILE* fp = fopen(code_file_name, "w");
//     if(fp != NULL){
//       fputs(new_code, fp);
//       fclose(fp);
//     }
// }

// static void generate_code_file_from_template(const gchar* new_code, const gchar* code_file_name, const gchar* template){
//   FILE *template_file = fopen("template.c", "r");
//   if (!template_file) {
//       perror("Error opening template.c");
//       return;
//   }
  
//   FILE *output_file = fopen(code_file_name, "w");
//   if (!output_file) {
//       perror("Error creating new_code.c");
//       fclose(template_file);
//       return;
//   }
  
//   char line[1024];
//   while (fgets(line, sizeof(line), template_file)) {
//       if (strstr(line, "[%1]")) {
//           fprintf(output_file, "    %s\n", new_code);
//       } else {
//           fputs(line, output_file);
//       }
//   }
  
//   fclose(template_file);
//   fclose(output_file);
// }

// static void 
// modify_click
// (GtkWidget* widget, gchar* new_code){
//   remove_click_signal(widget);
//   generate_code_file(new_code, "new_function.c");
//   int compilation_result = system(g_strdup_printf("gcc $(pkg-config --cflags gtk4) -shared -fPIC -o %s_click_callback.so new_function.c $(pkg-config --libs gtk4)", gtk_widget_get_name(widget)));
//   if(compilation_result == 0){

//     void *handle = dlopen(g_strdup_printf("./%s_click_callback.so", gtk_widget_get_name(widget)), RTLD_LAZY);
//     if(!handle){
//         printf("There was a problem in opening the so file 1");
//         fprintf(stderr, "Error: %s\n", dlerror());
//         return;
//     }
//     g_hash_table_insert(dl_table, g_strdup_inline(gtk_widget_get_name(widget)), handle);

//     g_signal_connect(widget, "clicked", G_CALLBACK(function_dispatcher), NULL);
//   } else {
//     printf("Code could not succesfully compile unfortunately...\n");
//   }
// }

// static GtkWidget* 
// get_first_text_view
// (GtkWidget* root){
//   if(GTK_IS_TEXT_VIEW(root)){
//     return root;
//   }
//   GtkWidget* child = gtk_widget_get_first_child(root);
//   while(child){
//     GtkWidget* result = get_first_text_view(child);
//     if(result != NULL){
//       return result;
//     }
//     child = gtk_widget_get_next_sibling(child);
//   }
//   return NULL;
// }

// static void 
// done_button_callback 
// (GtkWidget* widget, gpointer data){
//   GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_native(widget));
//   GtkWidget* root_widget = gtk_window_get_child(GTK_WINDOW(parent_window));

//   GtkWidget* button_to_modify = GTK_WIDGET(data);

//   GtkWidget* text_view = get_first_text_view(root_widget);
//   GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
//   GtkTextIter start, end;
//   gtk_text_buffer_get_start_iter(buffer, &start);
//   gtk_text_buffer_get_end_iter(buffer, &end);
//   char* text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);  
//   modify_click(button_to_modify, text);
//   g_free(text);

//   gtk_window_close(parent_window);
// }

// struct source_code_location{
//   gchar* document_path;
//   gchar* before_code;
//   gchar* function_code;
//   gchar* after_code;
// };

// void print_stack_trace() {
//     void *buffer[10];
//     int size = backtrace(buffer, 10);

//     printf("Call Stack:\n");

//     for (int i = 0; i < size; i++) {
//         Dl_info info;
//         if (dladdr(buffer[i], &info) && info.dli_sname) {
//             printf("%d: %s (%p)\n", i, info.dli_sname, buffer[i]);
//         } else {
//             printf("%d: ??? (%p)\n", i, buffer[i]);
//         }
//     }
// }

// gpointer get_clicked_callback_from_widget(GtkWidget* widget){
//   // gtk_button_clicked(GTK_BUTTON(widget));
//   // print_stack_trace();
//   // something something
// }

// gchar* get_function_name_from_pointer(gpointer pointer){

// }

// struct source_code_location get_source_code_location_from_funtion_name(gchar* function_name){

// }

// struct source_code_location get_source_code_location(GtkWidget* widget){
//   // Get function pointer
//   // Get name from function pointer
//   // Find file and location in file using ctags
// }

// void my_funny_function(){
//   printf("wow what a funny function right?\n");
// }

// // Right click on button
// static void show_code_editor(GtkGestureClick *gesture,int, double x, double y, gpointer* self){
    
//     GtkWidget *parent_widget = GTK_WIDGET(self);  // The widget that was clicked
//     GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_native(parent_widget));  // Get the parent window
//     GtkApplication *app = GTK_APPLICATION(gtk_window_get_application(parent_window)); // Get the application

//     if (!app) {
//         g_print("Error: Could not retrieve GtkApplication!\n");
//         return;
//     }

//     struct source_code_location src = get_source_code_location(parent_widget);
  
//     GtkWidget* window = gtk_application_window_new (app);
//     gtk_window_set_title (GTK_WINDOW (window), "Hello");
//     gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
//     gtk_window_present (GTK_WINDOW (window));

//     GtkWidget* scrolled_window = gtk_scrolled_window_new();
//     gtk_widget_set_size_request(scrolled_window, 380, 280);
//     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

//     GtkWidget* text_view = gtk_text_view_new();
//     gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);  // Enable word wrapping
//     gtk_widget_set_vexpand(text_view, TRUE);
//     gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_view);
    
//     gchar* contents = NULL;
//     gsize length = 0;
//     GError **error;
//     if(!g_file_get_contents("template.c",&contents, &length, error)){
//       printf("An issue occured in opening/reading the template.c file\n");
//       return;
//     }
//     gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view)), contents, length);
    
//     GtkWidget* done_button = gtk_button_new_with_label (g_strdup_printf("Modify %s", gtk_widget_get_name(parent_widget)));
//     g_signal_connect(done_button, "clicked", G_CALLBACK(done_button_callback), parent_widget);

//     // TEST
//     //button_A_callback(NULL, NULL);
//     //gtk_button_clicked(GTK_BUTTON(parent_widget));
    
//     //my_funny_function();
//     //print_stack_trace();
//     //backtrace_symbols_fd(funptr, 1, 1);

//     my_funny_function();
//     const void* funptr = (void*)&my_funny_function;
//     Dl_info info;
//     dladdr(funptr, &info);
//     printf("the functions name is: %s\n", info.dli_sname);
//     //printf("backtrace_stuff[0]= %s", backtrace_stuff[0]);

//     // TEST


//     GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
//     gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
//     gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
//     gtk_box_append(GTK_BOX(box), scrolled_window);
//     gtk_box_append(GTK_BOX(box), done_button);
    
//     gtk_window_set_child(GTK_WINDOW(window), box);
//     gtk_window_present(GTK_WINDOW(window));
// }

// void find_buttons(GtkWidget* widget, GPtrArray* buttons){
//   if(GTK_IS_BUTTON(widget)){
//     g_ptr_array_add(buttons, widget);
//   }
//   GtkWidget* child = gtk_widget_get_first_child(widget);
//   while(child){
//     find_buttons(child, buttons);
//     child = gtk_widget_get_next_sibling(child);
//   }
// }

// static void add_menu_to_all_buttons(GtkWidget* root_widget){
//     GPtrArray* buttons = g_ptr_array_new();
//     find_buttons(root_widget, buttons);

//     GtkGesture* gesture; 
    
//     for(int i = 0; i < buttons->len; i++){
//       gesture = gtk_gesture_click_new();
//       gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3);
//       gtk_widget_add_controller(GTK_WIDGET(g_ptr_array_index(buttons, i)), GTK_EVENT_CONTROLLER(gesture));
//       g_signal_connect(gesture, "pressed", G_CALLBACK(show_code_editor), g_ptr_array_index(buttons, i));
//     }
// }

// static void 
// close_shared_libraries_on_exit(
// GtkWidget* window, 
// GdkEvent *event, 
// gpointer data
// ){
//   g_hash_table_destroy(dl_table);
// }

// static void our_init(GtkWindow* window){
//   dl_table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free_custom_data);

//   GtkWidget* root_widget = gtk_window_get_child(GTK_WINDOW(window));
//   add_menu_to_all_buttons(root_widget);
// }


// ORIGINAL APPLICATION
// ===========================================================================================


#define stupid_ass_macro char**

char** second_var = NULL;
void (*my_func_ptr)         (int, stupid_ass_macro);

char* some_complicated_function(
  int ass, 
  stupid_ass_macro ass2, 
  gpointer ass3){}

int some_original_variable = 20;
void some_original_function      (int var) {
  printf("Hi! I am some_original_function and now i will print: %d", var);
}

struct my_epic_struct{
  int wow;
  long int more_wow;
};

typedef int my_own_int;

static void
button_A_callback 
(GtkWidget *widget,
 gpointer   data)
{
  some_original_variable++;
  some_original_function(some_original_variable);
}

static void
button_B_callback 
(GtkWidget *widget,
 gpointer   data)
{
  g_print ("This is the original callback for button B\n");
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button_A;
  GtkWidget *button_B;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Hello");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  button_A = gtk_button_new_with_label ("Button_A");
  gtk_widget_set_name(button_A, "button_A");
  g_signal_connect (button_A, "clicked", G_CALLBACK (button_A_callback), NULL);

  button_B = gtk_button_new_with_label ("Button_B");
  gtk_widget_set_name(button_B, "button_B");
  g_signal_connect (button_B, "clicked", G_CALLBACK (button_B_callback), NULL);

  // GtkGesture* gesture = gtk_gesture_click_new();
  // gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3); //Maybe 3 needs to be changed to something else if right-click isn't working?
  // gtk_widget_add_controller(GTK_WIDGET(button_A), GTK_EVENT_CONTROLLER(gesture));
  // g_signal_connect(gesture, "pressed", G_CALLBACK(show_code_editor), button_A);

  // gesture = gtk_gesture_click_new();
  // gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(gesture),3); 
  // gtk_widget_add_controller(GTK_WIDGET(button_B), GTK_EVENT_CONTROLLER(gesture));
  // g_signal_connect(gesture, "pressed", G_CALLBACK(show_code_editor), button_B);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

  gtk_box_append(GTK_BOX(box), button_A);
  gtk_box_append(GTK_BOX(box), button_B);

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
