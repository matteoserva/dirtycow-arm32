#include "daemon.h"
#include "client.h"
#include <string.h>
#include <unistd.h> //getuid

int main(int argc, char *argv[]) {


    if ( (argc == 2 && strcmp(argv[1], "--daemon") == 0 ) || getuid() == 0 )  {

        return do_daemon(argc,argv);
    }
	else
		return do_client(argc,argv);
}
