# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/openxlsx-src")
  file(MAKE_DIRECTORY "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/openxlsx-src")
endif()
file(MAKE_DIRECTORY
  "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/openxlsx-build"
  "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/openxlsx-subbuild/openxlsx-populate-prefix"
  "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/openxlsx-subbuild/openxlsx-populate-prefix/tmp"
  "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/openxlsx-subbuild/openxlsx-populate-prefix/src/openxlsx-populate-stamp"
  "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/openxlsx-subbuild/openxlsx-populate-prefix/src"
  "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/openxlsx-subbuild/openxlsx-populate-prefix/src/openxlsx-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/openxlsx-subbuild/openxlsx-populate-prefix/src/openxlsx-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/CS/year3/projects/schedulify/Schedulify/build-server/_deps/openxlsx-subbuild/openxlsx-populate-prefix/src/openxlsx-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
