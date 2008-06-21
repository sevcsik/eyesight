#ifndef CONF_H_
#define CONF_H_

#include <Ecore_Data.h>
#include <Evas.h>
#include "plugin.h"

#define OPTS "t:"

typedef struct _Args
{
   char *theme_path;
   Ecore_List *files;
}
Args;


Args *parse_args(int argc, char **argv, Evas_List **startup_errors);

#endif /*CONF_H_*/
