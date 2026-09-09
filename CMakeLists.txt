cmake_minimum_required(VERSION 3.9)
project(BigClock C)

# Override with -DBIGCLOCK_VERSION=1.2.3 (CI passes the git tag).
set(BIGCLOCK_VERSION "0.0.0" CACHE STRING "BigClock version string")

# Split into numeric parts for the 'vers' resource (Finder Get Info).
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" _v "${BIGCLOCK_VERSION}")
if(_v)
    set(VERS_MAJOR ${CMAKE_MATCH_1})
    set(VERS_MINOR ${CMAKE_MATCH_2})
    set(VERS_PATCH ${CMAKE_MATCH_3})
else()
    set(VERS_MAJOR 0)
    set(VERS_MINOR 0)
    set(VERS_PATCH 0)
endif()
configure_file(version.r.in ${CMAKE_CURRENT_BINARY_DIR}/version.r @ONLY)

add_application(BigClock
    TYPE "APPL"
    CREATOR "BgCk"
    main.c
    icon.r
    ${CMAKE_CURRENT_BINARY_DIR}/version.r
)

target_compile_definitions(BigClock PRIVATE BIGCLOCK_VERSION="${BIGCLOCK_VERSION}")
set_target_properties(BigClock PROPERTIES COMPILE_OPTIONS -O2)
