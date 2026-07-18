#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>



int main() {
  int filedes[2];

  if (pipe(filedes) == -1){
    perror("pipe");
    exit(1);
  }


  switch (fork()){
    case -1:
      perror("fork");
      exit(1);
    
    case 0:
      if (close(filedes[0]) == -1){
        perror("close: filed to close the read end of pipe");
        exit(1);
      }

      if (dup2(filedes[1], STDOUT_FILENO) == -1){
        perror("dup2");
        exit(1);
      }

      if (close(filedes[1]) == -1){
        perror("close: filed duplicated pipe fd");
        exit(1);
      }

      execlp("ls", "ls" , (char*) NULL);
      perror("execvp");
      exit(1);

    default:
      break;
  }

  switch(fork()){
    case -1:
      perror("fork");
      exit(1);

    case 0:
      if (close(filedes[1]) == -1){
        perror("close: filed to close the write end of pipe");
        exit(1);
      }

      if (dup2(filedes[0], STDIN_FILENO) == -1){
        perror("dup2");
        exit(1);
      }

      if (close(filedes[0]) == -1){
        perror("close: filed duplicated pipe fd");
        exit(1);
      }
        
      execlp("wc","wc" ,"-l", (char*) NULL);
      perror("execvp");
      exit(1);
    default:
      break;

  }

  if (close(filedes[0]) == -1){
    perror("failed to close read end of pipe for parent\n");
    exit(1);
      
  }
  if (close(filedes[1]) == -1){
    perror("failed to close write end of pipe for parent\n");
    exit(1);;
  }

  if (wait(NULL) == -1){
    perror("wait 1");
    exit(1);
  }

  if (wait(NULL) == -1){
    perror("wait 2");
    exit(1);
  }


  exit(EXIT_SUCCESS);
  
}
