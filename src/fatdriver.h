#ifndef FATDRIVER_H
#define FATDRIVER_H
#include <stdint.h>

struct rde;

void fatInit(void);

struct rde* fatOpen(const char* path);

int fatRead(struct rde* rde, char* buf, int n);

#endif
