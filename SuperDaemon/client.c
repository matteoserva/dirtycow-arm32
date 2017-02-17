#include "client.h"

#include "utils.h" //pump_streams
#include <string.h> // strcpy
#include "connector.h"
#include "utils_pty.h"
#include "config.h"
#include "config_messenger.h"


int do_client(int argc,char**argv)
{
	int fd = tcp_client_create(DEFAULT_TCP_IP,DEFAULT_TCP_PORT);
	if(fd >= 0)
	{
		void * config = config_data_new();
		int res = utils_pty_prepare_client();
		config_data_set_interactive(config,res<0?0:1);
		config_messenger_send(config,fd);
		pump_streams_sync(0,fd,fd,1);
		while(move_data(fd,1) > 0)
		    ;
		utils_pty_restore();

		return 0;
	}

	return -1;
}