#include "globals.h"
enum gtk_version gtk_ver = 0;

GtkWidget* application_root = NULL;
GHashTable *widget_callback_table = NULL;
GHashTable *computed_hashes = NULL;
GHashTable *known_widgets = NULL;

GHashTable *widget_to_css_filepath_map = NULL;

char* working_directory = NULL;
char* executable_path = NULL;
char* program_name = NULL;
char* program_src_folder = ""; //We need to fix this now! it should not be relative!
char* debug_symbols_path = NULL;

const action remapable_events[MAPPABLE_ACTIONS_LEN] = {
    {GTK_CALLBACK_clicked,               {GTK_CATEGORY_Button}},
    {GTK_CALLBACK_activate,              {GTK_CATEGORY_Button,       
                                          GTK_CATEGORY_Entry}},
    {GTK_CALLBACK_toggled,               {GTK_CATEGORY_CheckButton,  
                                          GTK_CATEGORY_ToggleButton}},
    {GTK_CALLBACK_value_changed,         {GTK_CATEGORY_SpinButton,   
                                          GTK_CATEGORY_Scale}},
    {GTK_CALLBACK_changed,               {GTK_CATEGORY_ComboBoxText, 
                                          GTK_CATEGORY_Entry, 
                                          GTK_CATEGORY_TextBuffer}},
    {GTK_CALLBACK_notify_property,       {GTK_CATEGORY_ANY}},
    
    {GTK_CALLBACK_pressed,               {GTK_CATEGORY_ANY}},

    {GTK_CALLBACK_insert_text,           {GTK_CATEGORY_Entry,        
                                          GTK_CATEGORY_TextBuffer}},
    {GTK_CALLBACK_delete_text,           {GTK_CATEGORY_Entry,        
                                          GTK_CATEGORY_TextBuffer}},
    {GTK_CALLBACK_backspace,             {GTK_CATEGORY_Entry}},

    {GTK_CALLBACK_size_allocate,         {GTK_CATEGORY_ANY}}, 
    {GTK_CALLBACK_map,                   {GTK_CATEGORY_ANY}}, 
    {GTK_CALLBACK_unmap,                 {GTK_CATEGORY_ANY}}, 

    {GTK_CALLBACK_drag_begin,            {GTK_CATEGORY_ANY}},
    {GTK_CALLBACK_drag_drop,             {GTK_CATEGORY_ANY}},
    {GTK_CALLBACK_drag_data_recieved,    {GTK_CATEGORY_ANY}},

    {GTK_CALLBACK_configure_event,       {GTK_CATEGORY_Window}}
 };

const source_code_info source_code_paths[30] = {
	{"hw", "../../gtk_program_src"},
	{"clock", "../../evaluation_applications/clock"},
	{"ow", "../../evaluation_applications/overwrites"},
	{"fg", "../../evaluation_applications/fake_gimp"},
	{"gnome-clocks", "/src/tuxrup_testing_applications_src/gnome-clocks"},
	{"fctix-configtool", "/src/tuxrup_testing_applications_src/fcitx-configtool"},
	{"quodlibet", "/src/tuxrup_testing_applications_src/quodlibet"},
	{"totem", "/src/tuxrup_testing_applications_src/totem"},
	{"xzgv", "/src/tuxrup_testing_applications_src/xzgv"}
};
