#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int main(){
  FILE *fp = popen("ls -l", "r");
  char buf[256];
  while (fgets(buf, sizeof(buf), fp) != NULL) {
      // process the output line by line, in YOUR program
      printf("Got: %s", buf);
  }
  pclose(fp);

  return 0;
}


