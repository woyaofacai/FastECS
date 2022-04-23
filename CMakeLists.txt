cmake_minimum_required(VERSION 3.16)

project(FastECS)
message(STATUS "CMAKE_CXX_COMPILER_ID ${CMAKE_CXX_COMPILER_ID}") 

add_definitions(-DUNICODE -D_UNICODE)

# if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
# 	add_compile_options("-std=gnu++17")
# else()
# 	add_compile_options("/std:c++latest")
# endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /std:c++17 /MP")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -pedantic")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/UnitTest)
