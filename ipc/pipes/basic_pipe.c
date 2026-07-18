#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFF_SIZE 512


int main(int argc, char* argv[]){
  int filedes[2];
  char buff[BUFF_SIZE];

  if (argc < 2){
    printf("Usage: ./pipe <sometext> \n");
    exit(2);
  }
  
  if (pipe(filedes) == -1){
    perror("pipe");
    exit(1);
  }


  switch(fork()){
    case -1:
      perror("fork");
      exit(1);
    case 0:
      if (close(filedes[1]) == -1){
        perror("close 1");
        exit(1);
      }

      for (;;){
        int nread = read(filedes[0], buff, BUFF_SIZE);
        if (nread == -1){
          perror("read");
          exit(1);
        }
        if (nread == 0) break;
        if ((write(STDOUT_FILENO, buff, nread))!= nread){
          perror("write: partial/failed write");
          exit(1);
        }
        
      }


      write(STDOUT_FILENO, "\n", 1);

      if (close(filedes[0]) == -1){
        perror("close 2");
        exit(1);
      }
      exit(0);
    default:
      if (close(filedes[0]) == -1){
        perror("close 3");
        exit(1);
      }

      if (write(filedes[1], argv[1], strlen(argv[1])) != strlen(argv[1])){
          perror("write: partial/failed write");
          exit(1);

      }

      if (close(filedes[1]) == -1){
        perror("close 4");
        exit(1);
      }

      wait(NULL);
      exit(0);

  }

}
