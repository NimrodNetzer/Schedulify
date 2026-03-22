# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\SchedServer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SchedServer_autogen.dir\\ParseCache.txt"
  "SchedServer_autogen"
  )
endif()
