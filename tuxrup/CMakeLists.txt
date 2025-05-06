cmake_minimum_required(VERSION 3.10)
project(TuxRup C)

# Define GTK version from the command line
# write `cmake .. -DUSE_GTK3` to use build GTK3 version
# write `cmake ..` to build GTK4 version
option(USE_GTK3 "Use GTK3" OFF)  

# FINDING PACKAGES / LIBRARIES
# ==========================================================
# GTK
find_package(PkgConfig REQUIRED)
if(USE_GTK3)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    add_definitions(-DUSE_GTK3)
else()
    pkg_check_modules(GTK4 REQUIRED gtk4)
endif()

#LIBELF
pkg_check_modules(LIBELF REQUIRED libelf)

#LLVM / LIBCLANG
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

find_library(LIBCLANG_LIBRARY NAMES clang libclang PATHS ${LLVM_LIBRARY_DIRS})
if(NOT LIBCLANG_LIBRARY)
    message(FATAL_ERROR "libclang not found!")
endif()

message(STATUS "Using libclang: ${LIBCLANG_LIBRARY}")
# ============================================================

# Set compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -shared -fPIC -Wno-discarded-qualifiers")

# Source files
set(SOURCES
    src/callback_map.c
    src/editing_properties/edit_properties_window.c
    src/function_dispatcher.c
    src/utilities/hashing.c
    src/utilities/util.c
    src/method_intercepting/gtk_events.c
    src/method_intercepting/events_util.c
    src/method_intercepting/hooks.c
    src/method_intercepting/overrides.c
    src/right_click/context_menu.c
    src/globals.c
    src/editing_css/css_utils.c
    src/editing_css/edit_css_window.c
    src/editing_callbacks/document_parsing.c
    src/editing_callbacks/edit_callbacks_window.c
    src/editing_callbacks/fix_function_body.c
)

# Add a shared library for pointer_name_conversion
add_library(pointer_name_conversion SHARED src/utilities/pointer_name_conversion.c)

# Link libraries based on GTK version
if(USE_GTK3)
	add_library(tuxrup_gtk3 SHARED ${SOURCES})
    target_link_libraries(tuxrup_gtk3 PRIVATE ${GTK3_LIBRARIES} ${LIBCLANG_LIBRARY} ${LIBELF_LIBRARIES} pointer_name_conversion)
    target_include_directories(tuxrup_gtk3 PRIVATE ${GTK3_INCLUDE_DIRS} ${LIBELF_INCLUDE_DIRS})
	target_compile_options(tuxrup_gtk3 PRIVATE ${GTK3_CFLAGS_OTHER})
else()
	add_library(tuxrup_gtk4 SHARED ${SOURCES})
    target_link_libraries(tuxrup_gtk4 PRIVATE ${GTK4_LIBRARIES} ${LIBCLANG_LIBRARY} ${LIBELF_LIBRARIES} pointer_name_conversion)
    target_include_directories(tuxrup_gtk4 PRIVATE ${GTK4_INCLUDE_DIRS} ${LIBELF_INCLUDE_DIRS})
	target_compile_options(tuxrup_gtk4 PRIVATE ${GTK4_CFLAGS_OTHER})
endif()
