#ifndef CONF_H_
#define CONF_H_

#define OPTS "t:"

typedef struct _Args
{
   char *theme_path;
   Evas_List *files;
}
Args;

Args *parse_args(int argc, char **argv);

#endif /*CONF_H_*/
