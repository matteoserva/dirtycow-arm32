#include "config_messenger.h"
#include <stdio.h>
#include "config.h"
#include <unistd.h>

enum CONFIG_OPTIONS {CONFIG_INTERACTIVE};

int config_messenger_send(void* configData, int fd)
{
        char end = 0;
        write(fd,&end,1);
	char inter = config_data_get_interactive(configData);
	char opt = CONFIG_INTERACTIVE;
	write(fd,&opt,1);
	write(fd,&inter,1);
	
	write(fd,&end,1);
	
	return 0;
}
void * config_messenger_receive(void* configData, int fd)
{
        
	
	char opt;
	if(read(fd,&opt,1) <= 0)
	  goto error1;
	
	if(read(fd,&opt,1) <= 0)
	  goto error1;
	
	config_data_set_interactive(configData,opt);
	
	if(read(fd,&opt,1) <= 0)
	  goto error1;
	
	return configData;
	
	error1:
	config_data_delete(configData);
	return NULL;
	
}