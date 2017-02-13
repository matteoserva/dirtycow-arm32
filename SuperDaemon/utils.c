/*
** Copyright 2012, The CyanogenMod Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#include <fcntl.h> // for open
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>



int provaScalata()
{
	int fd = open("/proc/self/attr/current",O_WRONLY);
	int ret = -1;
	if(fd != -1)
	{
		ret = write(fd,"u:r:super:s0",12);
		close(fd);
	}
	if(ret < 12)
		return -1;
	
	
		return 0;
}

int fork_zero_fucks() {
    int pid = fork();
    if (pid) {
        int status;
        waitpid(pid, &status, 0);
        return pid;
    }
    else {
        if ( (pid = fork()) != 0)
            exit(0);
        return 0;
    }
}


static int move_data(int in, int out)
{
	char buffer[128];
	int letti = read(in,buffer,128);
	if(letti < 0)
		return -1;
	if(letti > 0)
		write(out,buffer,letti);
	return letti;
}





int pump_streams_sync(int in1,int out1,int in2, int out2)
{
	
	int max_fd = in1;
	if(in2 > max_fd)
		max_fd = in2;
		
	fd_set fd_in;
	
	while (1)
	{
				// Wait for data from standard input and master side of PTY
			FD_ZERO(&fd_in);
			FD_SET(in1, &fd_in);
			FD_SET(in2, &fd_in);
			int	rc = select(max_fd + 1, &fd_in, NULL, NULL, NULL);
			if(rc < 0)
				return -1;
				
			
			if(FD_ISSET(in1,&fd_in))
			{
				int scriti = move_data(in1,out1);
				if(scriti <= 0)
					return 0;
			}
			
			if(FD_ISSET(in2,&fd_in))
			{
				int scriti = move_data(in2,out2);
				if(scriti <= 0)
					return 0;
			}
			
	}
}