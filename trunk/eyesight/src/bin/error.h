#ifndef ERROR_H_
#define ERROR_H_

#include <libintl.h>
#define _(STRING) gettext(STRING)

#include <Evas.h>
#include <Ecore_Evas.h>

//#define ERROR_ARG_NOARG _("-%s option requires an argument, but none has been given.")
#define ERROR_STARTUP_BEGIN _("During startup, the following errors occured:")
#define ERROR_ARG_UNKNOWN _("-%s is an unknown option, or it needs an argument. See README for details.")
#define ERROR_THEME _("Couldn't open %s as theme file. Check that the file exists and it's compatible with this Eyesight version!")
#define ERROR_PLUGIN_NO_SYMBOL _("Couldn't find symbol \"%s\" in plugin \"%s\". Check that it's compatible with this Eyesight version!")
#define ERROR_PLUGIN_NO_ID _("Can't identify plugin \"%s\". Check that it's compatible with this Eyesight version!")

void append_startup_error(Evas_List **startup_errors, char *error_string,
                          char *argument);
void display_startup_error_dialog(Ecore_Evas *ee, Evas_List *startup_errors);

#endif /*ERROR_H_*/
