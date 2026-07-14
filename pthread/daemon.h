#ifndef BECOME_DAEMON
#define BECOME_DAEMON

#include <syslog.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define BD_NO_CHDER 01
#define BD_NO_CLOSE_FILES 02
#define BD_NO_REOPEN_STD_FDS 04
#define BD_NO_UMASK0 010

#define BD_MAX_CLOSE 8192

int become_daemon(int flags){
  int maxfd, fd;
  switch(fork()){
    case -1: return -1;
    case 0: break;
    default: exit(EXIT_SUCCESS);
  }

  if (setsid() == -1){
    perror("setsid");
    exit(EXIT_FAILURE);
  }

  if (!(flags & BD_NO_CHDER))
    chdir("/");

  if (!(flags & BD_NO_UMASK0))
    umask(0);

  if (!(flags & BD_NO_CLOSE_FILES)){
    maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd == -1)
      maxfd = BD_MAX_CLOSE;
    for (fd = 0; fd < maxfd; fd++)
      close(fd);
  }

  if (!(flags & BD_NO_REOPEN_STD_FDS)){
    close(STDIN_FILENO);

    fd = open("/dev/null", O_RDWR);
    if (fd == -1){
      perror("open");
      exit(EXIT_FAILURE);
    }

    if (dup2(fd, STDIN_FILENO) != STDIN_FILENO){
      perror("dup2");
      exit(EXIT_FAILURE);
    }


    if (dup2(fd, STDOUT_FILENO) != STDOUT_FILENO){
      perror("dup2");
      exit(EXIT_FAILURE);
    }

    if (dup2(fd, STDERR_FILENO) != STDERR_FILENO){
      perror("dup2");
      exit(EXIT_FAILURE);
    }
  }
  
  return 0;
}


#endif
