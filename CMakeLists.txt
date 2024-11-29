cmake_minimum_required(VERSION 3.18)

project(lsr LANGUAGES C)

set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG -fsanitize=address -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

find_package(Lua51 REQUIRED)

if (NOT LUA51_FOUND)
    message(FATAL_ERROR "Lua5.1 libraries not found")
else()
    message(STATUS "Lua include directory: ${LUA_INCLUDE_DIR}")
    message(STATUS "Lua libraries: ${LUA_LIBRARIES}")
endif()

if(NOT LUA_INCLUDE_DIR)
    message(FATAL_ERROR "Lua headers not found")
else()
    message(STATUS "Lua include directory: ${LUA_INCLUDE_DIR}")
endif()

include_directories("${LUA_INCLUDE_DIR}")

file(GLOB_RECURSE SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

set(DEFAULT_SCRIPT_PATH "$ENV{HOME}/.lsr/lsr.lua")

add_executable(lsr ${SRCS})

target_compile_definitions(lsr PRIVATE DEFAULT_SCRIPT_PATH="${DEFAULT_SCRIPT_PATH}")
target_include_directories(lsr PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(lsr PRIVATE ${LUA_LIBRARIES})

add_custom_target(format COMMAND clang-format -i ${SRCS})

set(LUA_INSTALL_PATH "$ENV{HOME}/.lsr")

file(GLOB LUA_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/lua/*.lua")
file(MAKE_DIRECTORY "${LUA_INSTALL_PATH}")
add_custom_command(TARGET lsr POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LUA_INSTALL_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy ${LUA_SCRIPTS} "${LUA_INSTALL_PATH}"
    COMMENT "Copying Lua scripts to ${LUA_INSTALL_PATH}"
)

install(TARGETS lsr DESTINATION bin)

if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake

        COMMAND ${CMAKE_COMMAND} -E remove_directory "${LUA_INSTALL_PATH}"
        COMMENT "Removing Lua scripts from ${LUA_INSTALL_PATH}"
    )
endif()

