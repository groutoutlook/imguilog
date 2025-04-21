cmake_minimum_required(VERSION 3.20)
project(imguilog CXX)

set(CMAKE_C_STANDARD 99)

if (PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin-lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin-etc")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
    
    include(FetchContent)
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/giladreich/cmake-imgui
        GIT_TAG v1.90.7
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(imgui)
endif()


set(UL_BUILD_EXAMPLES OFF CACHE BOOL "Build the example executable")

# Main C++ sources
set(ULOG_SOURCES
    ULog.cpp
    ULogImGui.cpp
    utils/imgui_stdlib.cpp
)

set(ULOG_HEADERS
    ULog.hpp
    ULogCommon.h
    ULogImGui.hpp
    utils/imgui_stdlib.h
)

# C API sources
set(CULOG_SOURCES
    C/CULog.cpp
    C/CULogImGui.cpp
)

set(CULOG_HEADERS
    C/CULog.h
    C/CULogImGui.h
)

add_compile_definitions(ULOG_IMGUI)

# Create the C++ library
add_library(imguilog STATIC ${ULOG_SOURCES} ${ULOG_HEADERS})

# Create the C API library
add_library(Cimguilog STATIC ${CULOG_SOURCES} ${CULOG_HEADERS})

target_include_directories(imguilog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Cimguilog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/C)

# Link ImGui to your libraries
target_link_libraries(imguilog PUBLIC imgui)
target_link_libraries(Cimguilog PUBLIC imgui)


# Optionally, link ImGui if available
# find_package(imgui REQUIRED) # Uncomment and adjust if you want to link ImGui

if (UL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
