#include "daemon_father.h"
#include "daemon_child.h"
#include "config.h"
#include "utils.h"
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h> //close
#include "utils_pty.h"
#include "config_messenger.h"

 char *default_exe1[] = {"/system/bin/sh", NULL};
 char *default_exe2[] = {"/bin/bash", NULL};



int daemon_father_start_child(char ** params)
{
	
	daemon_child_execute(params);
	daemon_child_execute(default_exe1);
	daemon_child_execute(default_exe2);
	return -1;
}


static int daemon_father_loop(void* configData, char*preBuffer, int preBufferSize)
{
	int interactive = config_data_get_interactive(configData);
	
	fflush(stdout);
	
	char * wanted_cwd = config_data_get_current_dir(configData);
	if(wanted_cwd)
	{
	    chdir(wanted_cwd); 
	}
	
	
	if(preBufferSize == 0 && interactive == 0)
	{
		return daemon_father_start_child(config_data_get_params(configData));
	}
	
	int commFd[4];
	
	int commType = openCommFd(interactive?COMM_PTY:COMM_PIPE,commFd);
	
	if(commType == COMM_NONE)
	{
		return daemon_father_start_child(config_data_get_params(configData));
	}
	
	int usableCommFd[2];
	
	int cpid = fork();
	if(cpid < 0)
		return daemon_father_start_child(config_data_get_params(configData));
	
	closeUnwantedCommFd(commType,commFd,usableCommFd,cpid>0?1:0);
	
	if(cpid > 0)
	{
	        int term_rows = 0;
		int term_cols = 60;
		config_data_get_terminal_size(configData,&term_rows,&term_cols);
		utils_pty_set_terminal_size(usableCommFd[1],term_rows,term_cols);
		if(preBufferSize > 0)
			write(usableCommFd[1],preBuffer,preBufferSize);
		
		pump_streams_sync(0,usableCommFd[1],usableCommFd[0],1);
		
		return (0);
		
		
	}
	else
	{
		replace_standard_streams(usableCommFd[0],usableCommFd[1]);
		utils_pty_prepare_daemon_child(1);
		return daemon_father_start_child(config_data_get_params(configData));
	}
  
  return -1;
}


static void* create_default_config_data()
{
	void* configData = config_data_new();
	char ** params = default_exe1;
	
	for(int i = 0; params[i] != NULL; i++)
		config_data_push_argv(configData,params[i]);
	config_data_push_argv(configData,NULL);
	config_data_set_interactive(configData,0);
	
	return configData;
}


int daemon_father_run(int fd,void* _configData)
{
	replace_standard_streams(fd,fd);
	
	provaScalata();
	
	/* ORA TENTO DI CAPIRE SE CLIENT */
	
	char buffer[16];
	
	void* configData = NULL;
	
	fd_set rfds;
	struct timeval tv;
	tv.tv_sec=0;
	tv.tv_usec=250000;
	FD_ZERO(&rfds);
	FD_SET(0,&rfds);
	int pronti = select(1, &rfds, NULL, NULL, &tv);
	
	if(pronti == 0)
	  return daemon_father_loop(create_default_config_data(),NULL,0);
	
	
	int letti = read(0,buffer,1);
	if(letti < 1)
	{
	   return -1;
	}
	
	if(buffer[0] == 0)
	{
	  configData = config_messenger_receive(create_default_config_data(),0);
	  if(!configData)
	  {
	    return -1;
	  }
	  letti = 0;
	  
	}
	else
	{
	  configData = create_default_config_data();
	}
	
	return daemon_father_loop(configData,buffer,letti);

}