cmake_minimum_required(VERSION 2.8)
project (Test_fsm_engine)

enable_testing()


#Add include directories
include_directories("external/fifo/inc")
include_directories("../inc")


add_subdirectory("../" FUT)
add_subdirectory("external/fifo")

#Add Library
#add_library(fsm_engine SHARED ${srcs})
add_executable(test_fsm_engine fsm_test.c)
target_link_libraries(test_fsm_engine fsm_engine)

