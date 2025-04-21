cmake_minimum_required(VERSION 3.21)

add_executable(imguilog_example examples.cpp)
target_link_libraries(imguilog_example PUBLIC imguilog imgui)
