#ifndef CONF_H_
#define CONF_H_

#define OPTS "t:"

struct _args
{
	char *theme_path;
   Evas_List *files;
} args;

void parse_args(int argc, char **argv);

#endif /*CONF_H_*/
