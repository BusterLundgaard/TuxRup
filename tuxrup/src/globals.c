#include "globals.h"

g_signal_connect_data_t normal_g_signal_connect_data = NULL;

GHashTable *widget_callback_table = NULL;
GHashTable *widget_hashes = NULL;

const char* program_src_folder = "../../program_src";
const char *remapable_events[MAPPABLE_ACTIONS_LEN] = {"clicked"};