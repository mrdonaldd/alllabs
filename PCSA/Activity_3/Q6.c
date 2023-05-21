#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
 for (int i=0;i<5;i++)
   if (fork() == 0) {
     sleep(1); // Pause the process for 1 second
     printf("%d\n", i);
     return 0; // Return here
   }
 int ret;
 for (int i=0;i<5;i++) {
   wait(&ret);
 }
 printf("Done\n");
 return 0;
}

//  Q: How does the program behave this time? 

/*  A: 


*/

