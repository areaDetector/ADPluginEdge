TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# The following gets rid of the -fno-implicit-templates flag on vxWorks, 
# so we get automatic template instantiation.
# This is what we want for the source files that define template functions.
ifeq (vxWorks,$(findstring vxWorks, $(T_A)))
CODE_CXXFLAGS=
endif

DBD += NDPluginEdge.dbd

INC += NDPluginEdge.h

LIBRARY_IOC += NDPluginEdge

NDPluginEdge_SRCS += NDPluginEdge.cpp

USR_INCLUDES += $(OPENCV_INCLUDE)
ifdef OPENCV_LIB
  NDPluginEdge_DIR += $(OPENCV_LIB)
endif

include $(AREA_DETECTOR)/ADCore/ADApp/commonLibraryMakefile

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

