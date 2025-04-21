cmake_minimum_required(VERSION 3.21)

add_executable(untitledlog_example examples.cpp)
target_link_libraries(untitledlog_example PUBLIC UntitledLog imgui)
