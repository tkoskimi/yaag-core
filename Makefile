#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
CC = gcc

# define any compile-time flags
CFLAGS = -Wextra -g
CFLAGS_TEST = 

# define any directories containing header files other than /usr/include
INCLUDES = -I./include

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -Lusr/lib/gcc
LFLAGS_TEST = -L/usr/local/lib

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS =
LIBS_TEST = -lcmocka

# define the C source files
SRCS = ./src/main.c ./src/data_structures/doublyLinkedList.c
SRCS_TEST = ./test/main.test.c ./test/data_structures/doublyLinkedList.test.c

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o)
OBJS_TEST = $(SRCS_TEST:.c=.o)

# define the executable file 
MAIN = yaag

# define the build directory
BUILD_DIR = build

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

# make will not expect file to be created for these targets
.PHONY:	depend clean test

all: $(MAIN)
		@echo  YAAG has been compiled

build: $(OBJS)
		mkdir $(BUILD_DIR)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

test: $(OBJS_TEST)
		mkdir $(BUILD_DIR)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(MAIN) $(OBJS_TEST) $(LFLAGS_TEST) $(LIBS_TEST)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
		find . -type f \( -iname '*.o' -o -iname '*.out' \) -exec rm {} +
		rm -rf $(BUILD_DIR)

depend: $(SRCS) $(SRCS_TEST)
		makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

src/main.o: /usr/include/stdio.h
src/data_structures/doublyLinkedList.o: /usr/include/stdlib.h src/defs.h
test/main.test.o: /usr/include/setjmp.h /usr/include/features.h
test/main.test.o: /usr/include/stdc-predef.h
test/main.test.o: test/./data_structures/doublyLinkedList.test.h
test/data_structures/doublyLinkedList.test.o: /usr/include/setjmp.h
test/data_structures/doublyLinkedList.test.o: /usr/include/features.h
test/data_structures/doublyLinkedList.test.o: /usr/include/stdc-predef.h
