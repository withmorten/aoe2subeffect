cmake_minimum_required(VERSION 3.10)
project(aoe2subeffect VERSION 1.0)
add_subdirectory(genieutils EXCLUDE_FROM_ALL)
include_directories(
        "genieutils/include"
)
add_executable(aoe2subeffect aoe2subeffect.cpp)
target_link_libraries(aoe2subeffect genieutils)
