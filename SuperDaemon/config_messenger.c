#include "config_messenger.h"
#include <stdio.h>
#include "config.h"
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <limits.h>

enum CONFIG_OPTIONS {CONFIG_START_END=0,CONFIG_INTERACTIVE, CONFIG_WORKDIR,CONFIG_TERMSIZE};

static int config_messenger_send_interactive(void* configData, int fd)
{
	char inter = config_data_get_interactive(configData);
	char opt = CONFIG_INTERACTIVE;
	if( 0 > write(fd,&opt,1) )
	    return -1;
	if( 0 > write(fd,&inter,1) )
	    return -1;
        return 0;
}

static int config_messenger_receive_interactive(void* configData, int fd)
{
	char opt;
	if(read(fd,&opt,1) <= 0)
	      return -1;
        config_data_set_interactive(configData,opt);
	return 0;
}

static int config_messenger_send_current_work_dir(void* configData, int fd)
{
	char *dir = config_data_get_current_dir(configData);
	if(!dir)
	  return 0;
	uint32_t str_len = strlen(dir);
	
	if(str_len <= 0)
	  return 0;
	uint32_t mem_len = str_len + 1;
	char opt = CONFIG_WORKDIR;
	if( 0 > write(fd,&opt,1) )
	    return -1;
	
	uint32_t nlen = htonl(mem_len);
	
	if( 0 > write(fd,&nlen,sizeof(nlen)) )
	    return -1;
	if( 0 > write(fd,dir,mem_len) )
	    return -1;
	return 0;
	
}

static int config_messenger_receive_current_work_dir(void* configData, int fd)
{
	char strBuffer[PATH_MAX];
        uint32_t nlen;
	if( 0 > read(fd,&nlen,sizeof(nlen)) )
	    return -1;
	
	uint32_t mem_len = ntohl(nlen);
	
	if(mem_len > PATH_MAX)
	  return -1;
	
	if( 0 > read(fd,strBuffer,mem_len) )
	    return -1;
	
	config_data_set_current_dir(configData,strBuffer);
	return 0;
}

static int config_messenger_send_terminal_size(void* configData, int fd)
{
        int rows;
	int cols;
	
	if(config_data_get_terminal_size(configData,&rows,&cols))
	     return 0;
	
	if(rows <= 0 || cols <= 0)
	  return 0;
	
	uint32_t _rows = htonl(rows);
	uint32_t _cols = htonl(cols);
	
	char opt = CONFIG_TERMSIZE;
        if( 0 > write(fd,&opt,1) )
	    return -1;
	if( 0 > write(fd,&_rows,sizeof(_rows)) )
	    return -1;
	if( 0 > write(fd,&_cols,sizeof(_cols)) )
	    return -1;
	return 0;
}

static int config_messenger_receive_terminal_size(void* configData, int fd)
{
        uint32_t _rows;
	uint32_t _cols;
	if( 0 > read(fd,&_rows,sizeof(_rows)) )
	    return -1;
	if( 0 > read(fd,&_cols,sizeof(_cols)) )
	    return -1;
	
	_rows = ntohl(_rows);
	_cols = ntohl(_cols);
	if(_rows<= 0 || _cols <= 0)
	  return -1;
	
	config_data_set_terminal_size(configData,_rows,_cols);
	return 0;
  
}

int config_messenger_send(void* configData, int fd)
{
        char end = CONFIG_START_END;
        write(fd,&end,1);
	
	config_messenger_send_interactive(configData,fd);
	config_messenger_send_current_work_dir(configData,fd);
	config_messenger_send_terminal_size(configData,fd);
	write(fd,&end,1);
	
	return 0;
}



void * config_messenger_receive(void* configData, int fd)
{
        
        /* The START_END message is already received */
	
	char opt;
	char result;
	while(1)
	{
	   if(read(fd,&opt,1) <= 0)
	      goto error1;
	   result = -1;
	   switch(opt)
	   {
	     case CONFIG_INTERACTIVE:
	       result = config_messenger_receive_interactive(configData,fd);
	       break;
	       
	     case CONFIG_START_END:
	       result = 1;
	       break;
	     
	     case CONFIG_WORKDIR:
	       result = config_messenger_receive_current_work_dir(configData,fd);
	       break;
	       
	     case CONFIG_TERMSIZE:
	       result = config_messenger_receive_terminal_size(configData,fd);
	       break;
	     default:
	        result = -1;
		break;
	   }
	  
	   if(result == 1)
	     break;
	   else if(result < 0)
	     goto error1;
	   
	  
	  
	}

	
	return configData;
	
	error1:
	config_data_delete(configData);
	return NULL;
	
}