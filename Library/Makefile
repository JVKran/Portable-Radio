#############################################################################
#
# Project Makefile
#
# (c) Wouter van Ooijen (www.voti.nl) 2016
#
# This file is in the public domain.
# 
#############################################################################

# source files in this project (main.cpp is automatically assumed)	
SOURCES := DS3231.cpp TEA5767.cpp KY040.cpp A24C256.cpp Radio.cpp RDA5807.cpp ../Application/GUI.cpp radioDataSystem.cpp timeDateData.cpp

# header files in this project
HEADERS := DS3231.hpp TEA5767.hpp KY040.hpp A24C256.hpp Radio.hpp RDA5807.hpp ../Application/GUI.hpp radioDataSystem.hpp timeDateData.hpp

# other places to look for files for this project
SEARCH  := DS3231 Radio KY040 24C256

# set RELATIVE to the next higher directory 
# and defer to the appropriate Makefile.* there
RELATIVE := ..
include $(RELATIVE)/Makefile.due
