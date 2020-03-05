cmake_minimum_required(VERSION 3.6)

project(TinyECS)
message(STATUS "CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID}") 

add_definitions(-DUNICODE -D_UNICODE)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_compile_options("-std=gnu++17")
else()
	add_compile_options("/std:c++latest")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/UnitTest)
