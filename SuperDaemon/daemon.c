
#include "daemon.h"
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h> //close
#include "utils.h" //fork_zero_fucks
#include <stdio.h>

#include "connector.h"
#include "daemon_father.h"


#define TYPE_UNIX 1
#define TYPE_TCP 2

struct HandlersStruct_s
{

	int fd;
	int type;
};


static int create_daemons(struct HandlersStruct_s* daemonStructs, int ssize, int argc,char**argv)
{

	int un_fd;

	unsigned int numHandlers = 0;
	un_fd = unix_daemon_create(DEFAULT_UNIX_NAME);
	if(un_fd >= 0)
	{
		struct HandlersStruct_s tmp;
		tmp.type = TYPE_UNIX;
		tmp.fd = un_fd;
		daemonStructs[numHandlers++] = tmp;
	}

	un_fd = tcp_daemon_create(DEFAULT_TCP_IP,DEFAULT_TCP_PORT);
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
				return daemon_father_run(client,NULL);
				
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
