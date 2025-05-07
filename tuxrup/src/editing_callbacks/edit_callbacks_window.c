#include "edit_callbacks_window.h"

#include "document_parsing.h"
#include "fix_function_body.h"
#include "reference_type.h"

#include "../globals.h"
#include "../callback_map.h"
#include "../function_dispatcher.h"

#include "../utilities/util.h"
#include "../utilities/hashing.h"
#include "../utilities/pointer_name_conversion.h"

#include "../method_intercepting/hooks.h"

#include <string.h>
#include <dlfcn.h>

// =====================================================
// GLOBALS AND TYPEDEFS
// =====================================================
static GtkWidget* active_widget = NULL;

typedef struct {
    enum gtk_callback_category callbacks[MAPPABLE_ACTIONS_LEN];
    int size;
} callbacks_list;

static callbacks_list applicable_callbacks;

callback_info* cb_info; // since it used everywhere
bool edited_before;
GHashTable* all_variables;
GHashTable* document_symbols;

// ======================================================
// GENERAL UTILITY METHODS
// ======================================================
// Gets all the callbacks that can possibly be applied to a given widget (looks at its categories and sees what callbacks we currently support editing for this widget category)
callbacks_list set_applicable_callbacks_from_active_widget(){
    enum widget_type_category active_widget_category = get_widget_type_category(active_widget);
    applicable_callbacks = (callbacks_list){.callbacks = {}, .size=0};
   
    for(int i = 0; i < MAPPABLE_ACTIONS_LEN; i++){
        int j = 0;
        while(remapable_events[i].valid_widget_types[j] != GTK_CATEGORY_UNDEFINED){
            if(remapable_events[i].valid_widget_types[j] == active_widget_category){
                applicable_callbacks.callbacks[applicable_callbacks.size] = remapable_events[i].action_name;
                applicable_callbacks.size++;
                break;
            }
            j++;
        }
    }
}

static void create_empty_callback_file(guint hash){
    copy_file("../templates/new_callback.c", g_strdup_printf("./runtime_files/%u.c", hash));
}

static FILE* open_file_vscode(char* path){
    char* command = g_strdup_printf("nano %s", path);
    system(command);
}

static void compile_callback_file(char* c_file_path, char* shared_library_path){
	char* compile_command = g_strdup_printf("gcc %s -g -fPIC -shared -O1 -o %s", c_file_path, shared_library_path);  

    // Run the compilation command
    int ret = system(compile_command);
    if (ret != 0) {
        g_warning("Failed to compile %s\n", shared_library_path);
    }

    // Clean up
    g_free(compile_command);
}


// ========================================================
// UPDATING AND SETTING CALLBACK INFORMATION
// =======================================================
static void set_widget_callback(callback_info* info){
    remove_callback(info->id->widget, info->id->callback);
    if(info->dl_handle != NULL){
        dlclose(info->dl_handle);
        info->dl_handle = NULL;
    }
    g_signal_connect_data_ORIGINAL(
        info->id->widget, 
        get_callback_type_category_str(info->id->callback),
        G_CALLBACK(function_dispatcher), 
        &(info->id->callback), 
        NULL, (GConnectFlags)0);
    info->dl_handle = dlopen(info->shared_library_path, RTLD_LAZY);
}


char* set_callback_code_information(callback_info* info){
    info->hash = compute_callback_hash(info->id->widget, info->id->callback);
    info->modified_code_path = g_strdup_printf("./runtime_files/%u.c", info->hash);
    info->shared_library_path = g_strdup_printf("./runtime_files/%u.so", info->hash);

    if(has_debugging_symbols()){
        info->function_name = get_identifier_from_pointer(info->original_function_pointer);
    } else {
        printf("Not enough debug information in executable to be able to edit callbacks!\n");
        return NULL;
    }

    if(info->original_function_pointer != NULL){
        info->original_document_path = get_document_path(info->function_name);
    }
	cb_info = info;
}


