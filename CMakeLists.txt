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

# Set resource compiler flags for proper Chinese character encoding
if(MSVC)
    set(CMAKE_RC_FLAGS "/l 0x804 /c 65001")
endif()

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

# Common source files for both versions
set(COMMON_SOURCES
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
)

# Common include directories
set(COMMON_INCLUDES
    src
    src/core
    src/ui
    src/constants
    include
    resources
)

# English version
add_executable(RandPlay_EN WIN32
    ${COMMON_SOURCES}
    resources/RandPlay_en.rc
    resources/icons/RandPlay.ico
)

# Chinese version  
add_executable(RandPlay_ZH_CN WIN32
    ${COMMON_SOURCES}
    resources/RandPlay_zh_cn.rc
    resources/icons/RandPlay.ico
)

# Function to configure common properties
function(configure_randplay_target TARGET_NAME OUTPUT_NAME)
    # Add include directories
    target_include_directories(${TARGET_NAME} PRIVATE ${COMMON_INCLUDES})
    
    # Set target properties
    set_target_properties(${TARGET_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        VS_GLOBAL_WindowsTargetPlatformVersion "10.0"
        OUTPUT_NAME "${OUTPUT_NAME}"
    )
    
    # Link libraries
    target_link_libraries(${TARGET_NAME} PRIVATE
        user32
        gdi32
        comctl32
        shell32
        ole32
        shlwapi
        shcore
    )
    
    # Set compiler flags for MSVC
    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE
            /EHsc           # Exception handling
            /std:c++17      # C++ standard
            /W3             # Warning level 3
            /permissive-    # Disable non-conforming code
            /utf-8          # UTF-8 source file encoding
        )
        
        # Release optimizations
        target_compile_options(${TARGET_NAME} PRIVATE
            $<$<CONFIG:Release>:/O2>        # Optimize for speed
            $<$<CONFIG:Release>:/DNDEBUG>   # Define NDEBUG for release
        )
        
        # Debug options
        target_compile_options(${TARGET_NAME} PRIVATE
            $<$<CONFIG:Debug>:/Od>          # No optimization
            $<$<CONFIG:Debug>:/Zi>          # Debug information
            $<$<CONFIG:Debug>:/D_DEBUG>     # Define _DEBUG for debug
        )
        
        # Add manifest dependency for common controls
        set_target_properties(${TARGET_NAME} PROPERTIES
            LINK_FLAGS "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\""
        )
    endif()
    
    # Set output directory
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
    )
    
    # Copy icon file to output directory
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/resources/icons/RandPlay.ico"
        "$<TARGET_FILE_DIR:${TARGET_NAME}>/RandPlay.ico"
    )
endfunction()

# Configure both targets
configure_randplay_target(RandPlay_EN "RandPlay_EN")
configure_randplay_target(RandPlay_ZH_CN "RandPlay_ZH_CN")

# Add source groups for better IDE organization
function(add_source_groups TARGET_NAME)
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
endfunction()

# Apply source groups to both targets
add_source_groups(RandPlay_EN)
add_source_groups(RandPlay_ZH_CN)

# Add resource source groups for English version
source_group("Resource Files" FILES 
    resources/RandPlay_en.rc 
    resources/strings/lang_en.rc 
    resources/icons/RandPlay.ico
)

# Installation rules
install(TARGETS RandPlay_EN RandPlay_ZH_CN
    RUNTIME DESTINATION bin
)

install(FILES resources/icons/RandPlay.ico
    DESTINATION bin
)
