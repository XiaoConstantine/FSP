.SUFFIXES:.cc
.cc.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

CC = g++
INCLUDES=-I.
CFLAGS=-Wall -pg
# # where your object files are (. is the same directory as the makefile)
LDFLAGS=-L.
#
# # when "make" is typed at command line with no args "default" will get executed and do "all"
default: all
#
# #
# # assign source and headers for our "utility" lib to variables below
# #
# # replace "SBB" (my initials) with yours or use the same filenames
LIB_UTIL_SRC = XC_util.cc XC_io.cc XC_date.cc XC_ratings.cc XC_calculator.cc XC_tradeBook.cc XC_yieldCurve.cc
LIB_UTIL_SRC_HDR = XC_util.h XC_io.h XC_date.h XC_ratings.h XC_calculator.h XC_tradeBook.h XC_yieldCurve.h
LIB_UTIL_OBJ = $(LIB_UTIL_SRC:.cc=.o)
$(LIB_UTIL_OBJ) : $(LIB_UTIL_SRC_HDR)
LIB_UTIL_TARGET = libutil.a
# # the file where main() is
MAIN_SRC =mmain.cc
MAINOBJ = $(MAIN_SRC:.cc=.o) 
$(MAINOBJ) :$(LIB_TARGET)$(LIB_UTIL_TARGET)
MAINTARGET=testdriver 
#
ARCHIVE_OPTIONS = rucsv
#
$(LIB_UTIL_TARGET) : $(LIB_UTIL_OBJ)
	ar $(ARCHIVE_OPTIONS) $(LIB_UTIL_TARGET) $(LIB_UTIL_OBJ)

$(MAINTARGET) : $(MAINOBJ) $(LIB_TARGET) $(LIB_UTIL_TARGET)
	$(CC) $(LDFLAGS) $(MAINOBJ) $(LIB_UTIL_TARGET) $(LIB_TARGET) json_spirit.a -o $(MAINTARGET)

all : util test
test: $(MAINTARGET)
#
util: $(LIB_UTIL_TARGET)
#
#
clean:
	rm -f *.o 
#
clobber:
	-rm -f *.o *.a $(MAINTARGET)
#
backup: clobber
	tar cvf ./nyu.tar *.*""""""""""
