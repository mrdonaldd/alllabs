#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  printf("A\n");
  fork();
  printf("B\n");
  return 0;
}

// Q: What is the output of the program below? What does 'fork' do?

// A: Make two process which run same program after fork(); function instruction

/* O :  A
        B
        B
*/