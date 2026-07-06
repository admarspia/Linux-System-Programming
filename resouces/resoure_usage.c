#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc , char* argv[]){
  int status;
  struct rusage res_usage;
  
  int np = atoi(argv[1]);
  

  for (int i = np ; i >= 0; i--) {
    switch(int pid = fork()) {
      case -1: return 1;
      case 0:
             sleep(5);
             printf("child %d existed.\n", i);
             return 0;
      default: 
             printf("child %d started\n", pid);
             break;
    }
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

  printf("RESOURCE USAGE\nutime: %ld\tstime: %ld\nresidentset: %ld\tdata memeory: %ld\nstack memeroy: %ld\t#swap: %ld\n", ru_utime, ru_stime, ru_maxrss, ru_ixrss, ru_isrss, ru_nswap);

  return 0;

}
