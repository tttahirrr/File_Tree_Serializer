README.md


# File Tree Serializer
# Overview
This project implements a command-line utility in C that:
Serializes a tree of files/directories into a byte sequence.
Deserializes such a sequence to recreate the original file tree.
Inspired by tools like cpio, it’s designed to help understand C I/O, pointer arithmetic, bitwise manipulation, and file system traversal.

# Features
Serialization (-s): Converts a directory tree into a binary stream.
Deserialization (-d): Rebuilds the original tree from the binary stream.
Clobber (-c): Allows overwriting existing files.
Help (-h): Displays usage instructions.
All operations are run via the bin/transplant executable.

# Tech Highlights
C only — no dynamic memory allocation or array indexing ([]) allowed.
Raw command-line parsing using argc, argv — no getopt() or string libraries.
Uses Criterion for unit testing.

# Build Instructions
Run from the hw1 directory:
make          # Build release version
make debug    # Build with debugging options
make clean    # Clean build artifacts


# Usage Examples
Serialize a directory:
bin/transplant -s -p rsrc/testdir > outfile

Deserialize into a directory:
bin/transplant -d -p outputdir < outfile


# Running Tests
Run unit tests:
bin/transplant_tests
# Verbose output:
bin/transplant_tests --verbose=0


