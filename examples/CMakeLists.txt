cmake_minimum_required(VERSION 3.20)
include(FetchContent)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG master        
)

FetchContent_MakeAvailable(glfw)


# Specify ImGui backend sources for GLFW + DX12
set(IMGUI_BACKEND_SOURCES
    ${imgui_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/imgui/backends/imgui_impl_dx12.cpp
    ${imgui_SOURCE_DIR}/imgui/backends/imgui_impl_win32.cpp
)
set(IMGUI_BACKEND_HEADERS
    ${imgui_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.h
    ${imgui_SOURCE_DIR}/imgui/backends/imgui_impl_dx12.h
    ${imgui_SOURCE_DIR}/imgui/backends/imgui_impl_win32.h
)

# Add backend sources to your library

add_executable(imguilog_example examples.cpp)
target_sources(imguilog_example PUBLIC ${IMGUI_BACKEND_SOURCES} ${IMGUI_BACKEND_HEADERS})
target_link_libraries(imguilog_example PUBLIC imguilog 
imgui 
glfw 
Msvcrt ucrtd)
