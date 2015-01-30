#CMake file for Luapath
#exports 
# LUAPATH_LIBRARIES
# LUAPATH_INCLUDE_DIR
# 		
cmake_minimum_required(VERSION 2.8.2)
project(luapath)

set(luapath_VERION 0.1)

# set compiler flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /WX /EHa")
endif()

message(STATUS "Adding include directory : ${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include")

file(GLOB_RECURSE LUAPATH_SRC src/*.cpp)

# Allow the developer to select if Dynamic or Static libraries are built
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
# Set the LIB_TYPE variable to STATIC
SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
  # User wants to build Dynamic Libraries, so change the LIB_TYPE variable to CMake keyword 'SHARED'
  SET (LIB_TYPE SHARED)
ENDIF (BUILD_SHARED_LIBS)

#add preprocess macro for dll linkage
#add_definitions(-DLUAPATH_EXPORTS)

add_library(luapath  ${LIB_TYPE} ${LUAPATH_SRC})

#a place to save the library files
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")

#link the main lua library
find_package(Lua)
if(LUA_FOUND)
	message(STATUS "Found local lua installation")
	message(STATUS "version : ${LUA_VERSION_STRING}")
	message(STATUS "lua include path: ${LUA_LIBRARIES}")
	message(STATUS "lua libraries: ${LUA_LIBRARIES}")
	include_directories("${LUA_INCLUDE_DIR}")
	target_link_libraries(luapath  ${LUA_LIBRARIES})

	SET(LUAPATH_INCLUDE_DIR "${LUA_INCLUDE_DIR};${PROJECT_SOURCE_DIR}/include" PARENT_SCOPE)

else()
	message(STATUS "Using own lua version")
	if(WIN32)
		set(LUA_DIR "3rdparty/lua")
		add_subdirectory(${LUA_DIR} ${CMAKE_BINARY_DIR}/lua)
		target_link_libraries ( luapath liblua )
	elseif(UNIX)
		set(LUA_DIR "3rdparty/lua-unix")
		target_link_libraries ( luapath ${LUA_DIR}/liblua.a)
	endif()
	include_directories("${LUA_DIR}/src")

	SET(LUAPATH_INCLUDE_DIR "${LUA_DIR}/src;${PROJECT_SOURCE_DIR}/include" PARENT_SCOPE)

endif()

set(LUAPATH_LIBRARIES "luapath;${LUA_LIBRARIES}" PARENT_SCOPE)

#testing
add_subdirectory("test" ${CMAKE_BINARY_DIR}/test)

