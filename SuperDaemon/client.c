#include "client.h"

#include "utils.h" //pump_streams
#include <string.h> // strcpy
#include "connector.h"
#include "utils_pty.h"
#include "config.h"
#include "config_messenger.h"
#include <limits.h>
#include <unistd.h>

int do_client(int argc,char**argv)
{
        char workdir[PATH_MAX];
	int fd = tcp_client_create(DEFAULT_TCP_IP,DEFAULT_TCP_PORT);
	if(fd >= 0)
	{
		void * config = config_data_new();
		int res = utils_pty_prepare_client();
		config_data_set_interactive(config,res<0?0:1);
		
		char * currentDir = getcwd(workdir,PATH_MAX);
		if(currentDir)
		    config_data_set_current_dir(config,currentDir);
		
		int term_rows = 0;
		int term_cols = 0;
		if(!utils_pty_get_terminal_size(0,&term_rows,&term_cols))
		    config_data_set_terminal_size(config,term_rows,term_cols);
		
		config_messenger_send(config,fd);
		pump_streams_sync(0,fd,fd,1);
		while(move_data(fd,1) > 0)
		    ;
		utils_pty_restore();

		return 0;
	}

	return -1;
}