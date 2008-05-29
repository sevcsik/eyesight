#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <Ecore_Data.h>
#include <Evas.h>

#include "plugin.h"
#include "plugin_private.h"
#include "error.h"
#include "config.h"

#define PLUGIN_ERROR    0
#define PLUGIN_OK       1

#define PLUGIN_FILE     0
#define PLUGIN_NAME     1
#define PLUGIN_VERSION  2
#define PLUGIN_EMAIL    3



Plist_Data *
plugin_init(Evas_List **startup_errors)
{
   /* Find plugins */
   char *tmp;
   
   Ecore_Path_Group *pg = ecore_path_group_new();
   Ecore_List *plugins;
   
   // TODO: plugin.c: get path group from config file  
   ecore_path_group_add(pg, PACKAGE_PLUGIN_DIR);
   
   // TODO: plugin.c: getenv("HOME"): Windows compatibility?
   tmp = calloc(sizeof(char), strlen(getenv("HOME")) + strlen("/.eyesight/plugins") + 1);
   snprintf(tmp, strlen(getenv("HOME")) + strlen("/.eyesight/plugins") +1, 
            "%s/.eyesight/plugins", getenv("HOME"));
   ecore_path_group_add(pg, tmp);
   
   plugins = ecore_plugin_available_get(pg);
   
   free(tmp);
   
   /* Identify plugins */
   
   Plist_Data *plist_data;
   plist_data = malloc(sizeof(Plist_Data));
   plist_data->pg = pg;
   plist_data->plugin_list = ecore_list_new();
   
   void plugin_id_for_each(void *val, void *_data)
   {
      printf("Identifying plugin: %s\n", val);
      
      Plist_Data *data = _data;
      Plugin *plugin = malloc(sizeof(Plugin));
      plugin->name = NULL;
      plugin->version = NULL;
      plugin->email = NULL;
      plugin->file = NULL;
      plugin->state = 0;
      Plugin_Identify_Func identify;
      
      plugin->plugin = ecore_plugin_load(data->pg, val, NULL);
      identify = ecore_plugin_symbol_get(plugin->plugin, "identify");
      if (identify)
      {
         identify(&plugin->name, &plugin->version, &plugin->email);
         if (plugin->name && plugin->version && plugin->email)
         {
            plugin->state = PLUGIN_OK;
            plugin->file = val;
            ecore_list_append(data->plugin_list, plugin);
         }            
         else
         {
            append_startup_error(startup_errors, ERROR_PLUGIN_NO_ID, val);
            free(val);
            free(plugin);
         }
      }
      else
      {
         append_startup_error2(startup_errors, ERROR_PLUGIN_NO_SYMBOL,
                               "identify", val);
         free(val);
         free(plugin);
      }
   }
   
   ecore_list_for_each(plugins, plugin_id_for_each, plist_data);
   ecore_list_free_cb_set(plugins, NULL);
   ecore_list_destroy(plugins);
   
   /* Init plugins */
   
   void plugin_init_for_each(void *_val, void *data)
   {      
      Plugin *val = _val;
      printf("Initializing plugin: %s %s\n", val->file, val->email);
      Plugin_Init_Func init = NULL;
      init = ecore_plugin_symbol_get(val->plugin, "init");
      if (init)
      {
         switch (init(val->plugin_data))
         {
         case PLUGIN_INIT_API_MISMATCH:
            append_startup_error2(startup_errors, ERROR_PLUGIN_INIT_API_MISMATCH,
                                  val->name, val->email);
            break;
         case PLUGIN_INIT_FAIL:
            append_startup_error(startup_errors, ERROR_PLUGIN_INIT_FAIL,
                                 val->name);
            break;
         }
      }
      else
         append_startup_error2(startup_errors, ERROR_PLUGIN_NO_SYMBOL,
                               "init", val->name);
   }
   
   ecore_list_for_each(plist_data->plugin_list, plugin_init_for_each, NULL);
   
   return plist_data; // for later use
}
