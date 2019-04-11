// Global definitions
//
// (c) Tuomas Koskimies, 2018

#define OS_AMIGA 0
#define DEBUG
//#define LOGGING

// The program preserves consistency in a situations where it is not necessary.
// For example, when clearing up a list, the links can be broken up; At the end
// the list will be empty.
#define PRESERVE_CONSISTENCY 1
