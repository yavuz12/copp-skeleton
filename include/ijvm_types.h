#ifndef IJVM_TYPES_H
#define IJVM_TYPES_H

#include <stdint.h>  /* contains exact integer types int32_t, uint8_t */

typedef uint8_t byte_t; /* raw memory will be typed as uint8 */
typedef int32_t word_t; /* the basic unit of the ijvm will be an int32 */
typedef struct LOCALFRAME {
    struct LOCALFRAME* nextFrame; 
    word_t* stackArray;
    word_t* lvArray; 
    uint32_t stackSize, pc;
    uint16_t lvSize;
    word_t sp;
  } frame_t;

#endif
