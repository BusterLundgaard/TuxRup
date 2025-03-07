cmake_minimum_required(VERSION 3.10)
project(TuxRup C)

# Define GTK version from the command line
# write `cmake .. -DUSE_GTK3` to use build GTK3 version
# write `cmake ..` to build GTK4 version
option(USE_GTK3 "Use GTK3" OFF)  # Default is OFF, so GTK4 is used unless specified

# Find required packages
find_package(PkgConfig REQUIRED)
if(USE_GTK3)
    # If GTK3 is selected, find GTK3
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    add_definitions(-DUSE_GTK3)
else()
    # If GTK3 is not selected, find GTK4
    pkg_check_modules(GTK4 REQUIRED gtk4)
endif()

# Set compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -shared -fPIC -Wno-discarded-qualifiers")

# Source files
set(SOURCES
    src/intercept_gtk_functions.c
    src/callback_information.c
    src/callback_map.c
    src/compute_widget_hash.c
    src/document_parsing.c
    src/fix_function_body.c
    src/function_dispatcher.c
    src/globals.c
    src/gtk_events.c
    src/modify_callback.c
    src/pointer_name_conversion.c
    src/util.c
)

# Add the shared library target
if(USE_GTK3)
    add_library(tuxrup_gtk3 SHARED ${SOURCES})
else() 
    add_library(tuxrup_gtk4 SHARED ${SOURCES})
endif()

# Link libraries based on GTK version
if(USE_GTK3)
    target_link_libraries(tuxrup_gtk3 ${GTK3_LIBRARIES})
    target_include_directories(tuxrup_gtk3 PRIVATE ${GTK3_INCLUDE_DIRS})
else()
    target_link_libraries(tuxrup_gtk4 ${GTK4_LIBRARIES})
    target_include_directories(tuxrup_gtk4 PRIVATE ${GTK4_INCLUDE_DIRS})
endif()

# Add any additional compile options if necessary
if(USE_GTK3)
    target_compile_options(tuxrup_gtk3 PRIVATE ${GTK3_CFLAGS_OTHER})
else()
    target_compile_options(tuxrup_gtk4 PRIVATE ${GTK4_CFLAGS_OTHER})
endif()
