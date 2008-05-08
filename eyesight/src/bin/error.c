#include <string.h>
#include <stdio.h>

#include <Evas.h>

#include "error.h"

void
append_startup_error(Evas_List **startup_errors, char *error_string,
                     char *argument)
{
   char *errstr;
   errstr = malloc(strlen(error_string) + strlen(argument) - 1);
   snprintf(errstr, (strlen(error_string) + strlen(argument) - 1), error_string,
            argument);
   *startup_errors = evas_list_append(*startup_errors, errstr);         
}

