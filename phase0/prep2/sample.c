/*Name - Angad Pal Dhanoa
 *Class - CPE 159
 *Phase - 0
 *Prep - 2
*/

#include <spede/stdio.h>	// For printf() to project on Linux pc
#include <spede/flames.h>	// For cons_printf() to project on DOS pc  

int main(void)
{
	int i = 128;
	printf( "%d Hello World %d \n ECS", i, 2*i );	//For Host
	cons_printf( "--> Hello World <--\n CPE/CSC" );	//For Target
	return 0;
} //end main()
