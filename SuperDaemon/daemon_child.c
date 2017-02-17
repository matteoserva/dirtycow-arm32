


#include <unistd.h> //close

#include "daemon_child.h"



int daemon_child_execute(char**argv)
{
	execv(argv[0],argv);
	return -1;
}


