cmake_minimum_required(VERSION 3.6)
project (Test_fsm_engine)

enable_testing()


#Search for check.h
find_path(INCLUDE_DIR_CHECK NAMES check.h)

include_directories(${INCLUDE_DIR_CHECK}
                    ../inc
                    external/fifo/inc)

#Add HEAD project, FUT = File Under Test
add_subdirectory("../" FUT)
add_subdirectory("external/fifo")

#add_subdirectory(../
#                 FUT
#                 external/fifo)


#Set required libraries
find_library(LIBRARY_SUBUNIT NAMES subunit)

set(MyLIBS check m rt pthread)

if(LIBRARY_SUBUNIT)
  set(MyLIBS ${MyLIBS} ${LIBRARY_SUBUNIT})
  #message ("*** SUBUNIT FOUND ***")
else(LIBRARY_SUBUNIT)
  message ("*** Library 'subunit' NOT found! ***")
endif(LIBRARY_SUBUNIT)

foreach(x ${MyLIBS})
  message("*** MyLibs: " ${x})
endforeach(x ${MyLIBS})

#create executable
# ENTER [TEST NAME] and [SOURCE FILES]
add_executable(test_myApp fsm_test.c)


message("LD_LIBRARY_PATH: " ${LD_LIBRARY_PATH})


# ENTER [TEST NAME] and [APPLICATION NAME, same as in CMakeList.txt in application directory]
target_link_libraries(test_myApp fsm_engine ${MyLIBS} )


# ENTER [PROJECT NAME] and [TEST NAME]
add_test(TEST_myApp test_myApp)
