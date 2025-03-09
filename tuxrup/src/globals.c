#include "globals.h"

g_signal_connect_data_t normal_g_signal_connect_data = NULL;

GHashTable *widget_callback_table = NULL;
GHashTable *widget_hashes = NULL;

const char* working_directory = "";
const char* program_src_folder = "../../gtk_program_src";

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