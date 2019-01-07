#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>

#include <vector>
#include <pthread.h> 
#include <papi.h>
#include <cstdio>
#include <cstdlib>


#ifndef __ANNOTATION__H
#define __ANNOTATION__H


#define MAX_ALLOWED 15
#define N 1
#define MIN_GRANU 200000

extern "C" void CountBB_BA(int LibID, int FunID);
extern "C" void FunctionDelay(double alpha, long delay, int LibID, int FunID);
extern "C" void program_start_BA(int LibID, int FunID);
extern "C" void program_end_BA(int LibID, int FunID);
extern "C" void RecordFuncBBs( int LibID, int FunID);


#endif
