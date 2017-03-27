//universal magic
#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sstream>

void inicializujSignaly (void (*cleanupFunkcia)()) ;

#ifdef NELOGUJ
#define loguj(...) (0)
#else
void logheader () ;
#include <cstdio>
#define loguj(...) (logheader(), fprintf(stderr, __VA_ARGS__), fprintf(stderr, "\n"))
#endif

bool jeAdresar (std::string) ;
bool jeSubor (std::string) ;

long long gettime () ;

#endif
