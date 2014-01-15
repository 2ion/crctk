# README

crctk is a toolkit that helps making using CRC32 as a checksum algorithm
for your files easier. As such, it provides functionality as described
in the following.

## md5sum-style checksum verification

crctk can calculate the checksum of a batch of files and store the
mappings filename---CRC32 in a database file.

Note that other than the above-mentioned classical md5sum-style checksum
tools, crctk supports storing arbitrary characters in filenames
including newlines, except '\0' and '/' which are not allowed according
to POSIX. For that purpose, it usese the binary format of the extremely
fast and compact [tincycdb library][], which is an optimized
implementation of [cdb][] as created by Daniel J. Bernstein.

[tinycdb library]: http://www.corpit.ru/mjt/tinycdb.html
[cdb]: http://cr.yp.to/cdb.html

### Creating a database file

```sh
./crctk -C <db-file> <file-list>
```

### Verifying the entries of a datbase file

```sh
./crctk -V <db-file> [<file-list>]
```

If <file-list> is not present, go through the entries in <db-file>,
check if a file is present under the path stored in the database and
if yes, try veryfing this file against the stored checksum.
If <file-list> is present, take the basename of the paths stored in
the database, and for every entry in <file-list>, check if a
filename matches and if yes, verify the matching file against the
checksum stored in the database.

## Working with CRC32 hexstring tags


(To be written...)


[1] http://www.corpit.ru/mjt/tinycdb.html
[2] http://cr.yp.to/cdb.html

## Usage

```
crctk v0.2-95ab1e0
CRC32 Hexstring Toolkit
Copyright (C) 2014 2ion (asterisk!2ion!de)
Upstream: https://github.com/2ion/crctk
Usage: crctk [-vVcCoptsreh] <file>|<file-list>
Options:
 -v Compute CRC32 and compare with the hexstring
    in the supplied filename.
    Return values: EXIT_SUCCESS: match
                   EXIT_FAILURE: no match
                   0xA: invalid argument
                   0xB: regex compilation error
                   0xC: unknown error
 -V FILE. Read checksums and filenames from a FILE
    created by the -C option and check if the files
    have the listed checksums.
 -c Compute the CRC32 of the given file, print and exit.
 -C for multiple input files, create a checksum listing
    for use with the -V option.
 -p FILE. Print the contents of a file created by the -C
    options to stdout.
 -t Tag file with a CRC32 hexstring. Aborts if
    the filename does already contain a tag.
 -s Supplements -t: strip eventually existing tag
    and compute a new CRC32 hexstring.
    Return values: EXIT_SUCCESS: success
                   EXIT_FAILURE: generic failure
                   Rest as above.
 -r If the file is tagged, remove the tag.
 -e EXPR. Changes the regular expression used to
    match tags when doing -s|-r to EXPR. Default:
    [[:punct:]]\?[[:xdigit:]]\{8\}[[:punct:]]\?
 -h Print this message and exit successfully.
```
