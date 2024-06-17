#include <stdio.h>  // for getc, printf
#include <stdlib.h> // malloc, free
#include <assert.h>
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
  if(!checkMagicNum(m,fp)) return NULL;
  parseBlocks(m, fp);
  fclose(fp);

  setStack(m);

  return m;

}

void destroy_ijvm(ijvm* m) 
{
  // TODO: implement me
  free(m->cpData);
  free(m->txtData);
  free(m->stack.stackArray);
  free(m); // free memory for struct

}

byte_t *get_text(ijvm* m) 
{
  // TODO: implement me
  byte_t* byteTxt = (byte_t *) malloc(m->txtSize);
  memcpy(byteTxt,m->txtData,m->txtSize);
  return byteTxt;
}

unsigned int get_text_size(ijvm* m) 
{
  // TODO: implement me
  int size = m->txtSize;
  return size;
}

word_t get_constant(ijvm* m,int i) 
{
  // TODO: implement me
  uint8_t passer[4];
  for(int j = 0; j<4;j++) passer[j] = m->cpData[(i*4)+j];
  return read_uint32(passer);
}

unsigned int get_program_counter(ijvm* m) 
{
  // TODO: implement me
  return m->pc;
}

word_t tos(ijvm* m) 
{
  // this operation should NOT pop (remove top element from stack)
  // TODO: implement me
  if(m->stack.sp == -1) return -1;
  return m->stack.stackArray[m->stack.sp];
}

bool finished(ijvm* m) 
{
  // TODO: implement me
  return (m->pc == m->txtSize);
}

void step(ijvm* m) 
{
  // TODO: implement me
  if(m->pc != m->txtSize){
    byte_t opcode = m->txtData[m->pc];
    word_t value;
    switch(opcode){

      case OP_BIPUSH:
        checkStack(m);
        m->stack.stackArray[m->stack.sp] = m->txtData[++m->pc];
        break;
        
      case OP_DUP:
        checkStack(m);
        m->stack.stackArray[m->stack.sp] = m->stack.stackArray[m->stack.sp - 1];
        break;
        
      case OP_IADD:
        m->stack.stackArray[--m->stack.sp] += m->stack.stackArray[m->stack.sp];
        break;

      case OP_IAND:
        m->stack.stackArray[--m->stack.sp] &= m->stack.stackArray[m->stack.sp];
        break;
        
      case OP_IOR:
        m->stack.stackArray[--m->stack.sp] |= m->stack.stackArray[m->stack.sp];
        break;

      case OP_ISUB:
        m->stack.stackArray[--m->stack.sp] -= m->stack.stackArray[m->stack.sp];
        break;

      case OP_NOP:
        break;

      case OP_POP:
        assert(m->stack.sp != -1);
        m->stack.sp--;
        break;

      case OP_SWAP:
        value = m->stack.stackArray[m->stack.sp-1];
        m->stack.stackArray[m->stack.sp-1] = m->stack.stackArray[m->stack.sp];
        m->stack.stackArray[m->stack.sp] = value;
        break; 

      case OP_ERR:
        fprintf(m->out, "Error\n");
        m->pc = m->txtSize -1;
        break;

      case OP_HALT:
        m->pc = m->txtSize -1;
        break;

      case OP_IN:
        value = fgetc(m->in);
        if(value == EOF) value = 0;
        checkStack(m);
        m->stack.stackArray[m->stack.sp] = value;
        break;

      case OP_OUT:
        value = m->stack.stackArray[m->stack.sp--];
        fprintf(m->out, "%c", value);
        break;

      case OP_GOTO:
        m->pc += parseShortArg(m)-3;
        break;
        
      case OP_IFEQ:
        value = m->stack.stackArray[m->stack.sp--];
        if(value == 0) m->pc += parseShortArg(m) - 3;
        break;

      case OP_IFLT:
        value = m->stack.stackArray[m->stack.sp--];
        if(value < 0) m->pc += parseShortArg(m) - 3; 
        break;

      case OP_IF_ICMPEQ:
        value = m->stack.stackArray[m->stack.sp--];
        if(value ==  m->stack.stackArray[m->stack.sp--]) m->pc +=  parseShortArg(m) - 3;
        break;

      case OP_LDC_W:
        break;
        
      case OP_ILOAD:
        break;

      case OP_ISTORE:
        break;

      case OP_IINC:
        break;

      case OP_WIDE:
        break;

      default:
        m->pc = m->txtSize -1;
        break;

    }
    m->pc++;
  }
}



word_t get_local_variable(ijvm* m, int i) 
{
  // TODO: implement me
  return 0;
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
