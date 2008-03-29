#ifndef CONF_H_
#define CONF_H_

struct
{
	char *theme_path;
} args;

void parse_args(int argc, char **argv);

#endif /*CONF_H_*/
