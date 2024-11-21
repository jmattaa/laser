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

set(LUA_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/lua/lsr.lua")
set(LUA_INSTALL_PATH "/usr/local/share/lsr")

add_custom_command(
    TARGET lsr POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LUA_INSTALL_PATH}"
    COMMAND ${CMAKE_COMMAND} -E copy "${LUA_SCRIPT}" "${LUA_INSTALL_PATH}"
    COMMENT "Copying lsr.lua to ${LUA_INSTALL_PATH}"
)

install(TARGETS lsr DESTINATION /usr/local/bin)
install(FILES ${LUA_SCRIPT} DESTINATION ${LUA_INSTALL_PATH})

if(EXISTS "${CMAKE_BINARY_DIR}/cmake_install.cmake")
    set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake
)
