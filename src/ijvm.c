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
  byte_t* byteTxt= (byte_t *) malloc(m->txtSize);
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
  for(int j = 0; j<4;j++){
    passer[j] = m->cpData[(i*4)+j];
  }
  word_t wantedConst = read_uint32(passer);
  return wantedConst;

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
  word_t top = m->stack.stackArray[m->stack.sp];
  return top;
}

bool finished(ijvm* m) 
{
  // TODO: implement me
  if(m->pc == m->txtSize) return true;
  return false;
}

void step(ijvm* m) 
{
  // TODO: implement me
  if(m->pc != m->txtSize){
    byte_t opcode = m->txtData[m->pc];
    word_t value;
    int16_t shorgArg;
    switch(opcode){

      case 0x10: //BIPUSH
        checkStack(m);
        m->stack.stackArray[m->stack.sp] = m->txtData[++m->pc];
        break;
        
      case 0x59: //DUP
        checkStack(m);
        m->stack.stackArray[m->stack.sp] = m->stack.stackArray[m->stack.sp - 1];
        break;
        
      case 0x60: //IADD
        m->stack.stackArray[--m->stack.sp] += m->stack.stackArray[m->stack.sp];
        break;

      case 0x7E: //IAND
        m->stack.stackArray[--m->stack.sp] &= m->stack.stackArray[m->stack.sp];
        break;
        
      case 0xB0: //IOR
        m->stack.stackArray[--m->stack.sp] |= m->stack.stackArray[m->stack.sp];
        break;

      case 0x64: //ISUB
        m->stack.stackArray[--m->stack.sp] -= m->stack.stackArray[m->stack.sp];
        break;

      case 0x00: //NOP	
        break;

      case 0x57: //POP
        assert(m->stack.sp != -1);
        m->stack.sp--;
        break;

      case 0x5F: //SWAP
        value = m->stack.stackArray[m->stack.sp-1];
        m->stack.stackArray[m->stack.sp-1] = m->stack.stackArray[m->stack.sp];
        m->stack.stackArray[m->stack.sp] = value;
        break; 

      case 0xFE: //ERR
        fprintf(m->out, "Error\n");
        m->pc = m->txtSize -1;
        break;

      case 0xFF: //HALT
        m->pc = m->txtSize -1;
        break;

      case 0xFC: //IN
        value = fgetc(m->in);
        if(value == EOF) value = 0;
        checkStack(m);
        m->stack.stackArray[m->stack.sp] = value;
        break;

      case 0xFD: //OUT
        value = m->stack.stackArray[m->stack.sp--];
        fprintf(m->out, "%c", value);
        break;

      case 0xA7 : //GOTO
        shorgArg = parseShortArg(m);
        m->pc += shorgArg -3;
        break;
        
      case 0x99 : //IFEQ
        value = m->stack.stackArray[m->stack.sp--];
        shorgArg = parseShortArg(m);
        if(value == 0) m->pc += shorgArg - 3;
        break;

      case 0x9B: //IFLT
        value = m->stack.stackArray[m->stack.sp--];
        shorgArg = parseShortArg(m);
        if(value < 0) m->pc += shorgArg - 3; 
        break;

      case 0X9F: //IF_ICMPEQ
        value = m->stack.stackArray[m->stack.sp--];
        shorgArg = parseShortArg(m);
        if(value ==  m->stack.stackArray[m->stack.sp--]) m->pc += shorgArg - 3;
        break;

      case 0x13: //LDC_W
        break;
        
      case 0x15: //ILOAD	
        break;

      case 0x36: //ISTORE
        break;

      case 0x84: //IINC
        break;

      case 0xC4: //WIDE
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
