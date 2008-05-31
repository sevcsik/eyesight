#ifndef PLUGIN_H_
#define PLUGIN_H_

void identify(char **name, char **version, char **email);
int init(Plugin *plugin_data);
int open_file(void *plugin_data, char *filename, Evas_Object *main_window,
              Evas *evas);

#endif /*PLUGIN_H_*/
