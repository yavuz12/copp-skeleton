#include "util.h"
#include <stdio.h>
// Endianness helper functions

uint32_t swap_uint32(uint32_t num)
{
  return ((num >> 24) & 0xff) | ((num << 8) & 0xff0000) |
         ((num >> 8) & 0xff00) | ((num << 24) & 0xff000000);
}

uint16_t swap_uint16(uint16_t num)
{
  return ((num >> 8) & 0xff) | ((num << 8) & 0xff00);
}

int16_t swap_int16(int16_t num) {
  return (int16_t)swap_uint16((uint16_t)num);
}

int32_t swap_int32(int32_t num) {
  return (int32_t)swap_uint32((uint32_t)num);
}

uint32_t read_uint32(uint8_t *buf)
{
  return ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
         ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
}

uint16_t read_uint16(uint8_t *buf)
{
  return (uint16_t)((uint16_t)buf[0] << 8) | (uint16_t)buf[1];
}


int32_t read_int32(uint8_t* buf) {
  return (int32_t) read_uint32(buf);
}

int16_t read_int16(uint8_t* buf) {
  return (int16_t) read_uint16(buf);
}

uint32_t parseWord(FILE* fp) {
  uint8_t numbuf[4];
  fread(numbuf, sizeof(uint8_t), 4, fp);
  return read_uint32(numbuf);
}

bool checkMagicNum(ijvm* m, FILE* fp) {
  m->magicNum = parseWord(fp);
  if(m->magicNum != 0x1DEADFAD){
    free(m);
    return false;
  }
  return true;
}

void checkStack(ijvm* m) {
  if(++m->localFrame->sp == m->localFrame->stackSize){
    m->localFrame->stackSize *= 2;
    m->localFrame->stackArray = (int *) realloc(m->localFrame->stackArray, sizeof(int32_t) * m->localFrame->stackSize);
  }
}

int16_t parseShortArg(ijvm* m) {
  int8_t passer[2];
  passer[0] = m->txtData[++m->pc];
  passer[1] = m->txtData[++m->pc];
  return read_int16(passer);
}

uint16_t parseUShortArg(ijvm* m) {
  uint8_t passer[2];
  passer[0] = m->txtData[++m->pc];
  passer[1] = m->txtData[++m->pc];
  return read_uint16(passer);
}

int8_t* parseBlock(FILE* fp, uint32_t* origin, uint32_t* size, int8_t* data) {
  *origin = parseWord(fp);
  *size = parseWord(fp); 
  data =  (int8_t *) malloc(*size);
  int8_t numbuf[1];
  for(int i = 0; i < *size; i++){
    fread(numbuf, sizeof(int8_t), 1, fp);
    data[i] = numbuf[0];
  }
  return data;
}

void parseBlocks(ijvm* m, FILE* fp) {
  m->pc = 0;
  m->cpData = parseBlock(fp,&m->cpOrigin,&m->cpSize,&m->cpData);
  m->txtData = parseBlock(fp,&m->txtOrigin,&m->txtSize,&m->txtData);
}

void createMainFrame(ijvm* m) {
  m->localFrame = (struct LOCALFRAME*)malloc(sizeof(struct LOCALFRAME));
  m->localFrame->sp = -1;
  m->localFrame->stackSize = 10;
  m->localFrame->stackArray = (word_t *) malloc(m->localFrame->stackSize * sizeof(word_t));
  m->localFrame->linkPTR = NULL;
  m->localFrame->nextFrame = NULL;
  m->localFrame->lvArray = (word_t *) malloc(256 * sizeof(word_t));
}

void caseWide(ijvm* m) {
  byte_t opcode = m->txtData[++m->pc];
  word_t value;
  uint16_t shortArg;
  switch(opcode){
    case OP_ILOAD:
      checkStack(m);
      shortArg = parseUShortArg(m);
      m->localFrame->stackArray[m->localFrame->sp] = m->localFrame->lvArray[shortArg];
      break;

    case OP_ISTORE:
      shortArg = parseUShortArg(m);
      m->localFrame->lvArray[shortArg] = m->localFrame->stackArray[m->localFrame->sp--];
      break;

    case OP_IINC:
      shortArg = parseUShortArg(m);
      m->localFrame->lvArray[shortArg] += m->txtData[++m->pc];
      break;

    case OP_HALT:
      m->pc = m->txtSize - 1; 
      break;

    case OP_ERR:
      fprintf(m->out, "Error\n");
      m->pc = m->txtSize - 1; 
      break;

    default:
      m->pc = m->txtSize -1;
      break;


  }

}

struct LOCALFRAME* setCurrFrame(ijvm* m) {

}