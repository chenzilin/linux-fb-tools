project(fb2snap)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. DIR_SRCS)
add_executable(fb2snap ${DIR_SRCS})

target_link_Libraries(fb2snap png)
target_link_Libraries(fb2snap jpeg)

install(TARGETS fb2snap DESTINATION bin)
