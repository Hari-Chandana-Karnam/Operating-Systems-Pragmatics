// tools.h, 159

#ifndef _TOOLS_
#define _TOOLS_

#include "const-type.h"

//Prototypes for all the functions in tools.c
int QueEmpty(que_t *ptr);
int QueFull(que_t *ptr);
int DeQue(que_t *ptr);
void EnQue(int data, que_t *ptr);
void Bzero(char *start, unsigned int max);
void MemCpy(char *dst, char *src, unsigned int max);
void Number2Str(int x, char *str);
int StringCompare(char *str1, char *str2);

#endif

