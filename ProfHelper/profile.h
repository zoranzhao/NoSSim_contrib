#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <float.h>
#include <limits.h>
#include <time.h>

#include <sys/time.h>

#ifndef PROFILE_H
#define PROFILE_H

#ifdef __cplusplus
extern "C" {
#endif

void count_bb(int LibID, int FunID);
void function_start(int LibID, int FunID);
void function_exit(int LibID, int FunID);
void program_start(int LibID, int FunID);
void program_end(int LibID, int FunID);

typedef struct CallingTrackerStruct {
  int FunID[10000];//depth of calling stack
  int LibID[10000];//depth of calling stack
  int NumFuncInExec;
} CallingTracker;

typedef struct FuncProfileData {
  long TotalBBs;
  long CallingTimes;
  double TotalCycles;
} FuncProfData;

typedef struct ProfileData {
  FuncProfData Funcs[10000]; //indexed by function ID
} ProfData;

#ifdef __cplusplus
}//extern "C"
#endif

#endif
