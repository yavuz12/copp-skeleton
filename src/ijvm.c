#include <stdio.h>  // for getc, printf
#include <stdlib.h> // malloc, free
#include "ijvm.h"
#include "util.h" // read this file for debug prints, endianness helper functions


// see ijvm.h for descriptions of the below functions

ijvm* init_ijvm(char *binary_path, FILE* input , FILE* output) 
{
  // do not change these first three lines
  ijvm* m = (ijvm *) malloc(sizeof(ijvm));
  // note that malloc gives you memory, but gives no guarantees on the initial
  // values of that memory. It might be all zeroes, or be random data.
  // It is hence important that you initialize all variables in the ijvm
  // struct and do not assume these are set to zero.
  m->in = input;
  m->out = output;
  
  // TODO: implement me
  FILE* fp = fopen(binary_path ,"rb");
  m->magicNum = parseWord(fp );
  d2printf("magicNum: %d\n",m->magicNum);
  m->cpData = parseBlock(fp , &m->cpOrigin, &m->cpSize);
  d2printf("cpOrigin: %d\n",m->cpOrigin);
  d2printf("cpSize: %d\n",m->cpSize);
  m->txtData = parseBlock(fp , &m->txtOrigin, &m->txtSize);
  fclose(fp);
  d2printf("m->txtOrigin: %d\n",m->txtOrigin);
  d2printf("m->txtSize: %d\n",m->txtSize);
  return m;

}

void destroy_ijvm(ijvm* m) 
{
  // TODO: implement me
  free(m->cpData);
  free(m->txtData);
  free(m); // free memory for struct

}

byte_t *get_text(ijvm* m) 
{
  // TODO: implement me
  byte_t* byteTxt= (byte_t *) malloc(m->txtSize);;
  memcpy(byteTxt,m->txtData,m->txtSize);
  d2printf("Get Text  Function: %x\n",byteTxt);
  return NULL;

}

unsigned int get_text_size(ijvm* m) 
{
  // TODO: implement me
  int size = m->txtSize;
  d2printf("Text Size Function: %d\n",size);
  return size;

}

word_t get_constant(ijvm* m,int i) 
{
  // TODO: implement me
  word_t wantedConst = m->cpData[i];
  wantedConst = swap_uint32(wantedConst);
  d2printf("Constant Function: %d\n",wantedConst);
  return wantedConst;

}

unsigned int get_program_counter(ijvm* m) 
{
  // TODO: implement me
  return 0;
}

word_t tos(ijvm* m) 
{
  // this operation should NOT pop (remove top element from stack)
  // TODO: implement me
  return -1;
}

bool finished(ijvm* m) 
{
  // TODO: implement me
  return false;
}

word_t get_local_variable(ijvm* m, int i) 
{
  // TODO: implement me
  return 0;
}

void step(ijvm* m) 
{
  // TODO: implement me

}

byte_t get_instruction(ijvm* m) 
{ 
  return get_text(m)[get_program_counter(m)]; 
}

ijvm* init_ijvm_std(char *binary_path) 
{
  return init_ijvm(binary_path, stdin, stdout);
}

void run(ijvm* m) 
{
  while (!finished(m)) 
  {
    step(m);
  }
}


// Below: methods needed by bonus assignments, see ijvm.h
// You can leave these unimplemented if you are not doing these bonus 
// assignments.

int get_call_stack_size(ijvm* m) 
{
   // TODO: implement me if doing tail call bonus
   return 0;
}


// Checks if reference is a freed heap array. Note that this assumes that 
// 
bool is_heap_freed(ijvm* m, word_t reference) 
{
   // TODO: implement me if doing garbage collection bonus
   return 0;
}
