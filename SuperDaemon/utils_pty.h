#ifndef _UTILS_PTY_H
#define _UTILS_PTY_H


int utils_pty_prepare_client();
int utils_pty_restore();
int utils_pty_prepare_daemon_child(int slave_ptx);
int utils_create_pty_pair(int *master, int *slave);
int utils_pty_set_terminal_size(int master_ptx, int row, int col);
int utils_pty_get_terminal_size(int master_ptx, int *row, int *col);
#endif