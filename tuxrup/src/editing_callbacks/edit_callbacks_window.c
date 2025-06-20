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
	g_print("computed hash for widget with pointer %p and callback %d, got %ld\n", info->id->widget, info->id->callback, info->hash);
    info->modified_code_path = g_strdup_printf("./runtime_files/%u.c", info->hash);
    info->shared_library_path = g_strdup_printf("./runtime_files/%u.so", info->hash);

    if(has_debugging_symbols()){
		g_print("original_function_pointer is %p\n", info->original_function_pointer);
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
bool first_child;
CXCursor pc = {0};
enum CXChildVisitResult make_copy_of_document_with_comment(CXCursor c, CXCursor parent, CXClientData data){
	GString* buffer = (GString*)data;
	
	if(!is_part_of_main_file(c))
	{return CXChildVisit_Continue;}

	// This is kind of so we can "look ahead"
	if(first_child){
		pc = c; 
		first_child = false;
	}

	enum CXCursorKind kind = clang_getCursorKind(pc);
	enum CXCursorKind n_kind = clang_getCursorKind(c);
	char* identifier = clang_getCString(clang_getCursorSpelling(pc));

	// Make sure no functions are static
	if(kind == CXCursor_FunctionDecl){
		char* fun_name = identifier; 
		char* fun_return_type = get_function_return_type(pc);
		char* fun_args = get_function_arguments(pc);

		char* fun_body;	
		if(edited_before){   		
			GString* body_buffer = g_string_new("");
			FILE* modified_code_file = fopen(cb_info->modified_code_path, "r");
			char line[1024];
			while(fgets(line, sizeof(line), modified_code_file)){
				g_string_append(body_buffer, line);
			}
			fun_body = body_buffer->str;
		} else {
			fun_body = get_function_code(pc);
		}

		g_print("identifier is %s, cb_info->function_name is %s\n", identifier, cb_info->function_name);
		if (strcmp(identifier, cb_info->function_name) == 0){
			g_string_append(buffer, "//Modify this function!!\n"); 
		} 

		g_string_append(buffer, g_strdup_printf("\
%s %s(%s){\n\
%s\n\
}\n", 
			fun_return_type, fun_name, fun_args, fun_body));	
		goto child_visit_end;
	}
	if(n_kind == CXCursor_TypedefDecl && 
		(kind == CXCursor_StructDecl ||
		 kind == CXCursor_EnumDecl || 
		 kind == CXCursor_UnexposedDecl ||
		 kind == CXCursor_UnionDecl)){

		CXType typedefType = clang_getTypedefDeclUnderlyingType(c);
		CXCursor declCursor = clang_getTypeDeclaration(typedefType);
		enum CXCursorKind declCursorKind = clang_getCursorKind(declCursor);
		if(declCursorKind == kind){
			goto child_visit_end;
		}
	} 
	
	// Make sure we dont get the horrid stderr bug
	CXString name = clang_getCursorSpelling(pc);
	if (strcmp(clang_getCString(name), "stderr") == 0 ||
		strcmp(clang_getCString(name), "stdin") == 0 ||
		strcmp(clang_getCString(name), "stdout") == 0) {
		goto child_visit_end;
	}

	write_cursor_element(&pc, buffer, true, true);

	child_visit_end:
	pc = c;
	return CXChildVisit_Continue;
}


void on_edit_callback_button(GtkWidget* widget, gpointer data){
	/* active_widget = widget; */
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

    if(info->original_function_pointer != NULL){
        edited_before = (info->dl_handle != NULL);
		CXCursor c = get_root_cursor(info->original_document_path);
		GString* buffer = g_string_new("");

		first_child = true;
		clang_visitChildren(c, make_copy_of_document_with_comment, buffer);
		char* edit_path = g_strdup_printf("%s/tuxrup_edit.c", program_src_folder);
		g_file_set_contents(edit_path, buffer->str, strlen(buffer->str), NULL);
        open_file_vscode(edit_path);
    } 
    else {
        open_file_vscode(info->modified_code_path);
    }
}


void on_edit_callback_done_button(GtkWidget* widget, gpointer data){
	/* active_widget = widget; */
    enum gtk_callback_category callback = *(enum gtk_callback_category*)data;
    callback_info* info = callback_map_get(active_widget, callback);

    if(info->original_function_pointer == NULL){ 
        // This callback is entirely new and created by the user
        compile_callback_file(info->modified_code_path, info->shared_library_path);
        set_widget_callback(info);
    } else {
		char* edit_path = g_strdup_printf("%s/tuxrup_edit.c", program_src_folder);
		char* command = g_strdup_printf("gcc $(pkg-config --cflags --libs gtk4) %s -shared -fPIC -g -o %s", edit_path, info->shared_library_path);
		int res = system(command);
		set_widget_callback(info);

		// save the modified code!
		CXCursor c = get_root_cursor(edit_path);
		CXCursor c_func = get_function_cursor(c, info->function_name);
		char* modified_code = get_function_code(c_func); 
		g_file_set_contents(info->modified_code_path, modified_code, strlen(modified_code), NULL);
    }
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
