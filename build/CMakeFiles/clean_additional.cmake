# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ShowWindows_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ShowWindows_autogen.dir\\ParseCache.txt"
  "ShowWindows_autogen"
  )
endif()
