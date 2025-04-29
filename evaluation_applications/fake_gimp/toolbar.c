#include "toolbar.h"

Toolbar_menu_data menus[5] = {
    {"File",  {"Save", "Save As", "Import", "Export", "Quit"}},
    {"Edit",  {"Undo", "Redo", "Cut", "Copy", "Paste"}},
    {"Image", {"Resize", "Crop", "Rotate", "Flip", "Adjust"}},
    {"Layer", {"New Layer", "Duplicate Layer", "Merge Down", "Delete Layer", "Layer Properties"}},
    {"Help",  {"Documentation", "Keyboard Shortcuts", "Check for Updates", "Report Bug", "About"}},
};