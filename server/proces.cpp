//universal magic
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstdio>
#include <string>

using namespace std;

#include "proces.h"

Proces::Proces () : cwd("."), pid(-1), writefd(-1), readfd(-1) {}

int Proces::getPid() {
    return pid;
}
//TODO timeout
void Proces::write (string data) {
    unsigned cur = 0;
//     fprintf(stderr, "%d\n", data.size());
    while (cur < data.size()) {
        int wlen = 1024;
        if (cur + wlen > data.size()) wlen = data.size() - cur;
        int status = ::write(writefd, data.c_str() + cur, wlen);
        if (status != (int) wlen){// && errno!=EAGAIN) {
            if (status == -1) {
                fprintf(stderr, "write(%d): pid %d: %s\n",cur, pid, strerror(errno));
            }
            else {
                fprintf(stderr, "write: pid %d: zapisali sme len %d bajtov z %d\n", pid, status, wlen);
            }
            return ;
        }
        //if(errno!=EAGAIN)
        cur += status;
    }
}

string Proces::read (unsigned cap) {
    // nonblocking --- lebo pri vyrabani sme dali O_NONBLOCK
    char buf[cap];
    int status = ::read(readfd, buf, cap);
    if (status == -1) {
        if (errno != EAGAIN) {
            fprintf(stderr, "read: pid %d: %s\n", pid, strerror(errno));
        }
        return "";
    }
    return string(buf, status);
}

void Proces::zabi () {
    if (pid != -1) {
        int status = kill(pid, SIGTERM);
        if (status == -1) {
            fprintf(stderr, "zabi/kill: pid %d: %s\n", pid, strerror(errno));
        }
        pid = -1;
        status = close(writefd);
        if (status == -1) {
            fprintf(stderr, "zabi/close_writefd: pid %d: %s\n", pid, strerror(errno));
        }
        writefd = -1;
        status = close(readfd);
        if (status == -1) {
            fprintf(stderr, "zabi/close_readfd: pid %d: %s\n", pid, strerror(errno));
        }
        readfd = -1;
    }
}

void Proces::restartuj () {
    zabi();

    int parent2child[2];
    int child2parent[2];
    if (pipe(parent2child) != 0) {
        fprintf(stderr, "restartuj/parent2child: pid %d: %s\n", pid, strerror(errno));
        return;
    }
    if (pipe(child2parent) != 0) {
        fprintf(stderr, "restartuj/child2parent: pid %d: %s\n", pid, strerror(errno));
        return;
    }

    int flags = fcntl(child2parent[0], F_GETFL);
    fcntl(child2parent[0], F_SETFL, flags | O_NONBLOCK);
    
//     int flags2 = fcntl(parent2child[1], F_GETFL);
//     fcntl(parent2child[1], F_SETFL, flags2 | O_NONBLOCK);
//     
    int status = fork();
    if (status == -1) {
        fprintf(stderr, "restartuj/fork: pid %d: %s\n", pid, strerror(errno));
        return;
    }
  
    if (status == 0) {
        setsid();
        dup2(parent2child[0], 0);
        dup2(child2parent[1], 1);
        if (!(errfile == "")) {
            int errfd = open(errfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (errfd == -1) {
                fprintf(stderr, "restartuj/child/open: pid %d: %s\n", pid, strerror(errno));
            }
            else {
                dup2(errfd, 2);
                close(errfd);
            }
        }
        close(parent2child[0]);
        close(parent2child[1]);
        close(child2parent[0]);
        close(child2parent[1]);
        if (chdir(cwd.c_str()) == -1) {
            fprintf(stderr, "restartuj/chdir: %s: %s\n", cwd.c_str(), strerror(errno));
            exit(1);
        }
        char** _args = (char**) calloc(args.size() + 1, sizeof(char*));
        for (unsigned i = 0; i < args.size(); i++) _args[i] = (char*)args[i].c_str();
        execv(_args[0], _args);
        
        fprintf(stderr, "restartuj/execv: %s\n", strerror(errno));
        exit(1); // automaticky dealokuje _command ???
    }
    pid = status;
    writefd = parent2child[1];
    readfd = child2parent[0];
    close(parent2child[0]);
    close(child2parent[1]);
}

void Proces::setProperties (vector<string> _args, string _cwd, string _errfile) {
    zabi();
    args = _args;
    cwd = _cwd;
    errfile = _errfile;
}

bool Proces::zije () {
    return (kill(pid, 0)==0);
}
