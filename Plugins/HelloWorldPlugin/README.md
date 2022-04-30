# HelloWorldPlugin

In order to test out the [PluginManager](../README.md), or to be used as a template project, this plugin is a minimal implementation of a working plugin.

This project demonstrates how to get started, the only things it will do is:
 - Show "HelloWorldPlugin loaded" in the log when loaded
    To check the loading stage
 - Show "HelloWorldPlugin unloaded" in the log when unloaded
    To check the unloading state
 - Show "HelloWorldPlugin: Test plugin feels a step" when detecting a step during gameplay
    To check communucation with the StepMania message system

To get started with your own plugin:
 - copy this folder (HelloWorldPlugin) to youre new name (MyNewPlugin)
 - Rename HelloWorldPlugin.cpp and HelloWorldPlugin.h to MyNewPlugin.cpp and MyNewPlugin.h
 - Edit the first line in [CMakeLists.txt] from ```set(PLUGIN_NAME "HelloWorldPlugin")``` to ```set(PLUGIN_NAME "MyNewPlugin")```
 - Replace all references from HelloWorld to MyNew in both MyNewPlugin.cpp and MyNewPlugin.h
 - Re-run CMake with the plugin flags enabled
 - Start the game to test
