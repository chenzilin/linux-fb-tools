project(fbshow)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. DIR_SRCS)
add_executable(fbshow ${DIR_SRCS})

target_link_Libraries(fbshow png)
target_link_Libraries(fbshow jpeg)

install(TARGETS fbshow DESTINATION bin)
