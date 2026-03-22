# Install script for directory: C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SchedGUI")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Qt/Tools/mingw1310_64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/json" TYPE FILE FILES
    "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include/json/allocator.h"
    "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include/json/assertions.h"
    "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include/json/config.h"
    "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include/json/forwards.h"
    "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include/json/json.h"
    "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include/json/json_features.h"
    "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include/json/reader.h"
    "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include/json/value.h"
    "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include/json/version.h"
    "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-src/include/json/writer.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/jsoncpp-build/include/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
