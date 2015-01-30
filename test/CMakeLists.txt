#
# add_custom_test(<target> <libs> <sources>...)
#
#  Adds a Boost Test based test executable, <target>, built from <sources>,
#  linked against <test_libs> and  adds the test so that CTest will run it.
#  Both the executable and the test will be named <target>.
#
function(add_boost_test target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} ${Boost_FILESYSTEM_LIBRARY}
                     				 ${Boost_SYSTEM_LIBRARY}
                     				 ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    target_link_libraries(${target} luapath)

    add_test(${target} ${target})

endfunction()

set(Boost_USE_STATIC_LIBS ON)
#enable testing if Boost libraries are present
find_package(Boost COMPONENTS system filesystem unit_test_framework)
if(Boost_FOUND)
	message(STATUS "Using Boost Test Framework")
	message(STATUS "Boost includes : ${Boost_INCLUDE_DIR}")
	message(STATUS "Boost libraries : ${Boost_LIBRARIES}")

	file(GLOB_RECURSE TEST_SRC *.cpp)
	message(STATUS "Test sources: ${TEST_SRC}")

	enable_testing()
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
	add_boost_test(testLuaState ${TEST_SRC})
 #    add_executable(testLuaState ${TEST_SRC})
 #    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
	# LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
	# target_link_libraries(testLuaState ${Boost_LIBRARIES})

else()
	message(STATUS "Skipping test. Boost Test Framework not found")
endif()