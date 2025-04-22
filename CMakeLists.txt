cmake_minimum_required(VERSION 3.20)
project(imguilog CXX)

set(CMAKE_C_STANDARD 23)

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
else()
    # INFO: I dont even know how can I do this.
endif()


set(ILOG_BUILD_EXAMPLES OFF CACHE BOOL "Build the example executable")

# Main C++ sources
set(ULOG_SOURCES
    ULog.cpp
    ULogImGui.cpp
    utils/imgui_stdlib.cpp # for extra use of std::string in imgui.
)

set(ULOG_HEADERS
    ULog.hpp
    ULogCommon.h
    ULogImGui.hpp
    utils/imgui_stdlib.h
)

add_compile_definitions(ULOG_IMGUI)

# Create the C++ library
add_library(imguilog STATIC ${ULOG_SOURCES} ${ULOG_HEADERS})

target_include_directories(imguilog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link ImGui to your libraries
target_link_libraries(imguilog PUBLIC imgui)

if (ILOG_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
