cmake_minimum_required(VERSION 3.18)

project(lsr LANGUAGES C)

option(HOMEBREW_INSTALLATION "Is this a Homebrew installation" OFF)
option(ENABLE_SANITIZER "Enable AddressSanitizer for debug builds" ON)

if(ENABLE_SANITIZER)
    set(SANITIZER_FLAGS "-fsanitize=address")
else()
    set(SANITIZER_FLAGS "")
endif()

set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG ${SANITIZER_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

find_package(Lua 5.1 REQUIRED)

if(NOT LUA_FOUND)
    message(FATAL_ERROR "Lua5.1 libraries not found")
endif()

include_directories("${LUA_INCLUDE_DIR}")

file(GLOB_RECURSE SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(USER_HOME "$ENV{HOME}")
set(LUA_SCRIPTS_INSTALL_PATH "${USER_HOME}/.lsr")
set(DEFAULT_SCRIPT_PATH "${LUA_SCRIPTS_INSTALL_PATH}/lsr.lua")

add_executable(lsr ${SRCS})

target_compile_definitions(lsr PRIVATE DEFAULT_SCRIPT_PATH="${DEFAULT_SCRIPT_PATH}")
target_include_directories(lsr PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(lsr PRIVATE ${LUA_LIBRARIES})

file(GLOB LUA_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/lua/*.lua")

if (NOT HOMEBREW_INSTALLATION)
    add_custom_command(TARGET lsr POST_BUILD
        COMMENT "Copying Lua scripts to ${LUA_SCRIPTS_INSTALL_PATH}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${LUA_SCRIPTS_INSTALL_PATH}"
        COMMAND ${CMAKE_COMMAND} -E copy ${LUA_SCRIPTS} "${LUA_SCRIPTS_INSTALL_PATH}"
    )
endif()

install(TARGETS lsr DESTINATION bin)

