

#include "connector.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h> //fchmod
#include <string.h>




int unix_daemon_create(char *socketName) {

    int fd;
    struct sockaddr_un sun;

    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }
    if (fcntl(fd, F_SETFD, FD_CLOEXEC)) {
       
        goto err;
    }

    memset(&sun, 0, sizeof(sun));
    sun.sun_family = AF_LOCAL;
    strcpy(sun.sun_path, socketName);

    unlink(sun.sun_path);

    if (bind(fd, (struct sockaddr*)&sun, sizeof(sun)) < 0) {
        goto err;
    }

    fchmod(fd, 0777);

    if (listen(fd, 10) < 0) {
        goto err;
    }

	return fd;

err:
    close(fd);
    return -1;
}


int unix_daemon_client(char *socketName) {
	

    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));

    struct sockaddr_un sun;

    // Open a socket to the daemon
    int socketfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (socketfd < 0) {
        
        return(-1);
    }
    if (fcntl(socketfd, F_SETFD, FD_CLOEXEC)) {
        close(socketfd);
        return (-1);
    }

    memset(&sun, 0, sizeof(sun));
    sun.sun_family = AF_LOCAL;
    strcpy(sun.sun_path, socketName);

    if (0 != connect(socketfd, (struct sockaddr*)&sun, sizeof(sun))) {
        close(socketfd);
        return (-1);
    }

    return socketfd;
}






