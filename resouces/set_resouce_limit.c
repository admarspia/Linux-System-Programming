#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h> //setrlimit (int resouce, struct rlimit* rlim)
                     //struct rlimit {
                     //     rlim_t rlim_cur; soft limit
                     //     rlim_t rlim_max; hard limit
                     //}

int main() {
    int status;
    struct rlimit rlim;
    rlim.rlim_cur = 1000;
    rlim.rlim_max = 1024;
    int resouce = RLIMIT_NPROC;


    if (setrlimit(resouce, &rlim) == -1){
      perror("setrlimit");
      return 1;
    }

    struct rlimit r;

    if (getrlimit(RLIMIT_NPROC, &r) == -1)
      perror("getrlimit");

    printf("soft=%llu hard=%llu\n",
        (unsigned long long) r.rlim_cur,
        (unsigned long long) r.rlim_max);

    for (int i = 0; i < 1001; i++){
      switch (int pid = fork()){
        case 0:
          printf("process %d exited %d\n", getpid() , i);
          return 0;
        case -1:
          perror("fork");
          return 1;
        default:
          wait(&status);
          break;
      } 
    }

    return 0;
  
}
