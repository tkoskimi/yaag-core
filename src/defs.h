// Global definitions
//
// (c) Tuomas Koskimies, 2018

#define OS_AMIGA
#define DEBUG
//#define LOGGING

// The size of the coordinates in bits.
#define COORDINATE_SIZE_IN_BITS 32

// The program preserves consistency in a situations where it is not necessary.
// For example, when clearing up a list, the links can be broken up; At the end
// the list will be empty.
#define PRESERVE_CONSISTENCY 1
