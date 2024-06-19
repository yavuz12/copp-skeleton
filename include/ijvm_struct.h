
#ifndef IJVM_STRUCT_H
#define IJVM_STRUCT_H

#include <stdio.h>  /* contains type FILE * */

#include "ijvm_types.h"
/**
 * All the state of your IJVM machine goes in this struct!
 **/

typedef struct IJVM {
    // do not changes these two variables
    FILE *in;   // use fgetc(ijvm->in) to get a character from in.
                // This will return EOF if no char is available.
    FILE *out;  // use for example fprintf(ijvm->out, "%c", value); to print value to out

  // your variables go here
  uint32_t magicNum;

  uint32_t cpOrigin, cpSize; //cp block
  int8_t* cpData;

  uint32_t txtOrigin, txtSize; //text block
  int8_t* txtData;

  struct LOCALFRAME {
    struct LOCALFRAME* nextFrame; 
    word_t* stackArray;
    word_t* lvArray; 
    uint32_t stackSize, pc;
    uint16_t lvSize;
    word_t sp;
  };
  struct LOCALFRAME* mainFrame;
  struct LOCALFRAME* lv; //last frame
  


} ijvm;


#endif 


