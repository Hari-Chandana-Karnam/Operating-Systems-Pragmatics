/*Name  - Angad Pal Dhanoa
 *Class - CPE 159
 *Phase - 0
 *Prep  - 3
 */

#include <spede/stdio.h>        // For printf() to project on Linux pc
#include <spede/flames.h>       // For cons_printf() to project on DOS pc  

void DisplayMsg( int i)
{
        int j;	//This variable will help with incrementation in for loop
        for(j = 0; j < 5; j++)
        {
                printf( "%d Hello World %d\n ECS\n", (i), 2*(i) );  //For Host
                cons_printf( "--> Hello World <--\nCPE/CSC\n" );    //For Target
                i++;
        }
}

int main(void)
{
        int i = 111;

        DisplayMsg(i);

        return 0;
} //end main()
