#include "lang.h"

#include <stdbool.h>
#include <string.h>
#include <unistd.h>


#define WRITE_STR(fd, s) write(fd, s, strlen(s))


bool err(const char *msg, int syserr, const char *id);
void die(const char *msg, int syserr, const char *id);
