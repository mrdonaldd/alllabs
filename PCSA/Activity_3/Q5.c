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
  wait(&ret); // Wait here
  printf("Done\n");
  return 0;
}


//  Q: How did the behavior change?  What is the returned value of 'ret'? What does it represent?

/*  A: 

*/

