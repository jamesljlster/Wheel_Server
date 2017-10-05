# Set dependences paths
set(DEPS_PATHS
	${CMAKE_CURRENT_SOURCE_DIR}/deps/Wheel/src
	${CMAKE_CURRENT_SOURCE_DIR}/deps/tcpmgr/lib
	${CMAKE_CURRENT_SOURCE_DIR}/deps/args/lib
	)
include_directories(${DEPS_PATHS})

# Find other dependences
find_path(SERIALPORT_INCLUDE_DIR libserialport.h
	"/usr/include"
	"/usr/local/include"
	)
find_library(SERIALPORT_LIB libserialport.a
	"/usr/lib"
	"/usr/local/lib"
	)
include_directories(${SERIALPORT_INCLUDE_DIR})

# Add subdirectory
foreach(DEPS_PATH ${DEPS_PATHS})
	add_subdirectory(${DEPS_PATH})
endforeach()
