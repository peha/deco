#ifndef OPTION
#define OPTION
struct option
	{
	char key;
	int args;
	void *var;
	};
#endif


char **parse_opts(char **argv, const struct option *opts);
