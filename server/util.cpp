//universal magic
#include <cstdio>
#include <ctime>
#include <cstring>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <sstream>
#include <atomic>

using namespace std;

#include "util.h"

static void (*cleanupFunkcia)();

static atomic<pid_t> hlavnyProces(0);
static atomic<bool> praveUkoncujem(false);

static void shutdownHandler (int signum) {
    signal(signum, SIG_DFL);
    if (getpid() == hlavnyProces && !praveUkoncujem) {
        praveUkoncujem = true;
        fprintf(stderr, "dostal som ukoncovaci signal %s\n", strsignal(signum));
        if (cleanupFunkcia) {
            fprintf(stderr, "volam cleanup funkciu\n");
            cleanupFunkcia();
        }
    }
    raise(signum);
}

static void sigchldHandler (int signum) {
    int pid, status;
    while ((pid = waitpid(-1, &status, WNOHANG)), (pid > 0)) {
        if (WIFSIGNALED(status)) {
            fprintf(stderr, "proces %d umrel na: %s\n", pid, strsignal(WTERMSIG(status)) );
        }
    }
}

void inicializujSignaly (void (*_cleanupFunkcia)()) {
    hlavnyProces = getpid(); // vo forkoch nechceme volat cleanup funkciu
    cleanupFunkcia = _cleanupFunkcia;
    signal(SIGCHLD, sigchldHandler);
    signal(SIGINT, shutdownHandler);
    signal(SIGTERM, shutdownHandler);
    signal(SIGHUP, shutdownHandler);
    signal(SIGSEGV, shutdownHandler);
    signal(SIGFPE, shutdownHandler);
    signal(SIGPIPE, SIG_IGN);
}

#ifndef NELOGUJ
void logheader () {
    struct timeval stv;
    gettimeofday(&stv, NULL);
    struct tm *stm = localtime(&stv.tv_sec);
    if(stm == NULL) return;
    fprintf(stderr, "[%02d:%02d:%02d.%03ld] ", stm->tm_hour, stm->tm_min, stm->tm_sec, stv.tv_usec/1000);
}
#endif

bool jeAdresar (string filename) {
    struct stat st;
    if (stat(filename.c_str(), &st)) return false;
    return S_ISDIR(st.st_mode);
}

bool jeSubor (string filename) {
    struct stat st;
    if (stat(filename.c_str(), &st)) return false;
    return S_ISREG(st.st_mode);
}

long long gettime () {
  struct timeval tim;
  gettimeofday(&tim, NULL);
  return tim.tv_sec*1000LL + tim.tv_usec/1000LL;
}
