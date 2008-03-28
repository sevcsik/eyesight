#include "Eye.h"

char * parse_args(int argc, char **argv)
{
	// TODO: parse_args()
}

int main(int argc, char **argv)
{
    // gettext stuff
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
	
	if (!init_libs(argc, argv)) exit(-1);
	
	if (parse_args(argc, argv));
	
	ecore_main_loop_begin();
}
