find_package(Lua)
if(LUA_FOUND)
	message(STATUS "Found local lua installation")
	message(STATUS "${LUA_VERSION_STRING}")
else()
	message(STATUS "Using own lua installation")
endif()

include_directories("${LUA_INCLUDE_DIR}")