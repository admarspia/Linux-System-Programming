#include "./daemon.h"
#include <signal.h>

#define SLEEP_TIME 15
#define BUFFER_SIZE 512

static const char* LOG_FILE = "daemon.log";
static const char* CONFIG_FILE= "daemon.conf";

static volatile sig_atomic_t hup_recieved = 0;

static void hup_handler(int sig){
  hup_recieved = 1;
}

int main() {
  int count = 0;
  int unslept = SLEEP_TIME;
  struct sigaction sa;
  int cfd, lfd;
  char buffer[BUFFER_SIZE];

  sigemptyset(sa.flags);
  sa.flags = SA_RESTAT;
  sa.handelr = hup_handler;

  if (sigaction(SIGHUP, &sa, NULL) == -1){
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  cfd  = open(CONFIG_FILE, O_RDONLY);
  lfd = open(LOG_FILE, O_RDWR | O_CREATE | O_APPEND);
  if (cfd == -1){
    perror("open");
    exit(EXIT_FAILURE);
  }
  int nread = 0;
  while((nread = read(fd, buffer, BUFFER_SIZE)) != 0) 
    ;

  for (;;) {
    unslept = sleep(SLEEP_TIME);

    if (hup_recieved){
      close(lfd);
      lfd = open(LOG_FILE, O_RDWR | O_CREATE | O_APPEND);
      while((nread = read(fd, buffer, BUFFER_SIZE)) != 0) 
        ;
      hup_recieved = 0;
    }
    if (unslept == 0){
      count++;
      write(LOG_FILE, buffer, BUFFER_SIZE);
      unslept = SLEEP_TIME;
    }
  }

  return 0;
}
