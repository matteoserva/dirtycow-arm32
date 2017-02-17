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

#ifndef _UTILS_H_
#define _UTILS_H_


int provaScalata();
int fork_zero_fucks();
int move_data(int in, int out);
int pump_streams_sync(int in1,int out1,int in2, int out2);
int pump_streams_sync_clean(int in1,int out1,int in2, int out2);
void replace_standard_streams(int input,int output);
int utils_create_pipe_pair(int *master, int *slave);


enum COMM_TYPE { COMM_NONE,COMM_PTY,COMM_PIPE };
void closeUnwantedCommFd(int commType, int* storage, int* inpOut,int isMaster);
int openCommFd(int wantedType,int* storage);



#endif
