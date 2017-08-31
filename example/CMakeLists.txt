cmake_minimum_required(VERSION 3.6)
project (fsm_Example)

#Add include directories
include_directories(inc
                    external/fifo/inc)

#Search in following directories for other CMakeList.txt
add_subdirectory("../" FSM)
add_subdirectory("external/fifo")

#Add sources
set(srcs main.c)

#Add Library
add_executable(fsm_Example ${srcs})
target_link_libraries(fsm_Example fifo fsm_engine)
