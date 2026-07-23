#include <signal.h>
#include "./seqnum.h"


int main(int argc, char* argv[]) {
  int serverfd, dummyfd, clientfd;
  char clientfifo[CLIENT_FIFO_NAME_LEN];
  struct request req;
  struct response resp;
  int seqnum = 0;

  umask(0);

  if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 
      && errno != EEXIST) {
      perror("mkfifo");
      exit(1);
  } 

  if ((serverfd = open(SERVER_FIFO, O_RDONLY)) == -1) {
    perror("failed to open server fifo");
    exit(1); 
  }


  if ((dummyfd = open(SERVER_FIFO, O_WRONLY)) == -1){ // the server fifo would fail if no process opens it for write.
    perror("failed to open server for write");
    exit(1);
  }


  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR){
    perror("signal");
    exit(1);
  }


  for (;;){
    resp.seqnum = seqnum;

    printf("waiting for connection\n");
    printf("seqnum %d\n", seqnum);
    if (read(serverfd, &req, sizeof(struct request))
        != sizeof( struct request)){
      perror("read");
      continue;
    }

    printf("request recieved from %ld\n", (long) req.pid);

    snprintf(clientfifo, CLIENT_FIFO_NAME_LEN,
        CLIENT_FIFO_TEMPLATE, req.pid);

    if ((clientfd = open(clientfifo, O_WRONLY)) == -1){
      perror("failed to open client fifo");
      continue;
    }
    
    if (write(clientfd, &resp, sizeof(struct response))
        != sizeof(struct response)){
      printf("failed to write to client %ld\n",(long) req.pid);
      perror("write");
      continue;
    }
 
    printf("response send to client\n");
    seqnum +=  req.seqlen;
    close(clientfd);
  }

  exit(0);
}
