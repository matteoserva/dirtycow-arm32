#ifndef _DAEMON_CONFIG_H
#define _DAEMON_CONFIG_H



void config_data_push_argv(void* _cData, char* value);
void config_data_set_interactive(void* _cData, int value);
int config_data_get_interactive(void* _cData);
void* config_data_new();
void config_data_delete(void *);
char** config_data_get_params(void* _cData);
void config_data_clear_argc_argv(void * _cData);
char * config_data_get_current_dir(void * _cData);
void config_data_set_current_dir(void * _cData, char* cDir);
int config_data_get_terminal_size(void *_cData, int *rows, int*cols);
void config_data_set_terminal_size(void *_cData,int rows, int cols);

#endif