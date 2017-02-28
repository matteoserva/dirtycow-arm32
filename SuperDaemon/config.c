#include "config.h"
#include <stdlib.h>
#include "string.h"

typedef struct ConfigData
{
	int argc;
	char **argv;
	int interactive;
	unsigned int allocated_argv;
	char * current_dir;
	
	int terminal_size_rows;
	int terminal_size_cols;
	int terminal_size_valid;
} ConfigData;

void* config_data_new()
{
    struct ConfigData* configData = (struct ConfigData *) malloc(sizeof(struct ConfigData));
  
    configData->argc = 0;
    configData->argv = NULL;
    configData->interactive = 0;
    configData->allocated_argv = 0;
    configData->current_dir = NULL;
    configData->terminal_size_valid = 0;
    configData->terminal_size_rows=0 ;
    configData->terminal_size_cols = 0;
    return configData;
}

void config_data_push_argv(void* _cData, char* value)
{
	struct ConfigData* configData = (struct ConfigData *) _cData;
	if(configData->allocated_argv == 0)
	{
		configData->argv = malloc(16*sizeof(char*));
		configData->allocated_argv = 16;
	}
	else if(configData->allocated_argv == configData->argc)
	{
		configData->argv = realloc(configData->argv,2*configData->allocated_argv*sizeof(char*));
		configData->allocated_argv *= 2;
	}
	
	char *buf = NULL;
	if(value)
	{
		int len = strlen(value);
		buf = (char*) malloc((len+10) * sizeof(char));
		strcpy(buf,value);
	}
	
	
	configData->argv[configData->argc]=buf;
	configData->argc = 1+ configData->argc;
}

void config_data_set_interactive(void* _cData, int value)
{
	((struct ConfigData*) _cData)->interactive = value;
}

void config_data_clear_argc_argv(void * _cData)
{
     struct ConfigData* configData = (struct ConfigData *) _cData;
     if(configData->argc > 0)
     {
       for(int i = 0; i < configData->argc; i++)
	 free(configData->argv[i]);
     }
     if(configData->allocated_argv > 0)
     {
       free(configData->argv);
     }
     configData->allocated_argv= 0;
     configData->argc = 0;
     configData->argv = NULL;
}

static void config_data_clear_current_dir(void * _cData)
{
       struct ConfigData* configData = (struct ConfigData *) _cData;
       if(configData->current_dir != NULL)
	{
	  free(configData->current_dir);
	  configData->current_dir = NULL;
	  
	}
}

void config_data_set_current_dir(void * _cData, char* cDir)
{
      struct ConfigData* configData = (struct ConfigData *) _cData;
      config_data_clear_current_dir(_cData);
      if(cDir == NULL)
	return;
      
      int len = strlen(cDir);
      if(len <= 0)
	return;
      configData->current_dir = malloc(len+1);
      strcpy(configData->current_dir,cDir);
      
}


char * config_data_get_current_dir(void * _cData)
{
      struct ConfigData* configData = (struct ConfigData *) _cData;
      
      return configData->current_dir;
}

void config_data_set_terminal_size(void *_cData,int rows, int cols)
{
       if(rows <= 0 || cols <= 0)
	 return;
  
       struct ConfigData* configData = (struct ConfigData *) _cData;
       
       configData->terminal_size_rows = rows;
       configData->terminal_size_cols = cols;
       configData->terminal_size_valid = 1;
}

int config_data_get_terminal_size(void *_cData, int *rows, int*cols)
{
       struct ConfigData* configData = (struct ConfigData *) _cData;
       if(configData->terminal_size_valid <= 0)
	 return -1;
       if(configData->terminal_size_rows <= 0)
	 return -1;
       if(configData->terminal_size_cols <= 0)
	 return -1;
       
       *rows = configData->terminal_size_rows;
       *cols = configData->terminal_size_cols;
       return 0;
}


void config_data_delete(void * _cData)
{
	config_data_clear_argc_argv(_cData);
	config_data_clear_current_dir(_cData);
	free(_cData);
}

int config_data_get_interactive(void* _cData)
{
	return ((struct ConfigData*) _cData)->interactive;
}

char** config_data_get_params(void* _cData)
{
	return ((struct ConfigData*) _cData)->argv;
}