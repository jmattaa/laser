cmake_minimum_required(VERSION 3.5)

project(laser LANGUAGES C)
include_directories("./include")
add_executable(laser src/main.c
    include/laser.h
    src/laser.c
    include/colors.h
    include/utils.h
    src/utils.c
    include/sort.h
    src/sort.c
    include/git/lgit.h
    src/git/lgit.c
    include/git/lgit_utils.h
    src/git/lgit_utils.c

)
