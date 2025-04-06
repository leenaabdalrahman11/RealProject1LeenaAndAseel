cmake_minimum_required(VERSION 3.10)
project(GameMatch C)

set(CMAKE_C_STANDARD 99)

add_executable(GameMatch
        main.c
        common.h
        player.c
        player.h
        referee.c
        referee.h
        graphics.h
        graphics.c
)
