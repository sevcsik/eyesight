#ifndef FILE_H_
#define FILE_H_

#include "plugin.h"
#include <Evas.h>
#include <Ecore_Data.h>

typedef struct _Next_Readable_File_Open_Data
{
   Evas *evas;
   Evas_Object *main_window_fg;
   Ecore_List *plugins;
   Ecore_List *files;
   Plugin *plugin;
} Next_Readable_File_Open_Data;

int next_readable_file_open(void *next_file_open_data);

void prev_file_close(Evas *evas, Plugin *plugin);

void next_file_show(Plugin *plugin, Evas *evas, Evas_Object *main_window_fg);

void current_file_hide(Plugin *plugin, Evas *evas, Evas_Object *main_window_fg);


#endif /*FILE_H_*/
