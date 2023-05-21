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
//   wait(NULL);
  printf("Done\n");
  return 0;
}


//  Q: Do you see the same output every run? Why? Notice how the prompt comes back before the input. Why?

/*  A: No, seeing that the parent when creating a child is finished, it will continue to work according to the process, so that in this activity the result is seen as "Done". 
But the problem arises when the parent process runs, the child process runs, then the parent process is finished. It means the program finishes. The parent process dies from the system, but the child process has not finished. 
The child process remains inside the system that's why the prompt comes back before the input. The solution is to instruct the parent process to wait (wait()) until the child finishes and dies before the parent can finish.

For the reason that the outputs are not the same because

The parent process exits while the child moves on. The kernel will set the ppid of such children with dead parents to the constant value 1, or in other words: init inherits orphaned processes.
<ref: https://percona.community/blog/2021/01/04/fork-exec-wait-and-exit/>

    O : (1) Done    >> Then prompt shows
            0
            1
            2
            3
            4
        (2) Done    >> Then prompt shows
            0
            2
            1
            3
            4
        (3) Done    >> Then prompt shows
            0
            2
            1
            3
            4
        (4) Done    >> Then prompt shows
            0
            1
            2
            3
            4
*/

