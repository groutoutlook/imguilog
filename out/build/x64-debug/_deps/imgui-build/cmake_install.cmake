# Install script for directory: D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/install/x64-debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/bin-lib/imguid.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/imgui" TYPE FILE FILES
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/imgui.h"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/imconfig.h"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/imgui_internal.h"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/imstb_rectpack.h"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/imstb_textedit.h"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/imstb_truetype.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/misc/fonts" TYPE FILE FILES
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/misc/fonts/Cousine-Regular.ttf"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/misc/fonts/DroidSans.ttf"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/misc/fonts/Karla-Regular.ttf"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/misc/fonts/ProggyClean.ttf"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/misc/fonts/ProggyTiny.ttf"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-src/imgui/misc/fonts/Roboto-Medium.ttf"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "lib" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE OPTIONAL FILES "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-build/Debug/imguid.pdb")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/imgui/ImGuiTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/imgui/ImGuiTargets.cmake"
         "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-build/CMakeFiles/Export/1c9f1db1a0c9398944d65050189647a4/ImGuiTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/imgui/ImGuiTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/imgui/ImGuiTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/imgui" TYPE FILE FILES "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-build/CMakeFiles/Export/1c9f1db1a0c9398944d65050189647a4/ImGuiTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/imgui" TYPE FILE FILES "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-build/CMakeFiles/Export/1c9f1db1a0c9398944d65050189647a4/ImGuiTargets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/imgui" TYPE FILE FILES
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-build/generated/ImGuiConfigVersion.cmake"
    "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-build/generated/ImGuiConfig.cmake"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/ProgramDataD/MiscLang/01-C++/repo/UntitledLog/out/build/x64-debug/_deps/imgui-build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
