#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <cmocka.h>

#include "./data_structures/doublyLinkedList.test.h"
#include "./data_structures/quadTree.test.h"
#include "./data_structures/tree.test.h"
#include "./loaders/lvl_loader.test.h"
#include "./physics.test.h"

int main(int argc, char* argv[]) {
    opterr = 0;
    char *dirvalue = NULL;
    int c;

    while ((c = getopt (argc, argv, "d:")) != -1) {
        switch (c) {
            case 'd':
                dirvalue = optarg;
                break;
            case '?':
            if (optopt == 'd')
                fprintf (\
                    stderr,\
                    "Option -%c requires a dir for the assets.\n",\
                    optopt\
                );
            else
                fprintf (\
                    stderr,\
                    "Unknown option character `\\x%x'.\n",\
                    optopt\
                );
                return 1;
            default:
                abort ();
        }
    }
	// Tests should be added here.
	dbll_test();
    tree_test();
    qtree_test();
    physics_test();
	//lvl_loader_test(dirvalue);
}
