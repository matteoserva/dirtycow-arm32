#include <stdio.h>
#include <fcntl.h> // for open
#include "daemon.h"

#include "utils.h"
#include "client.h"
#include "pts.h"
#include <termios.h>

int unix_daemon_client(int argc,char**argv);

 void setup_sighandlers(void);
int do_client(int argc,char**argv)
{
	int fd = tcp_client_create(argc,argv);
	if(fd >= 0)
	{
		struct termios old_terset, terset;
		if(isatty(0))
		{
		
		tcgetattr(0, &terset); 
		old_terset = terset;
		  
		 
		   terset.c_oflag &= ~(ONLRET|ONLCR);
		   terset.c_oflag |= ONLCR;
		   terset.c_lflag &= ~(ECHO);
		   terset.c_lflag = 0xf0;
		   
		   
		   terset.c_oflag = ONLRET |ONLCR | ONLRET | OPOST;
		   terset.c_lflag = 0;
		   
		   
		   terset.c_iflag = 0;
		   terset.c_cc[VINTR] = 1;
		  tcsetattr (0, TCSANOW, &terset);
		  tcgetattr(0, &terset); 
		   printf ("%x\n",terset.c_oflag );
		  
		} 
		setup_sighandlers();
		pump_streams_sync(0,fd,fd,1);
		if(isatty(0))
			tcsetattr(0,TCSANOW,&old_terset);
		
		
		return 0;
	}
	
	
	return unix_daemon_client(argc,argv);
}


int main(int argc, char *argv[]) {
	
	
    if ( (argc == 2 && strcmp(argv[1], "--daemon") == 0 ) || getuid() == 0 )  {
        
        return do_daemon(argc,argv);
    }
	else
		return do_client(argc,argv);
}