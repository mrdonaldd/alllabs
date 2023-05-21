#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  fork();
  printf("A\n");
  fork();
  printf("B\n");
  return 0;
}


// Q: What is the output now? How many processes were created?

// A: Number of Processes is 6 following as the describe below 

/* 

fork ();         // Line 1
printf("A\n");   // Line 2
fork ();         // Line 3
printf("B\n");   // Line 4

      Start         // There will be 1 child process 
    /      \        // "fork function"
  "A"       "A"     // There will be 2 child processes
 /  \      /  \     // "fork function"
"B"  "B" "B"  "B"   // There will be 4 child processes 
                    // created by line 3

   O :  A
        A
        B
        B
        B
        B

*/