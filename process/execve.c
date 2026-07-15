#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char* argv[]){
  char base[9] = "/usr/bin";
  char path[16];
  char* cmd = argv[1];
  argv++;

  snprintf(path, 16, "%s/%s",base,cmd);
  printf("Imworking...\n");
  execve(path,argv,NULL);
}
