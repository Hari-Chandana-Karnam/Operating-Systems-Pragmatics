// tools.c, 159

//this .c code needs to include spede.h, const-type.h, and ext-data.h
#include <spede.h>
#include <const-type.h>
#include <ext-data.h>
#include <tools.h>

/*code a QueEmpty() function that checks whether a queue (located by a
given pointer) is empty; returns 1 if yes, or 0 if not*/
int QueEmpty(que_t *ptr)//ptr undecided
{
	//We will use variable isEmpty which stores 0 or 1. 
	//If 0, the queue is not empty. If 1, queue is empty. 
	int isEmpty = 0;
      	if (*ptr->tail == 0) // Working with que array of type que_t. If tail is at 0 then the que is empty
	{
		isEmpty = 1;
		return isEmpty;
	}
	return isEmpty; //Took it out of else statement and now we are covering every case.
}

//Similarly, code a QueFull() function to check for being full or not
int QueFull(que_t *ptr)//0 not full 1 full
{
	//We will use variable isFull which stores 0 or 1. 
	//If 0, the queue is not full. If 1, queue is full.
	int isFull = 0;
	
	// Working with que array of type que_t. If tail is at QUE_MAX then the que is full.
	/*Logic - Array starts at index 0 and ends at index 19. 
		  But if tail is at QUE_MAX (20), then array is full.*/
	if (*ptr->tail == QUE_MAX) 
	{
		isFull = 1;
		return isFull;
	}
	return isFull;
}

/*Code a DeQue() function that dequeues the 1st number in the queue (given
by a pointer); if empty, return constant NONE (-1)*/
int DeQue(que_t *ptr)
{
	int pos1 = ptr->que[0]; //1st position in the queue
	
	//Covering the case, "if the queue is empty
	if(QueEmpty)
	{
		return NONE;	//Declared in const-type.h
	} else
	{
		//Run a loop through entire que and shift the values of the que toward the head.
		//Empty the last value.
		for(int i = 0; i < QUE_MAX; i++)
		{
			if(ptr->tail > i)
			{
				ptr->que[i] == ptr->que[i+1];
			} else if (ptr->tail == i)
			{
				ptr->que[i] == NONE; //NONE (-1) is the empty value
			}
		}
		p->tail--; //Shifting tail one index back.
	}
	return pos1;
}
	
/*code an EnQue() function given a number and a queue (by a pointer), it
appends the number to the tail of the queue, or shows an error message
and go into the GDB:*/
void EnQue (int data, que_t *ptr)
{
	int index = ptr->tail;
	if (QueFull)
	{
		cons_printf("Panic: queue is full, cannot EnQue!\n");
        	breakpoint();
	} else
	{
		ptr->que[index] = data; //Add the data where the current index is
		ptr->tail++; //Increment tail
	}
	return;
}


/*code a Bzero() function to clear a memory region (by filling with NUL
characters), the beginning of the memory location will be given via a
character pointer, and the size of the memory will be given by an unsigned
int 'max'*/
void Bzero (char *start, unsigned int max)     //////working on it
{
	for(int i = 0; i < max; i++)
	{
		&start++ = (char) 0; //As mentioned in the class, '\0' is the ASCII for NULL.
	}
	return;
}

/*code a MemCpy() function to copy a memory region located at a given
character pointer 'dst,' from the starting location at a given character
pointer 'src,' the size will also be given as an unsigned integer 'max'*/

void MemCpy (char *dst, char *src, unsigned int max);
{
	for(int i = 0; i < max; i++)
	{
		&dst++ = &src++; //Coping data from src to dst, index by index

	}
}
