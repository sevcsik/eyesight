#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <Ecore_Data.h>
#include <Evas.h>

/*
 * = STUFF FOR PLUGINS =
 */

/* 
 * _Plugin_Data struct:
 * this structure is passed to plugins to manipulate eyesight's GUI
 */
typedef struct _Plugin_Data
{
   Evas *evas;                // Main window's canvas
   Evas_Object *main_window;  // Main window's edje object
} Plugin_Data;

/* 
 * Plugin type:
 * used for keeping track of plugin data
 */
typedef struct _Plugin
{
   Ecore_Plugin *plugin;
   void *plugin_data; // A place where plugin can store its stuff
   char *file;
   char *name;
   char *version;
   char *email;
   char state;
} Plugin;

/* Plugin init() return codes */
#define PLUGIN_INIT_API_MISMATCH 1
#define PLUGIN_INIT_FAIL 2
#define PLUGIN_INIT_SUCCESS 0

/* Check this in plugin init() */
#define PLUGIN_API_VERSION 1

/*
 * Plugin identify() function:
 * this function should set the supplied pointers to the according strings. If
 * any of these strings are NULL (which is default) plugin won't be loaded.
 */
typedef void (*Plugin_Identify_Func) (char **name, char **version, char **email);

/*
 * Plugin init() function:
 * this function should check the api version and init the libs used
 */
typedef int (*Plugin_Init_Func) (Plugin *plugin_data);

#endif /*PLUGIN_H_*/
