cmake_minimum_required(VERSION 3.9)
project(BigClock C)

add_application(BigClock
    TYPE "APPL"
    CREATOR "BgCk"
    main.c
    icon.r
)

set_target_properties(BigClock PROPERTIES COMPILE_OPTIONS -O2)
