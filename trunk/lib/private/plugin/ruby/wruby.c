#include "wruby.h"


FILE * SimpleFunc(float * modify_me)
{
        FILE * f = fopen ("/home/kamel/oliver.txt", "r");
	* modify_me = 5;
	return f;
}

