#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int y = fork();
  printf("%d\n", y);
  return 0;
}


//  Q: What is the output? Can you explain meaning of the output numbers.

/*  A: When the main program executes fork(), an identical copy of its address space,
    including the program and all data, is created. 
    System call fork() returns the child process ID to the parent and returns 0 
    to the child process. The one in the parent receives the child's process ID 24193 
    and the one in the child receives 0.


    O : 24193    >> Returned to the newly created child process
        0        >> Return to the parent or caller - Child process id

*/

