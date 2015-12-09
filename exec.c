#include "msg.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


bool put_env(const char *var)
	{
	return putenv((char *)var) == 0 || err(MSG_PUTENV, errno, var);
	}

bool spawn(void (*run)(void *), void *args, const char *msg, const char *id)
	{
	int status;

	switch (fork())
		{
		case -1: die(MSG_FORK, errno, NULL);
		case  0: run(args);
		}
	wait(&status);
	return WEXITSTATUS(status) == 0 || err(msg, 0, id);
	}

void exec(void *args)
	{
	char **cmd = args;

	if (dup2(STDERR_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
		die(MSG_OUT_TO_ERR, errno, NULL);
	execvp(cmd[0], cmd);
	die(MSG_EXEC, errno, cmd[0]);
	}
