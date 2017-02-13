#ifndef _DAEMON_DECLARATIONS_H
#define _DAEMON_DECLARATIONS_H
int unix_daemon_handle(int fd);
int unix_daemon_client(int argc, char *argv[]);
int read_int(int fd);
void write_int(int fd, int val);
 char* read_string(int fd);
 void write_string(int fd, char* val) ;
 int recv_fd(int sockfd);
 void send_fd(int sockfd, int fd);
 void setup_sighandlers(void);
 
 #ifdef LOGD
 #undefine LOGD
 #endif
 #ifdef LOGE
 #undefine LOGE
 #endif
 #ifdef PLOGE
 #undefine PLOGE
 #endif

 #define LOGD(...) ; 
#define LOGE(...) ; 

#define PLOGE(...) ; 
#endif
