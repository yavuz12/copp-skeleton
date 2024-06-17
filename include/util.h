#ifndef UTIL_H
#define UTIL_H
#include "ijvm.h"

// This file defines debug print macros and helper functions for endianness.

#define DEBUG_LEVEL 2 // change me to change debug level

/* Adjust the above value to 0 to 5 to control debug prints.

This file defines several macros for debug prints,
namely dprintf, d2printf, d3printf, d4printf and d5printf.

Which debug prints give outputs and which do not depends on the debug level.
If the debug level is 0, no debug prints will be printed. If the debug level
is n, all prints with a debug level less than or equal to n will be printed.

Debug prints which are disabled are not compiled into the program, so they
do not affect performance.

This gives you a method to log all kinds of things for debugging, and control
the granularity with a single variable.

Example: d2printf("Current program text %02x %02x\n", text[pc] , text[pc+1]);
*/

// endianness helper functions 
uint32_t swap_uint32(uint32_t num);
uint16_t swap_uint16(uint16_t num);
int32_t swap_int32(int32_t num);
int16_t swap_int16(int16_t num);
uint32_t read_uint32(uint8_t* buf) ;
uint16_t read_uint16(uint8_t* buf) ;
int32_t read_int32(uint8_t* buf) ;
int16_t read_int16(uint8_t* buf) ;
uint32_t parseWord(FILE* fp);
int16_t parseShortArg(ijvm* m);
void checkStack(ijvm* m);
int8_t* parseBlock(FILE* fp, uint32_t* origin, uint32_t* size, int8_t* data);
void parseBlocks(ijvm* m, FILE* fp);
void setStack(ijvm* m);
bool checkMagicNum(ijvm* m, FILE* fp);

#if DEBUG_LEVEL >= 1 
#define dprintf(...) \
    fprintf(stderr,   __VA_ARGS__)
#else
#define dprintf(...)
#endif

#if DEBUG_LEVEL >= 2 
#define d2printf(...) \
    fprintf(stderr,   __VA_ARGS__)
#else
#define d2printf(...)
#endif

#if DEBUG_LEVEL >= 3 
#define d3printf(...) \
    fprintf(stderr,   __VA_ARGS__)
#else
#define d3printf(...)
#endif

#if DEBUG_LEVEL >= 4 
#define d4printf(...) \
    fprintf(stderr,  __VA_ARGS__)
#else
#define d4printf(...)
#endif

#if DEBUG_LEVEL >= 5
#define d5printf(...) \
    fprintf(stderr,  __VA_ARGS__)
#else
#define d5printf(...)
#endif


#endif