// ==============================================================
// ON EDITING CALLBACKS
// ==============================================================
enum CXChildVisitResult make_copy_of_document_with_comment(CXCursor c, CXCursor parent, CXClientData data){
	GString* buffer = (GString*)data;
	
	if(!is_part_of_main_file(c))
	{return CXChildVisit_Continue;}
	g_print("got through the main file check!\n");

	enum CXCursorKind kind = clang_getCursorKind(c);
	char* identifier = clang_getCString(clang_getCursorSpelling(c));

	if(kind == CXCursor_FunctionDecl && strcmp(identifier, cb_info->function_name) == 0){	
		if(edited_before){
			char* fun_name = identifier; 
			char* fun_return_type = get_function_return_type(c);
			char* fun_args = get_function_arguments(c);
       		
		   	g_string_append(buffer, g_strdup_printf("%s %s(%s){\n", fun_return_type, fun_name, fun_args));	
			FILE* modified_code_file = fopen(cb_info->modified_code_path, "r");
        	char line[1024];
        	while(fgets(line, sizeof(line), modified_code_file)){
           	 	g_string_append(buffer, line);
        	}
			return CXChildVisit_Continue;
		} else {
			g_string_append(buffer, "// Modify this function! \n\n");
		}
	}
	// Skip non-defining declarations of structs (avoids duplicate printing)
		CXString name = clang_getCursorSpelling(c);
		if (strcmp(clang_getCString(name), "stderr") == 0 ||
			strcmp(clang_getCString(name), "stdin") == 0 ||
			strcmp(clang_getCString(name), "stdout") == 0) {
			clang_disposeString(name);
			return CXChildVisit_Continue;
		}
		clang_disposeString(name);

	write_cursor_element(&c, buffer, true, true);
	return CXChildVisit_Continue;
}


static void on_edit_callback_button(GtkWidget* widget, gpointer data){
    enum gtk_callback_category callback = *(enum gtk_callback_category*)data;
    
    // If creating completely new callback: 
    if(!callback_map_exists(active_widget, callback)){
        callback_info* info = callback_map_add_new(active_widget, callback);
        set_callback_code_information(info);
        copy_file("../tempplates/empty_modified_callback.c", info->modified_code_path);
        open_file_vscode(info->modified_code_path);
        return;
    }

    callback_info* info = callback_map_get(active_widget, callback);
    set_callback_code_information(info);

    g_print("you are editing a callback with the pointer %p\n", info->original_function_pointer);
    g_print("name of function at this pointer is %s \n", info->function_name);

    if(info->original_function_pointer != NULL){
        edited_before = (info->dl_handle != NULL);
		CXCursor c = get_root_cursor(info->original_document_path);
		GString* buffer = g_string_new("");
		clang_visitChildren(c, make_copy_of_document_with_comment, buffer);
		g_file_set_contents("./runtime_files/edit.c", buffer->str, strlen(buffer->str), NULL);
        open_file_vscode("./runtime_files/edit.c");
    } 
    else {
        open_file_vscode(info->modified_code_path);
    }
}


// ==============================================================
// ON FINALIZING THE EDITED CALLBACK / EDITING DONE
// ==============================================================
enum CXChildVisitResult create_isolated_function(CXCursor c, CXCursor parent, CXClientData data){
	GString* buffer = (GString*)data;
	if(!is_part_of_main_file(c))
	{return CXChildVisit_Continue;}
	
	enum CXCursorKind kind = clang_getCursorKind(c);
	char* identifier = clang_getCString(clang_getCursorSpelling(c));

