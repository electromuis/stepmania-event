# Plugin Manager

The plugin manager is a system to add core functionality to the stepmania engine in a modular way. Now in a bit more detail:

## Example plugins
https://github.com/electromuis/SM5PL-HelloWorldPlugin
https://github.com/electromuis/SM5PL-WebSocketPlugin
https://github.com/electromuis/SM5PL-PadmissPlugin
https://github.com/electromuis/SM5PL-GrooveStatsPlugin

To use these, clone the repository to this folder, enable the WITH_PLUGINS flag in cmake and compile as normal.

## Plugin Manager System
Plugin layout
Global preferences
add metrics
abstract layout

## Plugin loading / molurarity
.dll
.so
Embedded

## Stepmania engine / core functionality
Singletons
Lua functions
Screen types

## System support
Windows
Linux
Apple

## Writing a plugin
HelloWorldPlugin
CMake hooks
WITH_PLUGINS flag

## Todo's
libstepmania
stepmania sdk
github actions
content manager
