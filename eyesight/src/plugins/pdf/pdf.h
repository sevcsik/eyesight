#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <Evas.h>

void identify(char **name, char **version, char **email);
int init(void **plugin_data);
int open_file(void **plugin_data, char *filename, Evas_Object *main_window,
              Evas *evas);
void show(void **plugin_data, char *filename, Evas_Object *main_window, 
          Evas *evas);

#endif /*PLUGIN_H_*/
