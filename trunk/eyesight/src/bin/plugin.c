#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <Ecore_Data.h>
#include <Evas.h>

#include "plugin.h"
#include "error.h"
#include "config.h"

#define PLUGIN_ERROR 0
#define PLUGIN_OK 1

#define PLUGIN_FILE 0
#define PLUGIN_NAME 1
#define PLUGIN_VERSION 2
#define PLUGIN_EMAIL 3

typedef void (*Plugin_Identify_Func) (char **name, char **version, char **email);

void
plugin_init(Evas_List **startup_errors)
{
   /* Find plugins */
   
   char *tmp;
   
   Ecore_Path_Group *pg = ecore_path_group_new();
   Ecore_List *plugins;
   
   // TODO: plugin.c: get path group from config file  
   ecore_path_group_add(pg, PACKAGE_PLUGIN_DIR);
   
   // TODO: plugin.c: getenv("HOME"): Windows compatibility?
   tmp = calloc(sizeof(char), strlen(getenv("HOME")) + strlen("/.eyesight/plugins" + 1));
   sprintf(tmp, "%s/.eyesight/plugins", getenv("HOME"));
   ecore_path_group_add(pg, tmp);
   
   plugins = ecore_plugin_available_get(pg);
   
   free(tmp);
   
   /* Identify plugins */
   
   typedef struct _Plist_Data
   {
        Evas_List *plugin_list;
        Ecore_Path_Group *pg;
   } Plist_Data;
   
   Plist_Data *plist_data;
   plist_data = malloc(sizeof(Plist_Data));
   plist_data->pg = pg;
   
   void plugin_for_each(void *val, void *data)
   {
      printf("Identifying plugin: %s\n", val);
      char *name = NULL;
      char *version = NULL;
      char *email = NULL;
      char state = PLUGIN_ERROR;
      Plugin_Identify_Func identify;
      Ecore_Plugin *plugin;
      Plist_Data *plist_data = (Plist_Data *)data;
      char *pdata[5];
      
      plugin = ecore_plugin_load(plist_data->pg, val, NULL);
      identify = ecore_plugin_symbol_get(plugin, "identify");
      if (identify)
      {
         if (name && version && email)
         {
            identify(&name, &version, &email);
            pdata[PLUGIN_FILE] = val;
            pdata[PLUGIN_NAME] = name;
            pdata[PLUGIN_VERSION] = version;
            pdata[PLUGIN_EMAIL] = email;
            plist_data->plugin_list = evas_list_append(plist_data->plugin_list,
                                                       pdata);
         }
         else
         {
            append_startup_error(startup_errors, ERROR_PLUGIN_NO_ID, val);     
         }
      }      
   }
   
   ecore_list_for_each(plugins, plugin_for_each, plist_data);  
}
