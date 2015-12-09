#include "arch.h"

#include <stdbool.h>


bool put_env(const char *var);
bool spawn(void (*run)(void *), void *args, const char *msg, const char *id);
void exec(void *args);