	if(kind == CXCursor_FunctionDecl){
		if(!g_hash_table_contains(document_symbols, identifier)){
			return CXChildVisit_Continue;
		}
		char* fun_name = identifier; 
		char* fun_return_type = get_function_return_type(c);
		char* fun_args = get_function_arguments(c);
		char* fun_arg_names = get_function_argument_names(c);
		
		// For the modified function
		if(strcmp(identifier, cb_info->function_name) == 0){
			char* fun_body = get_function_code(c); 
			g_print("writing the target modified function!!\n");
			g_string_append(buffer, g_strdup_printf("\
						%s %s(%s){\n\
						sync_shared_variables_to_main(false);\n\
						%s\n\
						sync_shared_variables_from_main(true);\n\
						}\n\
						", fun_return_type, fun_name, fun_args, fun_body));
		}	
		// For all other functions
		else{
			g_string_append(buffer, g_strdup_printf("\
						__attribute__((noinline)) static %s %s(%s){\n \
						typeof(%s) *%s_fucking = get_pointer_from_identifier(\"%s\");\n \
						%s_fucking(%s);\n \
						}\n", fun_return_type, fun_name, fun_args, fun_name, fun_name, fun_name, fun_name, fun_arg_names));
		}
	}
	else if(kind == CXCursor_VarDecl){
		if(!g_hash_table_contains(document_symbols, identifier)){
			return CXChildVisit_Continue;
		}
		char* var_name = identifier; 
		char* var_type = get_variable_type(c); 
		g_hash_table_insert(all_variables, var_name, NULL);
		g_string_append(buffer, g_strdup_printf("\
					%s %s = 0;\n\
					", var_type, var_name));

	}
	else if(kind != CXCursor_StructDecl &&
			kind != CXCursor_EnumDecl && 
			kind != CXCursor_UnexposedDecl &&
			kind != CXCursor_UnionDecl
			){
		/* g_print("this cursor is a: %s\n", clang_getCString(clang_getCursorKindSpelling(kind))); */
		write_cursor_element(&c, buffer, true, true);
	}

	return CXChildVisit_Continue;
}


static void on_edit_callback_done_button(GtkWidget* widget, gpointer data){
    enum gtk_callback_category callback = *(enum gtk_callback_category*)data;
    callback_info* info = callback_map_get(active_widget, callback);

    if(info->original_function_pointer == NULL){ 
        // This callback is entirely new and created by the user
        compile_callback_file(info->modified_code_path, info->shared_library_path);
        set_widget_callback(info);
    } else { 
		int res = system("gcc $(pkg-config --cflags --libs gtk4) ./runtime_files/edit.c -E -P -o ./runtime_files/edit_expanded.c");
		CXCursor c = get_root_cursor("./runtime_files/edit_expanded.c");
		GString* buffer = g_string_new("#include \"../../src/utilities/pointer_name_conversion.h\"\n");
		all_variables = g_hash_table_new(g_str_hash, g_str_equal);
		clang_visitChildren(c, create_isolated_function, buffer);
		
		g_print("size of all_variables is %d\n", g_hash_table_size(all_variables));
		GHashTableIter iter;
		gpointer key;
		g_hash_table_iter_init(&iter, all_variables);
		g_string_append(buffer, "void dependency(){\n");
		while (g_hash_table_iter_next(&iter, &key, NULL)) {
			g_string_append(buffer, g_strdup_printf("%s = 0;\n", (char*)key));
		}
		g_string_append(buffer, "}");
		g_file_set_contents("./runtime_files/temp.c", buffer->str, strlen(buffer->str), NULL);

        compile_callback_file("./runtime_files/temp.c", info->shared_library_path);
        set_widget_callback(info);
    }
}

