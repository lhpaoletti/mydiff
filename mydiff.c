#include <stdio.h>


void trace(char *str) {
    fprintf(stderr, "%s\n", str);
    fflush(stderr);
}


void usage(void) {
    fprintf(stderr, "mydiff [-i] [-o outfile] file1 file2\n");
}


void abort_mydiff(char *str) {
    fprintf(stderr, "%s.\n", str);
    usage();
}


int main(int argc, char** argv) {
    printf("Hello world\n");
    return 0;
}
