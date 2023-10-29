#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>


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


int parse_arguments(
        const int argc, char *const *argv,
        const char **outfile, bool *const i_opt,
        const char **file1, const char **file2
) {
    char opt;
    while ((opt = getopt(argc, argv, "io:")) != -1) {
        switch (opt) {
        case 'i':
            *i_opt = true;
            break;
        case 'o':
            if (strcmp(optarg, "--") != 0) {
                *outfile = optarg;
            } else {
                abort_mydiff("The option -o requires one argument");
                return 1;
            }
            break;

        case '?':
        default:
            if (optopt == 'o') {
                abort_mydiff("The option -o requires one argument");
            } else if (isprint(optopt)) {
                // FIXME try to figure out a better solution for this
                fprintf(stderr, "The option -%c is not supported.\n", optopt);
                usage();
            } else {
                abort_mydiff("An unrecognizable character was given as option");
            }
            return 1;
        }
    }

    int posargs_num = argc - optind;
    if (posargs_num != 2) {
        switch (posargs_num) {
            case 0:
                abort_mydiff("The program requires 2 positional arguments");
                break;
            case 1:
                abort_mydiff("file2 is missing");
                break;
            default:
                abort_mydiff("More than 2 file names were given");
        }
        return 1;
    } else {
        *file1 = argv[optind];
        *file2 = argv[optind + 1];
    }

    return 0;
}


int open_files(
        const char *file_name1, FILE **file1,
        const char *file_name2, FILE **file2,
        const char *outfile_name, FILE **outfile
) {
    if (outfile_name == NULL) {
        *outfile = stdout;
    } else {
        *outfile = fopen(outfile_name, "w");
    }
    *file1 = fopen(file_name1, "r");
    *file2 = fopen(file_name2, "r");

    if (*file1 == NULL && *file2 == NULL) {
        fprintf(stderr, "Both files given failed to open: %s, %s\n", file_name1, file_name2);
        return 1;
    } else if (*file1 == NULL) {
        fprintf(stderr, "The file1 failed to open: %s\n", file_name1);
        return 1;
    } else if (*file2 == NULL) {
        fprintf(stderr, "The file2 failed to open: %s\n", file_name2);
        return 1;
    }
    if (*outfile == NULL) {
        fprintf(stderr, "The outfile failed to open: %s\n", outfile_name);
        return 1;
    }
    return 0;
}


int main(int argc, char** argv) {

    bool i_opt = false;
    const char *outfile_name = NULL;
    const char *file_name1, *file_name2;

    int parse_result = parse_arguments(
            argc, argv,
            &outfile_name, &i_opt,
            &file_name1, &file_name2);
    if (parse_result != 0) {
        return parse_result;
    }

    FILE *file1, *file2, *outfile;
    int open_result = open_files(
            file_name1, &file1,
            file_name2, &file2,
            outfile_name, &outfile);
    if (open_result != 0) {
        return open_result;
    }



    return 0;
}
