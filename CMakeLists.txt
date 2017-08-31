cmake_minimum_required(VERSION 2.8)
project (fsm_engine)

#Add include directories
include_directories("inc")

#Add sources
set(srcs src/fsm.c)

#Add Library
add_library(fsm_engine ${srcs})
target_link_libraries(fsm_engine fifo)
