# README

crctk is a toolkit that helps making using CRC32 as a checksum algorithm
for your files easier. As such, it provides functionality as described
in the following.

Stable releases are tagged in Git. Everything else is experimental!

## Compilation and installation

crctk depends on:

- libz
- libcdb/libtinycdb
- pandoc (markdown -> manpage converter)

On Debian, these may be installed using
```sh
sudo apt-get install libcdb-dev zlib1g-dev
```

```sh
make
# installs only the binary, no documentation
make install prefix=$HOME/bin
```
where $prefix is the directory the binary will be copied to.

## Documentation

The documentation is written in pandoc markdown and located in /doc, so you can build a 
wide range of documentation formats! By default, we deliver HTML and manpage versions.


## Outstanding Bugs

* The -a option currently somehow corrupts the database file if
  the same file is being added again.

## Command line synposis

More concise usage information.

```
crctk v0.3.3-94ded65 (Feb 17 2014 15:25:32)
CRC32 Hexstring Toolkit
Copyright (C) 2014 2ion (asterisk!2ion!de)
Upstream: https://github.com/2ion/crctk
Usage: ./crctk [-aCcefhnprstuVv] <file>|<file-listing>
Options:
 -v Compute CRC32 and compare with the hexstring
    in the supplied filename.
    Return values: EXIT_SUCCESS: match
                   EXIT_FAILURE: no match
 -V FILE. Read checksums and filenames from a FILE
    created by the -C option and check if the files
    have the listed checksums.
 -u HEXSTRING. Check if the specified file has the CRC
    sum HEXSTRING.
 -f Supplements -V. Instead of calculating the real CRC
    sum, use a CRC32 hexstring if the file is tagged.
 -x Supplements -V. For any tagged file, use the hexstring as
    as the reference CRC32 instead of computing anew.
 -c Compute the CRC32 of the given file, print and exit.
 -n Supplements -c. print CRC32 in its numerical format.
 -C for multiple input files, create a checksum listing
    for use with the -V option. Overwrites the given file.
 -a Supplements -C. Append to the given database file instead
    of overwriting it.
 -p FILE. Print the contents of a file created by the -C
    options to stdout.
 -t Tag file with a CRC32 hexstring. Aborts if
    the filename does already contain a tag.
 -s Supplements -t. strip eventually existing tag
    and compute a new CRC32 hexstring.
    Return values: EXIT_SUCCESS: success
                   EXIT_FAILURE: generic failure
 -r If the file is tagged, remove the tag.
 -e EXPR. Changes the regular expression used to
    match tags when doing -s|-r to EXPR. Default:
    [[:punct:]]\?[[:xdigit:]]\{8\}[[:punct:]]\?
 -h Print this message and exit successfully.```
