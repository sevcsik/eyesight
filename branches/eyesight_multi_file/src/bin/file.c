#include <Evas.h>
#include <Ecore_Data.h>
#include <stdio.h>

#include "plugin.h"
#include "file.h"

int
next_readable_file_open(void *_data)
{
   // Check plugins in the list if they can open it, break on success
   
   Next_Readable_File_Open_Data *data = _data;
   Plugin *plugin;
   Plugin_Open_Next_Func open_next;
   char *file;
   
   // Reset plugin list
   plugin = ecore_list_goto(data->plugins, ecore_list_first(data->plugins));
   
   while(file = ecore_list_next(data->files)) // Try every file...
   {
      do // ... with every plugin...
      {
         open_next = ecore_plugin_symbol_get(plugin->plugin, "open_next");
         if (!open_next)
         {
            printf("No symbol \'open_next\' symbol found in plugin %s\n",
                   plugin->name);
            continue;
         }
         if (open_next(plugin->plugin_data, file, data->main_window_fg, data->evas))
         {
            data->plugin = plugin;
            return 1; // ... until one gets opened succesfully.
         }
      } while (plugin = ecore_list_next(data->plugins));
   }
   return 0;
}

void 
prev_file_close(Evas *evas, Plugin *plugin)
{
   Plugin_Close_Prev_Func close_prev;
   if (!(close_prev = ecore_plugin_symbol_get(plugin->plugin, "close_prev")))
   {
      printf("No \'close_file\' symbol found in plugin %s", plugin->name);
      return;
   }
   close_prev(&(plugin->plugin_data), evas);
}

void
next_file_show(Plugin *plugin, Evas *evas, Evas_Object *main_window_bg)
{
   Plugin_Show_Next_Func show_next;
   if (!(show_next = ecore_plugin_symbol_get(plugin->plugin, "show_next")))
   {
      printf("No symbol \'show\' found in plugin %s", plugin->name);
      return;
   }
   show_next(&(plugin->plugin_data), main_window_bg, evas);
}

void
current_file_hide(Plugin *plugin, Evas *evas, Evas_Object *main_window_bg)
{
   Plugin_Hide_Current_Func hide_current;
   if (!(hide_current = ecore_plugin_symbol_get(plugin->plugin, "hide_current")))
   {
      printf("No symbol \'hide\' found in plugin %s", plugin->name);
      return;
   }
   hide_current(&(plugin->plugin_data), main_window_bg, evas);
}
