# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# Project name
project(ScriptCombiner)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets package and its components
# We need 'core' and 'base' for all apps
# 'stc' is for the StyledTextCtrl (the code editor)
find_package(wxWidgets COMPONENTS stc core base REQUIRED)

# Include wxWidgets' CMake configuration
include(${wxWidgets_USE_FILE})

# Define the executable
# For Windows, we use WIN32 to create a GUI app (no console)
if(WIN32)
    add_executable(ScriptCombiner WIN32 
        src/App.cpp 
        src/App.h 
        src/MainFrame.cpp 
        src/MainFrame.h
    )
else()
    add_executable(ScriptCombiner 
        src/App.cpp 
        src/App.h 
        src/MainFrame.cpp 
        src/MainFrame.h
    )
endif()

# For macOS, set properties to create a proper .app bundle
if(APPLE)
    set_target_properties(ScriptCombiner PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.ScriptCombiner"
        MACOSX_BUNDLE_NAME "ScriptCombiner"
    )
endif()

# Link the executable against wxWidgets libraries
target_link_libraries(ScriptCombiner ${wxWidgets_LIBRARIES})