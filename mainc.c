#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

void help() {
    printf("Usage:\n"
		   "   h3x [options] [infile [outfile]]\n"
		   "Options:\n"
           "   -h          print help message\n"
           "   -c cols     format <cols> octets per line (default 16)\n"
           "   -u          use upcase hex letters\n"
           "   -r          convert hex into binary\n"
           "   -g bytes    number of octets per group\n"
           "   -v          show version\n"
           "   -p          plain style\n"
           "   -l len      stop after <len> octets\n");
    exit(0);
}

void hex(FILE *f, int cols, int uppercase, int groups, int postscript) {
    unsigned char b[16];
    size_t i = 0, j;
    while ((j = fread(b, 1, cols, f)) > 0) {
        if (!postscript) printf("%08lx: ", i);
        for (size_t k = 0; k < cols; k++) {
            if (k % groups == 0 && k > 0) printf(" ");
            if (k < j) printf(uppercase ? "%02X " : "%02x ", b[k]);
            else printf("   ");
        }
        if (!postscript) {
            printf(" |");
            for (size_t k = 0; k < j; k++) printf("%c", isprint(b[k]) ? b[k] : '.');
            printf("|\n");
        } else printf("\n");
        i += cols;
    }
}

void reverse(FILE *f, FILE *out) {
    unsigned char b[1];
    while (fscanf(f, "%2hhx", &b[0]) == 1) fwrite(b, 1, 1, out);
}

int main(int argc, char *argv[]) {
    int opt, cols = 16, upcase = 0, rev = 0, groups = 2, postscript = 0;
    while ((opt = getopt(argc, argv, "huc:rig:vl:p")) != -1) {
        if (opt == 'h') help();
        if (opt == 'c') cols = atoi(optarg);
        if (opt == 'u') upcase = 1;
        if (opt == 'r') rev = 1;
        if (opt == 'g') groups = atoi(optarg);
        if (opt == 'v') { printf("h3x v1.1 by Ad4ndi\n"); exit(0); }
        if (opt == 'p') postscript = 1;
    }

    FILE *f = stdin, *out = stdout;
    if (optind < argc) f = fopen(argv[optind], rev ? "r" : "rb");
    if (!f) { perror("Error opening input file"); return 1; }

    if (rev && (optind + 1 < argc)) out = fopen(argv[optind + 1], "wb");
    if (!out) { perror("Error opening output file"); fclose(f); return 1; }

    rev ? reverse(f, out) : hex(f, cols, upcase, groups, postscript);

    fclose(f); fclose(out);
    return 0;
}
