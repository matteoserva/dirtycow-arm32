#ifndef _CONFIG_MESSENGER_H
#define _CONFIG_MESSENGER_H


int config_messenger_send(void* , int fd);
void * config_messenger_receive(void*, int fd);


#endif