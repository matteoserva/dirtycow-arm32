#ifndef _CONNECTOR_H
#define _CONNECTOR_H

#define DEFAULT_TCP_IP "127.0.0.1"
#define DEFAULT_TCP_PORT 11112

#define DEFAULT_UNIX_NAME "\0MAGISKSU"

int tcp_daemon_create(char * ipAddress, unsigned short port);
int tcp_client_create(char * ipAddress, unsigned short port);

int unix_daemon_client(char *socketName);
int unix_daemon_create(char *socketName);



#endif