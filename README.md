# README

crctk is a toolkit that helps making using CRC32 as a checksum algorithm
for your files easier. As such, it provides functionality as described
in the following.

Stable releases are tagged in Git. Everything else is experimental!

## Compilation and installation

crctk depends on:

- libz
- libcdb/libtinycdb

On Debian, these may be installed using
```sh
sudo apt-get install libcdb-dev zlib1g-dev
```

```sh
make
make README
make install prefix=$HOME/bin
```

where $prefix is the directory the binary will be copied to.

## md5sum-style checksum verification

crctk can calculate the checksum of a batch of files and store the
mappings filename---CRC32 in a database file.

Note that other than the above-mentioned classical md5sum-style checksum
tools, crctk supports storing arbitrary characters in filenames
including newlines, except '\0' and '/' which are not allowed according
to POSIX. For that purpose, it usese the binary format of the extremely
fast and compact [tinycdb][], which is an optimized
implementation of [cdb][] as created by Daniel J. Bernstein.

[tinycdb]: http://www.corpit.ru/mjt/tinycdb.html
[cdb]: http://cr.yp.to/cdb.html

## -c: Plain old calculating of the CRC32 checksum

```sh
./crctk -c [-n] -- FILE
```

Calculate the CRC32 checksum of the FILE, print and exit.

The modifier -n makes it print the checksum in its numerical format
(unsigned long).

## Checksum databases

### -C: Creating a database file

```sh
./crctk -C DB-FILE -- FILE-LIST
```

### -V: Verifying the entries of a datbase file

```sh
./crctk -V DB-FILE -- [FILE-LIST]
```

If FILE-LIST is not present, go through the entries in DB-FILE,
check if a file is present under the path stored in the database and
if yes, try veryfing this file against the stored checksum.
If FILE-LIST is present, take the basename of the paths stored in
the database, and for every entry in FILE-LIST, check if a
filename matches and if yes, verify the matching file against the
checksum stored in the database.

## Working with CRC32 hexstring tags

CRC32 hexstring tags look like this:
```
[EAA21973]
```
and in crctk are defined as a sequence of 8 characters from the
character class \[a-fA-F0-9\], optionally surrounded by punction from the
character class \[:punct:\] (See the section on Usage further down for
details). They may be put in filenames where they serve
the same purpose as checksum databases, that is delivering the checksum
information of the file they are tagging. crctk provides tools to
create, remove and verify such tags.

### -v: Verifying CRC32 hexstring tags

```sh
./crctk -v -- FILE
```
Look for a tag in the supplied FILE's filename and if it exists,
calculate the CRC32 sum of the file and check if it matches the one in
the tag.

### -r: Stripping CRC32 hexstring tags

```sh
./crctk -r -- FILE
```

Remove any hexstring tag in the FILE's filename, including surrounding
punctuation.

#### TODO

Remove only matching punctuation characters (like open- and closed
brackets, but not two open brackets).

### -t: Create a CRC32 hexstring tag

```sh
./crctk -t [-s] -- FILE
```

Compute FILE's CRC32 checksum and append a tag in the format \[$TAG\] to
the FILE's filename. If the filename has an extension (\*.\*), the tag
will be inserted before the last dot.

If the -s modifier was specified, strip any existing tags before
tagging.

#### TODO

Look out for extensions containing multiple dots, like .tar.gz.

## Other options

See the section on Usage for more available options.

## -h: Usage

More concise usage information.

```
crctk v0.3.2-0b76cea (Feb  7 2014 21:41:52)
CRC32 Hexstring Toolkit
Copyright (C) 2014 2ion (asterisk!2ion!de)
Upstream: https://github.com/2ion/crctk
Usage: ./crctk [-aCcefhnprstVv] <file>|<file-listing>
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
