cmake_minimum_required(VERSION 3.20)
project(GuptMac LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DGL_SILENCE_DEPRECATION)

# Core and Shared headers
include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Shared)

file(GLOB_RECURSE SOURCES 
    "*.mm" 
    "*.cpp" 
    "*.h"
)

add_executable(GuptMac MACOSX_BUNDLE ${SOURCES})

# Frameworks
target_link_libraries(GuptMac PRIVATE 
    "-framework Cocoa"
    "-framework ScreenCaptureKit"
    "-framework CoreGraphics"
    "-framework CoreVideo"
    "-framework CoreMedia"
    "-framework AVFoundation"
)

# Standard permissions and info
set_target_properties(GuptMac PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
)
