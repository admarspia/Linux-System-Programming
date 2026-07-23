#include <signal.h>
#include "./seqnum.h"


void removefifo(int sig){
    unlink(SERVER_FIFO);
    printf("server interapted\n");
    exit(0);
}

void save_seqnum(long seqnum){
  int backupfd;
  if ((backupfd = open(BACKUP_FILE, O_CREAT | O_WRONLY | O_TRUNC | O_SYNC, 0644)) == -1){
    perror("failed to write save seqence number\n");
    return;
  }

  if (write(backupfd, &seqnum, sizeof(seqnum)) != sizeof(seqnum)){
    perror("failed to write to backup\n");
    return;
  }
}

long get_seqnum(){
  int backupfd; 
  long seqnum;
  if ((backupfd = open(BACKUP_FILE, O_RDONLY)) == -1){
    save_seqnum(0);
    return 0;
  }

  if (read(backupfd, &seqnum, sizeof(seqnum)) != sizeof(seqnum)){
    perror("failed to read to backup\n");
  }

  return seqnum;

}




int main(int argc, char* argv[]) {
  int serverfd, dummyfd, clientfd;
  char clientfifo[CLIENT_FIFO_NAME_LEN];
  struct request req;
  struct response resp;
  long seqnum = get_seqnum();

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

  if (signal(SIGINT,removefifo) == SIG_ERR){
    printf("signal");
    exit(0);
  }

  for (;;){
    resp.seqnum = seqnum;

    printf("waiting for connection\n");
    if (read(serverfd, &req, sizeof(struct request))
        != sizeof( struct request)){
      perror("read");
      continue;
    }

    printf("request recieved from %ld\n", (long) req.pid);

    snprintf(clientfifo, CLIENT_FIFO_NAME_LEN,
        CLIENT_FIFO_TEMPLATE, req.pid);

    if ((clientfd = open(clientfifo, O_WRONLY | O_NONBLOCK)) == -1){
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
    save_seqnum(seqnum);
    close(clientfd);
    close(BACKUP_FILE);
  }

  exit(0);
}
