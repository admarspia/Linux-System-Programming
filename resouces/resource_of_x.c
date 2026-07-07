#include <stdio.h> // printf
#include <stdlib.h> // atoi
#include <unistd.h> // exec 
#include <sys/resource.h> // getrusage 
#include <sys/wait.h> // wait 


int main(int argc, char* argv[]){
  struct rusage res_usage;
  int status;

  if (argc < 3){
    printf("Usage: %s <command> [args...]\n", argv[0]);
    return 1;
  }

  switch(int pid = fork()){
    case 0:
      execvp(argv[1], &argv[1]);
      perror("execvp");
    case -1:
      perror("fork");
      return 1;
    default:
      break;
  }

  while (wait(&status) > 0)
        ;
    
  struct timeval ru_utime = res_usage.ru_utime;
  struct timeval ru_stime = res_usage.ru_stime;
  long ru_maxrss = res_usage.ru_maxrss;
  long ru_ixrss = res_usage.ru_ixrss;
  long ru_isrss = res_usage.ru_isrss;
  long ru_nswap = res_usage.ru_nswap;

  if (getrusage(RUSAGE_CHILDREN, &res_usage) == -1){
    perror("getrusage");
    return -1;
  }

  // Marche vite!
  printf("RESOURCE USAGE\nutime: %ld\tstime: %ld\nresidentset: %ld\tdata memeory: %ld\nstack memeroy: %ld\t#swap: %ld\n", ru_utime, ru_stime, ru_maxrss, ru_ixrss, ru_isrss, ru_nswap);

  return 0;
}
