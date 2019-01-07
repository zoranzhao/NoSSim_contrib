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

#ifndef __PROFILE__H
#define __PROFILE__H


#define MAX_ALLOWED 15
#define N 1
#define MIN_GRANU 200000

extern "C" void CountBB(int LibID, int FunID);
extern "C" void papi_instrument_func_entry(int LibID, int FunID);
extern "C" void papi_instrument_func_exit(int LibID, int FunID);
extern "C" void program_start(int LibID, int FunID);
extern "C" void program_end(int LibID, int FunID);



typedef struct CallingTrackerStruct {
  int FunID[10000];//depth of calling stack
  int LibID[10000];//depth of calling stack
  int NumFuncInExec;
} CallingTracker;



typedef struct FuncProfileData {
  long TotalBBs;
  long CallingTimes;
  long TotalCycles;
} FuncProfData;



typedef struct ProfileData {
  FuncProfData Funcs[10000]; //indexed by function ID
} ProfData;


#endif
