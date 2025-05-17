#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gtk-3.0/gtk/gtk.h>

#include "io.h"

char* program_src_folder = NULL;
char* working_directory = NULL;
char* executable_path = NULL;
char* executable_name = NULL;
char* executable_symbols_path = NULL;

GHashTable* source_code_paths = NULL;

char* get_program_src_folder(){
	if(program_src_folder != NULL){
		return program_src_folder;
	}

	if(source_code_paths == NULL){
		source_code_paths = g_hash_table_new(g_str_hash, g_str_equal);
		g_hash_table_insert(source_code_paths, "hw", "../../gtk_program_src"                                              );
		g_hash_table_insert(source_code_paths, "clock", "../../evaluation_applications/clock"                             );
		g_hash_table_insert(source_code_paths, "ow", "../../evaluation_applications/overwrites"                           );
		g_hash_table_insert(source_code_paths, "fg", "../../evaluation_applications/fake_gimp"                            );
		g_hash_table_insert(source_code_paths, "gnome-clocks", "/src/tuxrup_testing_applications_src/gnome-clocks"        );
		g_hash_table_insert(source_code_paths, "fctix-configtool", "/src/tuxrup_testing_applications_src/fcitx-configtool");
		g_hash_table_insert(source_code_paths, "quodlibet", "/src/tuxrup_testing_applications_src/quodlibet"              );
		g_hash_table_insert(source_code_paths, "totem", "/src/tuxrup_testing_applications_src/totem"                      );
		g_hash_table_insert(source_code_paths, "xzgv", "/src/tuxrup_testing_applications_src/xzgv"                        );
	}

	if(!g_hash_table_contains(source_code_paths, get_executable_name())){
		g_print("Could not find source code folder. Have you maybe forgotten to add it to the hardwired list?\n");		
		return "";
	}	

	program_src_folder = g_hash_table_lookup(source_code_paths, get_executable_name());
	return program_src_folder;
}

char* get_working_directory(){
	if(working_directory != NULL){
		return working_directory;
	}

	char* cwd = malloc(PATH_MAX);
	if (cwd == NULL) {
		perror("malloc error");
		return NULL;
	}
	if (getcwd(cwd, PATH_MAX) == NULL) {
		perror("getcwd error");
		free(cwd);
		return NULL;
	}

	working_directory = cwd;
	return working_directory;
}

char* get_executable_path(){
	if(executable_path != NULL){
		return executable_path;
	}

    char* buffer = malloc(sizeof(char)*PATH_MAX);
    ssize_t len = readlink("/proc/self/exe", buffer, PATH_MAX - 1);
    if(len == -1){
        return NULL;
    }
    buffer[len] = '\0';

	executable_path = g_strdup(buffer);
	return executable_path;
}

char* get_executable_name(){
	if(executable_name != NULL){
		return executable_name;
	}
	executable_name = g_basename(g_strdup(get_executable_path()));
	return executable_name;
}

char* get_executable_symbols_path(){
	if(executable_symbols_path != NULL){
		return executable_symbols_path;
	}
	executable_symbols_path = g_strdup_printf("symbols/%s", get_executable_name()); 
	return executable_symbols_path;
}
