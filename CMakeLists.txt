cmake_minimum_required(VERSION 3.16)

# Set project name and version
project(RandPlay 
    VERSION 1.0.0 
    DESCRIPTION "Random File Player - A Windows application for randomly opening files"
    LANGUAGES CXX RC
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Unicode
add_definitions(-DUNICODE -D_UNICODE)

# Set Windows subsystem
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
endif()

# Configure build types
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Project version: ${PROJECT_VERSION}")

# Find required packages
find_package(PkgConfig QUIET)

# Add the executable
add_executable(RandPlay WIN32
    # Main source file
    src/main.cpp
    
    # Core modules
    src/core/Utils.cpp
    src/core/Utils.h
    src/core/Settings.cpp
    src/core/Settings.h
    src/core/FileIndexer.cpp
    src/core/FileIndexer.h
    
    # UI modules
    src/ui/UIHelpers.cpp
    src/ui/UIHelpers.h
    
    # Constants
    src/constants/AppConstants.h
    src/constants/ResourceIds.h
    
    # Precompiled header
    include/pch.h
    
    # Resources
    resources/RandPlay.rc
    resources/strings/lang_en.rc
    resources/icons/RandPlay.ico
)

# Add include directories
target_include_directories(RandPlay PRIVATE
    src
    src/core
    src/ui
    src/constants
    include
    resources
)

# Add source groups for better IDE organization
source_group("Source Files\\Main" FILES src/main.cpp)
source_group("Source Files\\Core" FILES 
    src/core/Utils.cpp
    src/core/Settings.cpp
    src/core/FileIndexer.cpp
)
source_group("Source Files\\UI" FILES 
    src/ui/UIHelpers.cpp
)
source_group("Header Files\\Core" FILES 
    src/core/Utils.h
    src/core/Settings.h
    src/core/FileIndexer.h
)
source_group("Header Files\\UI" FILES 
    src/ui/UIHelpers.h
)
source_group("Header Files\\Constants" FILES 
    src/constants/AppConstants.h
    src/constants/ResourceIds.h
)
source_group("Header Files\\PCH" FILES include/pch.h)
source_group("Resource Files" FILES 
    resources/RandPlay.rc 
    resources/strings/lang_en.rc 
    resources/icons/RandPlay.ico
)

# Set target properties
set_target_properties(RandPlay PROPERTIES
    WIN32_EXECUTABLE TRUE
    VS_GLOBAL_WindowsTargetPlatformVersion "10.0"
    OUTPUT_NAME "RandPlay"
)

# Link libraries
target_link_libraries(RandPlay PRIVATE
    user32
    gdi32
    comctl32
    shell32
    ole32
    shlwapi
)

# Set compiler flags for MSVC
if(MSVC)
    target_compile_options(RandPlay PRIVATE
        /EHsc           # Exception handling
        /std:c++17      # C++ standard
        /W3             # Warning level 3
        /permissive-    # Disable non-conforming code
    )
    
    # Release optimizations
    target_compile_options(RandPlay PRIVATE
        $<$<CONFIG:Release>:/O2>        # Optimize for speed
        $<$<CONFIG:Release>:/DNDEBUG>   # Define NDEBUG for release
    )
    
    # Debug options
    target_compile_options(RandPlay PRIVATE
        $<$<CONFIG:Debug>:/Od>          # No optimization
        $<$<CONFIG:Debug>:/Zi>          # Debug information
        $<$<CONFIG:Debug>:/D_DEBUG>     # Define _DEBUG for debug
    )
    
    # Add manifest dependency for common controls
    set_target_properties(RandPlay PROPERTIES
        LINK_FLAGS "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\""
    )
endif()

# Set output directory
set_target_properties(RandPlay PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

# Copy icon file to output directory
add_custom_command(TARGET RandPlay POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/resources/icons/RandPlay.ico"
    "$<TARGET_FILE_DIR:RandPlay>/RandPlay.ico"
)

# Installation rules
install(TARGETS RandPlay
    RUNTIME DESTINATION bin
)

install(FILES resources/icons/RandPlay.ico
    DESTINATION bin
)
