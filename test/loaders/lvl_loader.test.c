#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>

#include <cmocka.h>

typedef struct {
    char *path;
    float x;
    float y;
    int active;
} t_data;
 
t_data t_rec;

FILE *fp;

static int lvl_loader_setup(void **state) {
    return 0;
}

static int lvl_loader_teardown(void **state) {
    return 0;
}

static void read_from_text(void **state) {
    t_rec.path = "hello";
    printf("\nmitä on: %s", t_rec.path);
    t_rec.path = calloc(100, sizeof(char));
    fp = fopen("./test/assets/t_rec.txt", "r");
    if (fp != NULL) {
        printf("löytyy!");
        fscanf(fp, "%s %f %f %d", t_rec.path, &t_rec.x, &t_rec.y, &t_rec.active);

        printf("\n");
        printf("path: %s\n", t_rec.path);
        printf("x: %f\n", t_rec.x);
        printf("y: %f\n", t_rec.y);
        printf("active: %d\n", t_rec.active);

        fclose(fp);
    }
    fprintf(stderr, "Value of errno: %d\n", errno);
    return;
}

static void clr(void **state) {
}

int lvl_loader_test(char *assets) {
    // Full file name
    char full_name[256];
    strcpy(full_name, assets);
    strcat(full_name, "t_rec.txt");
    printf("%s", full_name);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown( read_from_text, lvl_loader_setup, lvl_loader_teardown ),
    };

    return cmocka_run_group_tests( tests, NULL, NULL );
}
