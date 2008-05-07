#ifndef ERROR_H_
#define ERROR_H_

#include <libintl.h>
#define _(STRING) gettext(STRING)

//#define ERROR_ARG_NOARG _("-%s option requires an argument, but none has been given.")
#define ERROR_ARG_UNKNOWN _("-%s is an unknown option, or it needs an argument. See README for details.")
#define ERROR_THEME _("Couldn't open %s as theme file. Check that the file exists and it's compatible with this Eyesight version!")

#endif /*ERROR_H_*/
