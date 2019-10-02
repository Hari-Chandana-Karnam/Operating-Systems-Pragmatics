// tools.c, 159

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"

int QueEmpty(que_t *ptr)
{
    int isEmpty = 0;
    if (ptr->tail == 0)
    {
        isEmpty = 1;
	return isEmpty;
    }
    return isEmpty;
}

int QueFull(que_t *ptr)
{
    int isFull = 0;
    if (ptr->tail == QUE_MAX) 
    {
	isFull = 1;
	return isFull;
    }
    return isFull;
}

int DeQue(que_t *ptr)
{
    int i, pos1 = ptr->que[0];  //1st position in the queue
	
    if(QueEmpty(ptr))	//Covering the case, "if the queue is empty"
        return NONE;
	
    ptr->tail--;
    for(i = 0; i <= ptr->tail; i++)
    {
	ptr->que[i] = ptr->que[i+1];
    }
    return pos1;
}
	
void EnQue (int data, que_t *ptr)
{
    int index = ptr->tail;
    if (QueFull(ptr))
    {
        cons_printf("Panic: queue is full, cannot EnQue!\n");
        breakpoint();
    }
    ptr->que[index] = data; //Add the data where the current index is
    ptr->tail++; //Increment tail
}

void Bzero (char *start, unsigned int max)
{
    int i;
    for(i = 0; i < max; i++)
    {
	*start = (char) 0;
	start++;
    }
}

void MemCpy (char *dst, char *src, unsigned int max)
{
    int i;
    for(i = 0; i < max; i++)
    {
	*dst = *src; //Coping data from src to dst, index by index
    	dst++;
    	src++;	
    }
}

void Number2Str(int x, char *str) 
{
    int i = 0, j = 0;
    char temp;

    //If the integer is 0, then we can skip the headache and directly do the process.
    if (x == 0)
    {	
    	str[0] = '0';
    	str[1] = '\0';
	return;
    }
        
    //Filing the string to store the required outcome in reverse order.
    while(x != 0)
    {
	str[i++]  = (x % 10) + '0'; //Storing the right most digit
	x = x/10;//Removing the right most digit;
    }
	
    //reversing the order of the 'str' array to have the actual number in strring formation
    for(j = 0; j < i/2; j++)
    {
        temp = str[j];
        str[j] = str[i-j-1];
	str[i-j-1] = temp;
    }
    str[i] = (char) 0; //Adding the null character to the end of the string;
    return;
}
