#ifndef PDF_H_
#define PDF_H_

#include <Evas.h>

void identify(char **name, char **version, char **email);
char init(void **plugin_data);
char open_file(void **_plugin_data, char *filename, Evas_Object *main_window,
              Evas *evas);
void show(void **plugin_data, char *filename, Evas *evas);
void setup_toolbar(Evas_Object *controls);

#endif /*PDF_H*/
