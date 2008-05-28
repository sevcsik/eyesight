#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <Ecore_Data.h>
#include <Evas.h>

typedef struct _Plugin
{
	Ecore_Plugin *plugin;
	void *plugin_data;
   char *file;
   char *name;
   char *version;
   char *email;
   char state;
} Plugin;

typedef struct _Plist_Data
{
   Ecore_List *plugin_list;
   Ecore_Path_Group *pg;
} Plist_Data;

/* PLUGIN INIT ERROR CODES */
#define PLUGIN_INIT_API_MISMATCH 1
#define PLUGIN_INIT_FAIL 2
#define PLUGIN_INIT_SUCCESS 0

#define PLUGIN_API_VERSION 1

Plist_Data *plugin_init(Evas_List **startup_errors);

#endif /*PLUGIN_H_*/
