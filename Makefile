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
CFLAGS_TEST = -DTEST

# define any directories containing header files other than /usr/include
INCLUDES = -I./include
INCLUDES_TEST =

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -Lusr/lib/gcc
LFLAGS_TEST = -L/usr/local/lib -g

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS =
LIBS_TEST = -lcmocka

# define the main source file
SRC_MAIN = ./src/main.c
SRC_MAIN_TEST = ./test/main.test.c

# define the C source files
SRCS = \
	./src/mem.c \
	./src/data_structures/doublyLinkedList.c \
	./src/data_structures/quad_tree.c \
	./src/data_structures/tree.c \
	./src/physics.c

SRCS_TEST = \
	./test/data_structures/doublyLinkedList.test.c \
	./test/data_structures/tree.test.c \
	./test/data_structures/quadTree.test.c \
	./test/loaders/lvl_loader.test.c \
	./test/physics.test.c

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJ_MAIN = $(SRC_MAIN:.c=.o) 
OBJ_MAIN_TEST = $(SRC_MAIN_TEST:.c=.o) 
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

build: $(OBJ_MAIN) $(OBJS)
		mkdir $(BUILD_DIR)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$(MAIN) $(OBJ_MAIN) $(OBJS) $(LFLAGS) $(LIBS)

test: $(OBJ_MAIN_TEST) $(OBJS) $(OBJS_TEST)
		mkdir $(BUILD_DIR)
		$(CC) $(CFLAGS) $(CFLAGS_TEST) $(INCLUDES) $(INCLUDES_TEST) -o $(BUILD_DIR)/$(MAIN) \
		$(OBJ_MAIN_TEST) $(OBJS) $(OBJS_TEST) $(LFLAGS) $(LFLAGS_TEST) $(LIBS) $(LIBS_TEST)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
ifeq ($(MAKECMDGOALS),test)
		$(CC) $(CFLAGS) $(CFLAGS_TEST) $(INCLUDES) -c $< -o $@
else
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
endif

clean:
		find . -type f \( -iname '*.o' -o -iname '*.out' \) -exec rm {} +
		rm -rf $(BUILD_DIR)

depend: $(SRC_MAIN) $(SRC_MAIN_TEST) $(SRCS) $(SRCS_TEST)
		makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

