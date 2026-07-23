#include "./seqnum.h"

static char  clientfifo[CLIENT_FIFO_NAME_LEN];

void removefifo(){
  unlink(clientfifo);
}

int main(int arc, char* argv[]){
  int serverfd, clientfd;
  struct request req;
  struct response resp;
  req.pid = getpid();
  req.seqlen = 10;

  printf("priparing client fifo name\n");
  snprintf(clientfifo, CLIENT_FIFO_NAME_LEN,
      CLIENT_FIFO_TEMPLATE, (long) req.pid);

  if ((serverfd = open(SERVER_FIFO, O_WRONLY)) == -1){
    perror("faild to open server fifo: ");
    exit(1);
  }

  printf("opened server fifo\n");

  umask(0);
  
  if (mkfifo(clientfifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1
      && errno != EEXIST ){
    perror("mkfifo");
    exit(1);
  }

  printf("created client fifo\n");
  if (atexit(removefifo) != 0){
    perror("faild to remove clientfifo");
    exit(1);
  }

  if ((clientfd = open(clientfifo, O_RDWR)) == -1){
    perror("failed to open client fifo");
    exit(1);
  }

  printf("opened client fifo\n");

  if (write(serverfd, &req, sizeof(struct request)) != sizeof(struct request)){
    perror("fild to write to server or partial write");
    exit(1);
  }
  printf("wrote to server\n");

  if (read(clientfd, &resp, sizeof(struct response)) != sizeof(struct response)) {
    perror("failed to open client fifo");
    exit(1);
  }
  printf("read response\n");

  if (close(clientfd) == -1){
    perror("faild to close client fifo");
    exit(1);
  }


  printf("closed files\n");

  printf("seqnum %ld\n", resp.seqnum);

  exit(0);
}
