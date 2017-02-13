#include "unix_daemon_declarations.h"
#include <sys/types.h>
#include <limits.h> //PATH_MAX
#include <sys/socket.h>
#include <sys/un.h>
#include "su.h"
#include "pts.h"
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
       #include <fcntl.h>
#include "utils.h"
#include <termios.h> 
#include <stdio.h>
// Constants for the atty bitfield
#define ATTY_IN     1
#define ATTY_OUT    2
#define ATTY_ERR    4

   


int unix_daemon_client(int argc, char *argv[]) {
	

    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    struct sockaddr_un sun;

    // Open a socket to the daemon
    int socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (socketfd < 0) {
        PLOGE("socket");
        exit(-1);
    }
    if (fcntl(socketfd, F_SETFD, FD_CLOEXEC)) {
        PLOGE("fcntl FD_CLOEXEC");
        exit(-1);
    }

    memset(&sun, 0, sizeof(sun));
    sun.sun_family = AF_LOCAL;
    strcpy(sun.sun_path, REQUESTOR_DAEMON_PATH);

    if (0 != connect(socketfd, (struct sockaddr*)&sun, sizeof(sun))) {
        PLOGE("connect");
        exit(-1);
    }

    LOGD("connecting client %d", getpid());
	struct termios slave_orig_term_settings, slave_new_settings;
	if(isatty(0))
	{
		
		 tcgetattr(0, &slave_orig_term_settings); 
		 slave_new_settings = slave_orig_term_settings;
		  cfmakeraw (&slave_new_settings); 
		 
		 //slave_new_settings.c_lflag &= (ICANON);
		 
		 slave_new_settings.c_iflag =0x5500 ;//|= (IXANY|IUCLC |ICRNL| INLCR );
		 struct termios s = slave_new_settings;
		 printf("%X,%X,%X,%X\n",s.c_iflag,s.c_oflag,s.c_cflag,s.c_iflag);
		 // new_term_settings.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
			//new_term_settings.c_oflag &= ~(ONLCR);
		  
		  tcsetattr (0, TCSANOW, &slave_new_settings);
		 // setsid();
			// set_stdin_raw();
		
	}
	
	pump_streams_sync(0,socketfd,socketfd,1);

    if(isatty(0))
	{
		tcsetattr (0, TCSANOW, &slave_orig_term_settings);
		
	}

    return 0;
}
