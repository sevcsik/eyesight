#include "plugin.h"
#include <Epdf.h>
#define API_VERSION_REQ 1

void 
identify(char **name, char **version, char **email)
{
   // TODO: pdf.c: i18n
   *name = "PDF viewer (using epdf)";
   *version = "0.1";
   *email = "sevcsik@gmail.com";
}

int 
init(Plugin *plugin_data)
{
   if (PLUGIN_API_VERSION != API_VERSION_REQ) return PLUGIN_INIT_API_MISMATCH;
   // No need to init epdf
   return PLUGIN_INIT_SUCCESS;
}

int 
open_file(void *plugin_data, char *filename, Evas_Object *main_window,
          Evas *evas)
{
   
}
