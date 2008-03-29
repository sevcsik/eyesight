#ifndef MAIN_H_
#define MAIN_H_

struct
{
	Ecore_Evas *ee;
	Evas *evas;
	Evas_Object *edje;
} main_window;

char * parse_args(int argc, char **argv);

#endif /*MAIN_H_*/
