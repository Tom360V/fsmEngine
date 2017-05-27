#------------------------------------------------------------------------------
DEP:=../queue

where-am-i = $(dir $(abspath $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))))

myINC:=inc \
../queue/inc \


mySRC:=fsm.c

include ../../makefile.def

whattobuild: DEPP $(OBJ) $(LIBNAME)

