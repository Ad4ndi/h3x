import std.stdio;
import std.conv;
import std.string;
import std.getopt;
import std.file;
import std.ascii;
import std.algorithm;
import std.math;
import core.stdc.stdlib;

import config;

void printHelp() {
    writeln("Usage:");
    writeln("   h3x [options] [infile]");
    writeln("Options:");
    writeln("   -h          print help message");
    writeln("   -c cols     format <cols> octets per line (default ", DEF_COLS, ")");
    writeln("   -u          use uppercase hex letters");
    writeln("   -r          convert hex into binary");
    writeln("   -g bytes    number of octets per group (default ", DEF_GROUPS, ")");
    writeln("   -v          show version");
    writeln("   -p          plain style");
    writeln("   -l len      stop after <len> octets");
    exit(0);
}

void printVersion() {
    writeln("h3x v1.4 by Ad4ndi");
    exit(0);
}

void printHex(File f, int cols, bool uppercase, int groups, bool postscript, size_t len) {
    ubyte[DEF_COLS] buffer;
    size_t offset = 0;

    while (!f.eof && len > 0) {
        auto readBytes = f.rawRead(buffer[0 .. min(len, cols)]).length;

        if (!postscript) writef("%08x: ", offset);

        foreach (i; 0 .. cols) {
            if (i % groups == 0 && i > 0) write(" ");
            writef(i < readBytes ? (uppercase ? "%02X " : "%02x ") : "   ", buffer[i]);
        }

        if (!postscript) {
            write(" |");
            foreach (i; 0 .. readBytes) write(buffer[i].isPrintable ? cast(char) buffer[i] : '.');
            writeln("|");
        } else { writeln(); }

        offset += cols;
        len -= readBytes;
    }
}

void reverse(File f, size_t len) {
    string hexData;
    while (!f.eof && len > 0) { hexData ~= f.readln().strip().replace(" ", ""); }

    if (hexData.length % 2 != 0) {
        stderr.writeln("Error: Hex data has an odd number of characters.");
        return;
    }

    foreach (i; 0 .. hexData.length / 2) {
        try {
            ubyte byteValue = hexData[i * 2 .. i * 2 + 2].to!ubyte(16);
            stdout.rawWrite([byteValue]);
            len--;
            if (len == 0) break;
        } catch (ConvException) {
            stderr.writeln("Invalid hex input: ", hexData[i * 2 .. i * 2 + 2]);
        }
    }
}

void main(string[] args) {
    int cols = DEF_COLS, groups = DEF_GROUPS;
    bool upcase = false, rev = false, postscript = false;
    size_t len = size_t.max;

    getopt(args, 
        "h|help", &printHelp, 
        "c|cols", &cols, 
        "u|uppercase", &upcase, 
        "r|reverse", &rev, 
        "g|groups", &groups, 
        "v|version", &printVersion, 
        "p|plain", &postscript, 
        "l|len", &len
    );

    File f = args.length > 1 ? File(args[1], "rb") : stdin;

    if (rev) {
        reverse(f, len);
    } else {
        printHex(f, cols, upcase, groups, postscript, len);
    }
}
