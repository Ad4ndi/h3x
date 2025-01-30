#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>

#define DEFAULT_COLS 16
#define DEFAULT_GROUPS 2

void help() {
    printf("Usage:\n"
           "   h3x [options] [infile [outfile]]\n"
           "Options:\n"
           "   -h          print help message\n"
           "   -c cols     format <cols> octets per line (default 16)\n"
           "   -u          use uppercase hex letters\n"
           "   -r          convert hex into binary\n"
           "   -g bytes    number of octets per group\n"
           "   -v          show version\n"
           "   -p          plain style\n"
           "   -l len      stop after <len> octets\n");
    exit(0);
}

void hex(FILE *f, int cols, int uppercase, int groups, int postscript, size_t len) {
    unsigned char buffer[DEFAULT_COLS];
    size_t offset = 0, read_bytes;
    
    while ((read_bytes = fread(buffer, 1, (len < cols ? len : cols), f)) > 0 && len > 0) {
        if (!postscript) printf("%08lx: ", offset);
        
        for (size_t i = 0; i < cols; i++) {
            if (i % groups == 0 && i > 0) printf(" ");
            if (i < read_bytes)
                printf(uppercase ? "%02X " : "%02x ", buffer[i]);
            else
                printf("   ");
        }
        
        if (!postscript) {
            printf(" |");
            for (size_t i = 0; i < read_bytes; i++)
                printf("%c", isprint(buffer[i]) ? buffer[i] : '.');
            printf("|\n");
        } else {
            printf("\n");
        }
        
        offset += cols;
        len -= read_bytes;
    }
}

void reverse(FILE *f, FILE *out, size_t len) {
    unsigned char byte;
    while (len > 0 && fscanf(f, "%2hhx", &byte) == 1) {
        fwrite(&byte, 1, 1, out);
        len--;
    }
}

int main(int argc, char *argv[]) {
    int opt, cols = DEFAULT_COLS, upcase = 0, rev = 0, groups = DEFAULT_GROUPS, postscript = 0;
    size_t len = SIZE_MAX;
    
    while ((opt = getopt(argc, argv, "huc:rg:vl:p")) != -1) {
        switch (opt) {
            case 'h': help(); break;
            case 'c': cols = atoi(optarg); break;
            case 'u': upcase = 1; break;
            case 'r': rev = 1; break;
            case 'g': groups = atoi(optarg); break;
            case 'v': printf("h3x v1.2 by Ad4ndi\n"); exit(0);
            case 'p': postscript = 1; break;
            case 'l': len = strtoul(optarg, NULL, 10); break;
            default: exit(1);
        }
    }
    
    FILE *f = stdin, *out = stdout;
    
    if (optind < argc) {
        f = fopen(argv[optind], rev ? "r" : "rb");
        if (!f) { perror("Error opening input file"); return 1; }
    }
    
    if (rev && (optind + 1 < argc)) {
        out = fopen(argv[optind + 1], "wb");
        if (!out) { perror("Error opening output file"); fclose(f); return 1; }
    }
    
    if (rev) {
        reverse(f, out, len);
    } else {
        hex(f, cols, upcase, groups, postscript, len);
    }
    
    fclose(f);
    if (out != stdout) fclose(out);
    
    return 0;
}
