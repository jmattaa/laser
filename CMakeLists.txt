cmake_minimum_required(VERSION 3.18)

project(lsr LANGUAGES C)

option(ENABLE_SANITIZER "Enable AddressSanitizer for debug builds" ON)

if(ENABLE_SANITIZER)
    set(SANITIZER_FLAGS "-fsanitize=address")
else()
    set(SANITIZER_FLAGS "")
endif()

set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG ${SANITIZER_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Lua 5.1 REQUIRED)

if(NOT LUA_FOUND)
    message(FATAL_ERROR "Lua5.1 libraries not found")
endif()
include_directories("${LUA_INCLUDE_DIR}")

find_package(libgit2 REQUIRED)
if (NOT LIBGIT2_FOUND)
    message(FATAL_ERROR "Libgit2 libraries not found")
else()
    message(STATUS "Libgit2 include directory: ${LIBGIT2_INCLUDE_DIR}")
    message(STATUS "Libgit2 libraries: ${LIBGIT2_LIBRARIES}")
endif()
include_directories("${LIBGIT2_INCLUDE_DIR}")

file(GLOB_RECURSE SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(USER_HOME "$ENV{HOME}")
if (NOT LUA_SCRIPTS_INSTALL_PATH)
    set(LUA_SCRIPTS_INSTALL_PATH "${USER_HOME}/.lsr")
endif()
set(DEFAULT_SCRIPT_PATH "${LUA_SCRIPTS_INSTALL_PATH}/lsr.lua")

add_executable(lsr ${SRCS})

target_compile_definitions(lsr PRIVATE DEFAULT_SCRIPT_PATH="${DEFAULT_SCRIPT_PATH}")
target_include_directories(lsr PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(lsr PRIVATE ${LUA_LIBRARIES})

target_include_directories(lsr PRIVATE ${LIBGIT2_INCLUDE_DIR})
target_link_libraries(lsr PRIVATE ${LIBGIT2_LIBRARIES})

add_custom_target(format COMMAND clang-format -i ${SRCS})

set(LUA_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/.lsr")

file(GLOB LUA_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/lua/*.lua")

add_custom_command(TARGET lsr POST_BUILD
    COMMENT "Copying Lua scripts to ${LUA_SCRIPTS_INSTALL_PATH}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LUA_SCRIPTS_INSTALL_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy ${LUA_SCRIPTS} "${LUA_SCRIPTS_INSTALL_PATH}"
)

install(TARGETS lsr DESTINATION bin)

if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake

        COMMAND ${CMAKE_COMMAND} -E remove_directory "${LUA_SCRIPTS_INSTALL_PATH}"
        COMMENT "Removing Lua scripts from ${LUA_SCRIPTS_INSTALL_PATH}"
    )
endif()
