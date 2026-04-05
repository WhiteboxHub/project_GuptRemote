cmake_minimum_required(VERSION 3.20)
project(GuptMac LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# macOS specific frameworks
find_library(FOUNDATION_FRAMEWORK Foundation)
find_library(APPKIT_FRAMEWORK AppKit)
find_library(SCREENCAPTURE_FRAMEWORK ScreenCaptureKit)
find_library(COREGRAPHICS_FRAMEWORK CoreGraphics)
find_library(COREIMAGE_FRAMEWORK CoreImage)
find_library(AVFOUNDATION_FRAMEWORK AVFoundation)
find_library(COREMEDIA_FRAMEWORK CoreMedia)
find_library(QUARTZCORE_FRAMEWORK QuartzCore)

set(MACOS_LIBS 
    ${FOUNDATION_FRAMEWORK} 
    ${APPKIT_FRAMEWORK} 
    ${SCREENCAPTURE_FRAMEWORK} 
    ${COREGRAPHICS_FRAMEWORK}
    ${COREIMAGE_FRAMEWORK}
    ${AVFOUNDATION_FRAMEWORK}
    ${COREMEDIA_FRAMEWORK}
    ${QUARTZCORE_FRAMEWORK}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(GuptMac MACOSX_BUNDLE
    main.mm
    Shared/Protocol.h
    Core/Capture/ScreenCapturer.h
    Core/Capture/ScreenCapturer.mm
    Core/Input/InputInjector.h
    Core/Input/InputInjector.mm
    Core/Network/TcpNetwork.h
    Core/Network/TcpNetwork.cpp
)

target_link_libraries(GuptMac PRIVATE ${MACOS_LIBS})

# macOS Bundle Properties
set_target_properties(GuptMac PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.gupt.remotedesktop"
    MACOSX_BUNDLE_BUNDLE_NAME "GuptMac"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
)
