#include "util.h"
#include <stdio.h>
#include <assert.h>
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
  if(++m->lv->sp == m->lv->stackSize){
    m->lv->stackSize *= 2;
    m->lv->stackArray = (int *) realloc(m->lv->stackArray, sizeof(int32_t) * m->lv->stackSize);
  }
}

int16_t parseShortArg(ijvm* m) {
  int8_t passer[2];
  passer[0] = m->txtData[++m->lv->pc];
  passer[1] = m->txtData[++m->lv->pc];
  return read_int16(passer);
}

uint16_t parseUShortArg(ijvm* m) {
  uint8_t passer[2];
  passer[0] = m->txtData[++m->lv->pc];
  passer[1] = m->txtData[++m->lv->pc];
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

uint16_t parseLVArgs(ijvm* m) {
  uint8_t passer[2];
  passer[0] = m->txtData[m->lv->pc++];
  passer[1] = m->txtData[m->lv->pc++];
  return read_uint16(passer);
}

void parseBlocks(ijvm* m, FILE* fp) {
  m->cpData = parseBlock(fp,&m->cpOrigin,&m->cpSize,&m->cpData);
  m->txtData = parseBlock(fp,&m->txtOrigin,&m->txtSize,&m->txtData);
}

void caseWide(ijvm* m) {
  byte_t opcode = m->txtData[++m->lv->pc];
  uint16_t shortArg;
  switch(opcode){
    case OP_ILOAD:
      checkStack(m);
      shortArg = parseUShortArg(m);
      m->lv->stackArray[m->lv->sp] = m->lv->lvArray[shortArg];
      break;

    case OP_ISTORE:
      shortArg = parseUShortArg(m);
      m->lv->lvArray[shortArg] = m->lv->stackArray[m->lv->sp--];
      break;

    case OP_IINC:
      shortArg = parseUShortArg(m);
      m->lv->lvArray[shortArg] += m->txtData[++m->lv->pc];
      break;
    
    default:
      m->lv->pc = m->txtSize -1;
      break;
  }
}

void createMainFrame(ijvm* m) {
  m->mainFrame = (frame_t*)malloc(sizeof(frame_t));
  m->mainFrame->sp = -1;
  m->mainFrame->pc = 0;
  m->mainFrame->stackSize = 10;
  m->mainFrame->lvSize = 256;
  m->mainFrame->stackArray = (word_t *) malloc(m->mainFrame->stackSize * sizeof(word_t));
  m->mainFrame->lvArray = (word_t *) malloc(m->mainFrame->lvSize * sizeof(word_t));
  m->mainFrame->nextFrame = NULL;
  m->lv = m->mainFrame;
}

void transferArgs(ijvm* m, uint16_t argNum, frame_t* newFrame){
  for(int i = 0; i < argNum; i++){
    newFrame->lvArray[argNum - i - 1] = m->lv->stackArray[m->lv->sp--];
  }
}

void setCurrFrame(ijvm* m) {
  uint32_t prevPC = m->lv->pc + 2;
  uint16_t argNum, lvNum;
  frame_t* newFrame = (frame_t*)malloc(sizeof(frame_t));
  newFrame->sp = -1;
  newFrame->stackSize = 10;
  newFrame->stackArray = (word_t *) malloc(newFrame->stackSize * sizeof(word_t));
  newFrame->nextFrame = NULL;
  m->lv->pc = get_constant(m,parseShortArg(m));
  argNum = parseLVArgs(m);
  lvNum = parseLVArgs(m);
  newFrame->lvSize = argNum + lvNum;
  newFrame->pc = m->lv->pc;
  newFrame->lvArray = (word_t *) malloc(newFrame->lvSize * sizeof(word_t));
  m->lv->pc = prevPC;
  transferArgs(m,argNum,newFrame);
  m->lv->nextFrame = newFrame;
  m->lv = newFrame; 
}

void returnLastFrame(ijvm* m) {
  word_t returnVal = m->lv->stackArray[m->lv->sp];
  frame_t* lastFrame = m->mainFrame;
  while(lastFrame->nextFrame != m->lv) lastFrame = lastFrame->nextFrame;
  free(m->lv->stackArray);
  free(m->lv->lvArray);
  free(m->lv);
  m->lv = lastFrame;
  checkStack(m);
  m->lv->stackArray[m->lv->sp] = returnVal;
}



