# h3x - minimal hexdump CLI utility

### Usage:
```
h3x [options] [infile [outfile]]
```

### Oprtions:
```
-h          print help message
-c cols     format <cols> octets per line (default 16)
-u          use upcase hex letters
-r          convert hex into binary
-g bytes    number of octets per group
-v          show version
-p          plain style
-l len      stop after <len> octets
```

### Install
```
$ git clone https://github.com/Ad4ndi/h3x/
$ cd h3x
$ clang main.c -o h3x
$ sudo cp h3x /usr/bin
```
