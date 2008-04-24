#ifndef EYESIGHT_H_
#define EYESIGHT_H_

#define WM_CLASS "Eyesight"
#define WM_NAME "eyesight"
#define WM_TITLE "Eyesight Document Viewer v0.1"

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "main.h"
#include "init_libs.h"
#include "conf.h"
#include "main_window.h"
#include "error.h"

#define _(STRING) gettext(STRING)

#endif /*EYESIGHT_H_*/
