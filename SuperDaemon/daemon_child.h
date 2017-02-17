#ifndef _DAEMON_CHILD_H
#define _DAEMON_CHILD_H

int daemon_child_temp(int type, int clientFd);

int daemon_child_execute(char**argv);

#endif