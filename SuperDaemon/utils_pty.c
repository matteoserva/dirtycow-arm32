#include "utils_pty.h"
#include <termios.h>
#include <signal.h>
#include <unistd.h> //STDIN_fileno
#include <string.h> //memset

#include <sys/ioctl.h>

//for posix openpt
#define __USE_XOPEN2KXSI
//for grantpt
#define __USE_XOPEN
//for ptsname_r
#define __USE_GNU
#include <stdlib.h>
#undef __USE_XOPEN2KXSI
#undef __USE_XOPEN
#undef __USE_GNU

#include <limits.h> //PATH_MAX



#include <fcntl.h> //O_RDWR

static int quit_signals[] = { SIGALRM, SIGHUP, SIGPIPE, SIGQUIT, SIGTERM, SIGINT, 0 };
static struct termios saved_terset;
static unsigned int saved_terset_available = 0;


int utils_pty_restore()
{
	if(saved_terset_available != 1)
		return -1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_terset) < 0) {
        return -1;
    }
	saved_terset_available = 0;
	
	return 0;
}


static int utils_pty_set_stdin_raw(int fd) {
    struct termios new_termios,old_stdin;

    // Save the current stdin termios
    if (tcgetattr(fd, &old_stdin) < 0) {
        return -1;
    }

    // Start from the current settings
    new_termios = old_stdin;

    // Make the terminal like an SSH or telnet client
    new_termios.c_iflag |= IGNPAR;
    new_termios.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXANY | IXOFF);
    new_termios.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHONL);
    new_termios.c_oflag &= ~OPOST;
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSAFLUSH, &new_termios) < 0) {
        return -1;
    }

   

    return 0;
}


static void sighandler(int sig) {
	(void)sig;
    

	utils_pty_restore();
	
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Put back all the default handlers
    struct sigaction act;
    int i;

    memset(&act, '\0', sizeof(act));
    act.sa_handler = SIG_DFL;
    for (i = 0; quit_signals[i]; i++) {
        if (sigaction(quit_signals[i], &act, NULL) < 0) {
           
            continue;
        }
    }
}

static void utils_pty_setup_sighandlers(void) {
    struct sigaction act;
    int i;

    // Install the termination handlers
    // Note: we're assuming that none of these signal handlers are already trapped.
    // If they are, we'll need to modify this code to save the previous handler and
    // call it after we restore stdin to its previous state.
    memset(&act, '\0', sizeof(act));
    act.sa_handler = &sighandler;
    for (i = 0; quit_signals[i]; i++) {
        if (sigaction(quit_signals[i], &act, NULL) < 0) {
            
            continue;
        }
    }
}


static void save_terset(struct termios *new_terset)
{
	saved_terset = *new_terset;
	saved_terset_available=1;
	
}


int utils_create_pty_pair(int *master, int *slave)
{
  *master = -1;
  *slave = -1;
  
  int _master_ptx, _slave_ptx; 
  char name_buffer[PATH_MAX];
  _master_ptx = posix_openpt(O_RDWR| O_NOCTTY); 
  if (_master_ptx < 0) 
	return -1;
  
  
	if(grantpt(_master_ptx))
		goto error1; 
	if(unlockpt(_master_ptx))
		goto error1; 

	ptsname_r(_master_ptx,name_buffer,PATH_MAX-1);
	
	_slave_ptx  = open(name_buffer, O_RDWR); 

	if(_slave_ptx < 0)
		goto error1;

	*master = _master_ptx;
	*slave = _slave_ptx;
	return 0;
  
error1:  
   close(_master_ptx);
  return -1;
  
}

int utils_pty_prepare_daemon_child(int slave_ptx)
{
  struct termios slave_orig_term_settings; // Saved terminal settings 
		struct termios new_term_settings; // Current terminal settings 
	if(!isatty(slave_ptx))
		return -1;
          setsid();
	  //utils_pty_set_stdin_raw(slave_ptx);
	  tcgetattr(slave_ptx, &slave_orig_term_settings); 
	  new_term_settings = slave_orig_term_settings;
	  new_term_settings.c_oflag &= ~(ONLCR| OCRNL | ONLRET|ONLRET);
	  new_term_settings.c_lflag |= ISIG;
	  tcsetattr (slave_ptx, TCSANOW, &new_term_settings);	 
	  ioctl(slave_ptx, TIOCSCTTY, 1);
	
	return 0;
}

int utils_pty_get_terminal_size(int master_ptx, int *row, int *col)
{
   *row = -1;
   *col = -1;
   
   if(!isatty(master_ptx))
	return -1;
    struct winsize w;
   if (ioctl(master_ptx, TIOCGWINSZ, &w) == -1)
     return -1;
   
   *row = w.ws_row;
   *col = w.ws_col;
   return 0;
  
}

int utils_pty_set_terminal_size(int master_ptx, int row, int col)
{
  if(row <= 0 && col <= 0)
    return 0;
  
  
  if(!isatty(master_ptx))
		return -1;
	      struct winsize w;
        	if (ioctl(master_ptx, TIOCGWINSZ, &w) != -1) {
		 if(col > 0)
		    w.ws_col = col;
		 if(row > 0)
		    w.ws_row = row;
	        ioctl(master_ptx, TIOCSWINSZ, &w);

	        }
  return 0;
}

int utils_pty_prepare_client()
{
	if(!isatty(0) || !isatty(1))
		return -1;

	struct termios old_terset, new_terset;
	tcgetattr(0, &old_terset); 
	new_terset = old_terset;
	
	
	new_terset.c_lflag &= ~(ECHO);
	new_terset.c_oflag =  OPOST | ONLCR | ONOCR ;
	new_terset.c_lflag = 0;
	new_terset.c_iflag = ICRNL  ;
	//new_terset.c_cc[VINTR] = 1;
	tcsetattr (0, TCSANOW, &new_terset);
	
	save_terset(&old_terset);

	utils_pty_setup_sighandlers();
	
	return 0;
}