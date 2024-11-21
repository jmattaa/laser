cmake_minimum_required(VERSION 3.14)

project(lsr LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG -fsanitize=address -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

find_package(Lua51 REQUIRED)

if(LUA_INCLUDE_DIR)
    message(STATUS "Lua include directory: ${LUA_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "Lua headers not found")
endif()

include_directories("${LUA_INCLUDE_DIR}")

file(GLOB_RECURSE SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(lsr ${SRCS})

target_include_directories(lsr PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(lsr PRIVATE ${LUA_LIBRARIES})

add_custom_target(format COMMAND clang-format -i ${SRCS})
install(TARGETS lsr DESTINATION /usr/local/bin)

