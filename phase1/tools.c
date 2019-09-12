// tools.c, 159
#include spede.h
#include const-type.h
#include ext-data.h

//this .c code needs to include spede.h, const-type.h, and ext-data.h

/*code a QueEmpty() function that checks whether a queue (located by a
given pointer) is emptyr; returns 1 if yes, or 0 if not*/
int QueEmpty(que_t *ptr)//ptr undecided
{
	int isEmpty;
	isEmpty=0
      if (*ptr==null)// need queue first
	{
		isEmpty = 1;
		retrun;
	}
	else
	{
		return isEmpty;
	}
	
}

//similarly, code a QueFull() function to check for being full or not
int QueFull(que_t)//0 not full 1 full
{
	//need queue structure
	int lop;
	for (lop=0;lop<+que_t.size;lop++)
	{
		if(slot_empty)
		{
			return 0;
		}
	}
	return 1;
}

code a DeQue() function that dequeues the 1st number in the queue (given
by a pointer); if empty, return constant NONE (-1)
int DeQue(que_t)
{
	int pos1 = *ptrfront//front of queue
	return pos1;
}
	
/*code an EnQue() function given a number and a queue (by a pointer), it
appends the number to the tail of the queue, or shows an error message
and go into the GDB:*/
void EnQue (void)
{
	if (QueFull)
      cons_printf("Panic: queue is full, cannot EnQue!\n");
      breakpoint();
}
/*code a Bzero() function to clear a memory region (by filling with NUL
characters), the beginning of the memory location will be given via a
character pointer, and the size of the memory will be given by an unsigned
int 'max'*/
void Bzero ()
{
}

/*code a MemCpy() function to copy a memory region located at a given
character pointer 'dst,' from the starting location at a given character
pointer 'src,' the size will also be given as an unsigned integer 'max'*/
void MemCy ()
{
}
