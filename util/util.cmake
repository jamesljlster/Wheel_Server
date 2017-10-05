
# Set paths
set(UTIL_PATHS
	${CMAKE_CURRENT_SOURCE_DIR}/util/wheel_server
	)

# Add subdirectiories
foreach(UTIL_PATH ${UTIL_PATHS})
	add_subdirectory(${UTIL_PATH})
endforeach()