void testing_create_isolated_function(){
	system("gcc -g -c ./runtime_files/edit.c $(pkg-config --cflags --libs gtk4) -o ./runtime_files/object.o");
	document_symbols = get_identifiers("./runtime_files/object.o");

	int res = system("gcc $(pkg-config --cflags --libs gtk4) ./runtime_files/edit.c -E -P -o ./runtime_files/edit_expanded.c");
	CXCursor c = get_root_cursor("./runtime_files/edit_expanded.c");
	GString* buffer = g_string_new("#include \"../../src/utilities/pointer_name_conversion.h\"\n");
	all_variables = g_hash_table_new(g_str_hash, g_str_equal);
	cb_info = malloc(sizeof(callback_info));
	cb_info->function_name = "button_A_callback";
	clang_visitChildren(c, create_isolated_function, buffer);

	g_print("size of all_variables is %d\n", g_hash_table_size(all_variables));
	GHashTableIter iter;
	gpointer key;
	g_hash_table_iter_init(&iter, all_variables);
	g_string_append(buffer, "void dependency(){\n");
	while (g_hash_table_iter_next(&iter, &key, NULL)) {
		g_string_append(buffer, g_strdup_printf("%s = 0;\n", (char*)key));
	}
	g_string_append(buffer, "}");
	g_file_set_contents("./runtime_files/temp.c", buffer->str, strlen(buffer->str), NULL);
	g_file_set_contents("./runtime_files/temp.c", buffer->str, strlen(buffer->str), NULL);
}


// ===========================================================================
// BUILDING THE WINDOW
// ===========================================================================
GtkWidget* create_edit_callback_field(enum gtk_callback_category* callback){
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);;
    gtk_widget_set_halign(hbox, GTK_ALIGN_END);

    GtkWidget* edit_button = gtk_button_new_with_label(callback_map_exists(active_widget, *callback) ? "Edit" : "Create");
    g_signal_connect_data_ORIGINAL(edit_button, "clicked", G_CALLBACK(on_edit_callback_button), callback, NULL, (GConnectFlags)0);

    GtkWidget* edit_done_button = gtk_button_new_with_label("done editing");
    g_signal_connect_data_ORIGINAL(edit_done_button, "clicked", G_CALLBACK(on_edit_callback_done_button), callback, NULL, (GConnectFlags)0);

    add_widget_to_box(hbox, edit_button);
    add_widget_to_box(hbox, edit_done_button);
    
    GtkWidget* label = gtk_label_new(get_callback_type_category_str(*callback));
    add_widget_to_box(hbox, label);

    return hbox;
}


void set_source_code_location(GtkEntry* entry, gpointer user_data){
    GtkEntryBuffer* buffer = gtk_entry_get_buffer(entry);
    const char* current_text = gtk_entry_buffer_get_text(buffer);
    program_src_folder = current_text;
}
void set_debug_location(GtkEntry* entry, gpointer user_data){
    GtkEntryBuffer* buffer = gtk_entry_get_buffer(entry);
    const char* current_text = gtk_entry_buffer_get_text(buffer);
    debug_symbols_path = current_text;
}

void build_edit_callbacks_window(GtkWidget* window){
    GtkWidget* vbox = create_and_add_scrollable_item_list(window, 200, 750);
    set_applicable_callbacks_from_active_widget();

    for(int i = 0; i < applicable_callbacks.size; i++){
        GtkWidget* edit_callback_field = create_edit_callback_field(&applicable_callbacks.callbacks[i]);
        #ifdef USE_GTK3
        gtk_container_add(GTK_CONTAINER(vbox), edit_callback_field);
        #else
        gtk_box_append(GTK_BOX(vbox), edit_callback_field);
        #endif
    }
}


// ======================================================================================
// OPEN / CLOSE
// ======================================================================================
gboolean cleanup_callback_editor_window(GtkWidget* closed_window, gpointer user_data){
    // Cleanup here
}

void open_edit_callbacks_window(GtkWidget* widget) {
    active_widget = widget;   
    GtkWidget* window = create_window(widget, "Edit widget properties", 200, 800);

    build_edit_callbacks_window(window); 
   
    g_signal_connect_data_ORIGINAL(window, "destroy", G_CALLBACK(cleanup_callback_editor_window), NULL, NULL, (GConnectFlags)0);    gtk_window_present(GTK_WINDOW(window));
#ifdef USE_GTK3
	gtk_widget_show_all_ORIGINAL(window);
#else
	gtk_window_present_ORIGINAL(GTK_WINDOW(window));
#endif
}
