#ifndef _DAEMON_CONFIG_H
#define _DAEMON_CONFIG_H



void config_data_push_argv(void* _cData, char* value);
void config_data_set_interactive(void* _cData, int value);
int config_data_get_interactive(void* _cData);
void* config_data_new();
void config_data_delete(void *);
char** config_data_get_params(void* _cData);
void config_data_clear_argc_argv(void * _cData);

#endif