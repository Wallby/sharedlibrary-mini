#ifndef SHAREDLIBRARY_MINI_H
#define SHAREDLIBRARY_MINI_H

#include <stdio.h>

#if defined(_WIN32)
// NOTE: https://gcc.gnu.org/onlinedocs/gcc/Microsoft-Windows-Function-Attributes.html
#define SM_EXPORT __declspec(dllexport)
#define SM_IMPORT __declspec(dllimport)
#define SM_SHAREDLIBRARY_EXTENSION ".dll"
#else //< #elif defined(__linux__)
// NOTE: https://stackoverflow.com/a/19666769
#define SM_EXPORT __attribute__((visibility ("default")))
#define SM_IMPORT
#define SM_SHAREDLIBRARY_EXTENSION ".so"
#endif


// void(*on_print)(char* a, FILE* b);
void sm_set_on_print(void(*a)(char*, FILE*));
void sm_unset_on_print();

// NOTE: returns 1 if succeeded
//       returns 0 if something went wrong
//       returns -1 if no code was executed
// NOTE: if succeeded.. loaded sharedlibrary will have been written to..
//       ..*sharedlibrary
int sm_load(char* filename, int* sharedlibrary);
#define sm_load2(filename, sharedlibrary) sm_load(filename SM_SHAREDLIBRARY_EXTENSION, sharedlibrary)
// NOTE: returns 1 if succeeded
//       returns -1 if no code was executed
int sm_unload(int sharedlibrary);

// NOTE: returns 1 if succeeded
//       returns 0 if something went wrong
//       returns -1 if no code was executed
// NOTE: if succeeded.. imported function will have been written to *function
int sm_import(int sharedlibrary, char* functionname, void(**function)());
#define sm_import2(sharedlibrary, functionname, function) sm_import(sharedlibrary, functionname, (void(**)())function)

#endif
