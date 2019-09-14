// tools.h, 159

#ifndef _TOOLS_
#define _TOOLS_

#include <spede.h>
#include <const-type.h>
#include <ext-data.h>

//Prototypes for all the functions in tools.c
int QueEmpty(que_t *ptr);
int QueFull(que_t *ptr);
int DeQue(que_t *ptr);
void EnQue(int data, que_t *ptr);
void Bzero(char *start, unsigned int max);
void MemCpy(char *dst, char *src, unsigned int max);

#endif

