#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
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
    int i_count = 0;
    char opt;
    while ((opt = getopt(argc, argv, "io:")) != -1) {
        switch (opt) {
        case 'i':
            i_count++;
            *i_opt = true;
            if (i_count > 1) {
                abort_mydiff("The option -i can only be given once");
                return 1;
            }
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


ssize_t min(ssize_t this, ssize_t that) {
    return (this < that) ? this : that;
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


    int diff_count;
    int line_count = 0;
    char *line1 = NULL;
    char *line2 = NULL;
    char *char1, *char2;
    size_t line1_cap = 0;
    size_t line2_cap = 0;
    ssize_t written1, written2;
    size_t diff_length;

    do {
        written1 = getline(&line1, &line1_cap, file1);
        written2 = getline(&line2, &line2_cap, file2);
        if (written1 <= 0 || written2 <= 0) { break; }

        diff_count = 0;
        line_count++;
        if (written1 == 1 || written2 == 1) { continue; }

        // This cast is safe because of the break-condition.
        diff_length = (size_t) min(written1, written2);
        diff_length--;  // Ignore the newline character.

        for (size_t i = 0; i < diff_length; i++) {
            char1 = line1 + i;
            char2 = line2 + i;
            if (i_opt) {
                *char1 = tolower(*char1);
                *char2 = tolower(*char2);
            }
            if (*char1 != *char2) {
                diff_count++;
            }
        }
        if (diff_count > 0) {
            fprintf(outfile, "Line: %d, characters: %d\n", line_count, diff_count);
        }

    } while (true);


    free(line1);
    free(line2);
    return 0;
}
