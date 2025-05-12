cmake_minimum_required(VERSION 3.20)


# Add backend sources to your library

add_executable(imguilog_example examples.cpp)
target_link_libraries(imguilog_example PUBLIC imguilog 
imgui::imgui
glfw
Msvcrtd ucrtd)
