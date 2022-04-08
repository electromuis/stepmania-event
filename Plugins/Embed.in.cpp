#include "arch/Plugin/PluginDriver.h"

extern "C" {
extern PluginDetails ${PLUGIN_NAME}_Details;
}

LoadedPluginEmbedded ${PLUGIN_NAME}Register(${PLUGIN_NAME}_Details); 