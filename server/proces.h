//universal proboj magic
#ifndef PROCES_H
#define PROCES_H

#include <vector>
#include <string>

class Proces {
    private:
        std::vector<std::string> args;
        std::string cwd;
        std::string errfile;
        int pid;
        int writefd;
        int readfd;

    public:
        Proces () ;

        int getPid () ;
        void write (std::string data) ;
        std::string read (unsigned cap) ;
        void zabi () ;
        void restartuj () ;

        void setProperties (std::vector<std::string> _args, std::string _cwd, std::string _errfile) ;

        bool zije () ;
};

#endif

