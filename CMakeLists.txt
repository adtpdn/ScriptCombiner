# CMake minimum version
cmake_minimum_required(VERSION 3.10)

# Project name
project(ScriptCombiner)

# -----------------------------------------------------------------------------
# 1. Architecture & Standard Settings
# -----------------------------------------------------------------------------

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MacOS: Build Universal Binary (Native support for M1/M2/M3 and Intel)
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac" FORCE)
endif()

# -----------------------------------------------------------------------------
# 2. Dependencies
# -----------------------------------------------------------------------------

# Find wxWidgets (core=GUI, base=System, stc=StyledTextCtrl/Editor)
find_package(wxWidgets COMPONENTS stc core base REQUIRED)

# Include wxWidgets configuration
include(${wxWidgets_USE_FILE})

# Source files
set(SOURCE_FILES
    src/App.cpp
    src/App.h
    src/MainFrame.cpp
    src/MainFrame.h
)

# -----------------------------------------------------------------------------
# 3. Build Target Definitions
# -----------------------------------------------------------------------------

if(WIN32)
    # Windows: WIN32 flag ensures it runs as a GUI app (no console window)
    add_executable(ScriptCombiner WIN32 ${SOURCE_FILES})
    
    # Enable High DPI Awareness for crisp text on Windows
    if(MSVC)
        set_property(TARGET ScriptCombiner APPEND PROPERTY LINK_FLAGS "/MANIFEST:NO") 
    endif()

elseif(APPLE)
    # macOS: Create an App Bundle (.app)
    add_executable(ScriptCombiner MACOSX_BUNDLE ${SOURCE_FILES})
    
    # Bundle properties
    set_target_properties(ScriptCombiner PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "ScriptCombiner"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.adtpdn.scriptcombiner"
        MACOSX_BUNDLE_ICON_FILE "icon.icns" # Optional: Add if you have an icon
        MACOSX_BUNDLE_INFO_STRING "Script Combiner for AI"
    )

else()
    # Linux / Unix
    add_executable(ScriptCombiner ${SOURCE_FILES})
    
    # Linux install target (standard location /usr/local/bin)
    install(TARGETS ScriptCombiner DESTINATION bin)
endif()

# -----------------------------------------------------------------------------
# 4. Linking
# -----------------------------------------------------------------------------

target_link_libraries(ScriptCombiner ${wxWidgets_LIBRARIES})