src/main.o: /usr/include/stdio.h
test/main.test.o: /usr/include/setjmp.h /usr/include/features.h
test/main.test.o: /usr/include/stdc-predef.h /usr/include/stdio.h
test/main.test.o: /usr/include/stdlib.h /usr/include/alloca.h
test/main.test.o: /usr/include/unistd.h
test/main.test.o: test/./data_structures/doublyLinkedList.test.h
test/main.test.o: test/./data_structures/quadTree.test.h
test/main.test.o: test/./data_structures/tree.test.h
test/main.test.o: test/./loaders/lvl_loader.test.h test/./physics.test.h
src/mem.o: /usr/include/stdlib.h /usr/include/alloca.h
src/mem.o: /usr/include/features.h /usr/include/stdc-predef.h
src/data_structures/doublyLinkedList.o: /usr/include/assert.h
src/data_structures/doublyLinkedList.o: /usr/include/features.h
src/data_structures/doublyLinkedList.o: /usr/include/stdc-predef.h
src/data_structures/doublyLinkedList.o: /usr/include/stdlib.h
src/data_structures/doublyLinkedList.o: /usr/include/alloca.h
src/data_structures/doublyLinkedList.o: src/data_structures/./doublyLinkedList.h
src/data_structures/doublyLinkedList.o: src/defs.h src/mem.h
src/data_structures/quad_tree.o: /usr/include/assert.h
src/data_structures/quad_tree.o: /usr/include/features.h
src/data_structures/quad_tree.o: /usr/include/stdc-predef.h
src/data_structures/quad_tree.o: /usr/include/ctype.h /usr/include/endian.h
src/data_structures/quad_tree.o: /usr/include/stdlib.h /usr/include/alloca.h
src/data_structures/quad_tree.o: /usr/include/string.h /usr/include/strings.h
src/data_structures/quad_tree.o: src/defs.h src/mem.h
src/data_structures/quad_tree.o: src/data_structures/./doublyLinkedList.h
src/data_structures/quad_tree.o: src/data_structures/./quad_tree.h
src/data_structures/quad_tree.o: /usr/include/limits.h
src/data_structures/quad_tree.o: src/data_structures/././tree.h
src/data_structures/quad_tree.o: src/data_structures/././../obj.h
src/data_structures/tree.o: /usr/include/assert.h /usr/include/features.h
src/data_structures/tree.o: /usr/include/stdc-predef.h /usr/include/stdio.h
src/data_structures/tree.o: /usr/include/string.h /usr/include/strings.h
src/data_structures/tree.o: src/defs.h src/mem.h /usr/include/stdlib.h
src/data_structures/tree.o: /usr/include/alloca.h src/data_structures/tree.h
src/data_structures/tree.o: src/data_structures/./doublyLinkedList.h
src/data_structures/tree.o: src/data_structures/././../obj.h
test/data_structures/doublyLinkedList.test.o: /usr/include/setjmp.h
test/data_structures/doublyLinkedList.test.o: /usr/include/features.h
test/data_structures/doublyLinkedList.test.o: /usr/include/stdc-predef.h
test/data_structures/doublyLinkedList.test.o: /usr/include/stdlib.h
test/data_structures/doublyLinkedList.test.o: /usr/include/alloca.h src/mem.h
test/data_structures/doublyLinkedList.test.o: src/data_structures/doublyLinkedList.h
test/data_structures/tree.test.o: /usr/include/setjmp.h
test/data_structures/tree.test.o: /usr/include/features.h
test/data_structures/tree.test.o: /usr/include/stdc-predef.h
test/data_structures/tree.test.o: /usr/include/stdio.h /usr/include/stdlib.h
test/data_structures/tree.test.o: /usr/include/alloca.h /usr/include/string.h
test/data_structures/tree.test.o: /usr/include/strings.h
test/data_structures/tree.test.o: src/data_structures/tree.h
test/data_structures/tree.test.o: src/data_structures/./doublyLinkedList.h
test/data_structures/tree.test.o: src/data_structures/././../obj.h
test/data_structures/tree.test.o: src/data_structures/doublyLinkedList.h
test/data_structures/tree.test.o: src/defs.h src/mem.h
test/data_structures/quadTree.test.o: /usr/include/setjmp.h
test/data_structures/quadTree.test.o: /usr/include/features.h
test/data_structures/quadTree.test.o: /usr/include/stdc-predef.h
test/data_structures/quadTree.test.o: /usr/include/stdlib.h
test/data_structures/quadTree.test.o: /usr/include/alloca.h src/mem.h
test/data_structures/quadTree.test.o: src/data_structures/quad_tree.h
test/data_structures/quadTree.test.o: /usr/include/limits.h
test/data_structures/quadTree.test.o: src/data_structures/./doublyLinkedList.h
test/data_structures/quadTree.test.o: src/data_structures/././tree.h
test/data_structures/quadTree.test.o: src/data_structures/././../obj.h
test/loaders/lvl_loader.test.o: /usr/include/errno.h /usr/include/features.h
test/loaders/lvl_loader.test.o: /usr/include/stdc-predef.h
test/loaders/lvl_loader.test.o: /usr/include/stdio.h /usr/include/string.h
test/loaders/lvl_loader.test.o: /usr/include/strings.h /usr/include/setjmp.h
test/loaders/lvl_loader.test.o: /usr/include/stdlib.h /usr/include/alloca.h
test/physics.test.o: /usr/include/setjmp.h /usr/include/features.h
test/physics.test.o: /usr/include/stdc-predef.h /usr/include/stdlib.h
test/physics.test.o: /usr/include/alloca.h src/mem.h
