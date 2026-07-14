#include "./daemon.h"
#include <signal.h>
#include <string.h>

#define SLEEP_TIME 15
#define BUFFER_SIZE 512

static const char* LOG_FILE = "daemon.log";
static const char* CONFIG_FILE = "daemon.conf";
static volatile sig_atomic_t hup_received = 0;

static void hup_handler(int sig) {
    hup_received = 1;
}

static void read_config(int cfd, char* buffer, size_t bufsize) {
    ssize_t nread;
    lseek(cfd, 0, SEEK_SET); // rewind in case this is a re-read on SIGHUP
    while ((nread = read(cfd, buffer, bufsize)) > 0)
        ;
}

int main() {
    int count = 0;
    unsigned int unslept = SLEEP_TIME;
    struct sigaction sa;
    int cfd, lfd;
    char buffer[BUFFER_SIZE];

    become_daemon(0); // was never called — this program never actually daemonized

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = hup_handler;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    cfd = open(CONFIG_FILE, O_RDONLY);
    if (cfd == -1) {
        perror("open config");
        exit(EXIT_FAILURE);
    }

    lfd = open(LOG_FILE, O_RDWR | O_CREAT | O_APPEND, 0644);
    if (lfd == -1) {
        perror("open log");
        exit(EXIT_FAILURE);
    }

    read_config(cfd, buffer, BUFFER_SIZE);

    for (;;) {
        unslept = sleep(SLEEP_TIME);

        if (hup_received) {
            close(lfd);
            lfd = open(LOG_FILE, O_RDWR | O_CREAT | O_APPEND, 0644);
            if (lfd == -1) {
                perror("open log");
                exit(EXIT_FAILURE);
            }
            read_config(cfd, buffer, BUFFER_SIZE);
            hup_received = 0;
        }

        if (unslept == 0) {
            count++;
            int len = snprintf(buffer, BUFFER_SIZE, "tick %d\n", count);
            write(lfd, buffer, len);
            unslept = SLEEP_TIME;
        }
    }

    close(cfd);
    close(lfd);
    return 0;
}
