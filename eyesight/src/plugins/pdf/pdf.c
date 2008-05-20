#include "plugin.h"

void identify(char **name, char **version, char **email)
{
   *name = "PDF viewer (using epdf)";
   *version = "0.1";
   *email = NULL;
}

int init()
{
   
}