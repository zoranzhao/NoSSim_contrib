#include "profile.h"

ProfData LibProfData[20];
CallingTracker Tracker;

long bb_number;
double current;

static inline double now(){
   struct timeval time;
   if (gettimeofday(&time,NULL)){
      return 0;
   }
   return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

static void acc_time(){
   double pre_duration = now() - current;
   int pre_lib_id = Tracker.LibID[Tracker.NumFuncInExec-1];
   int pre_fun_id = Tracker.FunID[Tracker.NumFuncInExec-1];
   LibProfData[pre_lib_id].Funcs[pre_fun_id].TotalCycles = LibProfData[pre_lib_id].Funcs[pre_fun_id].TotalCycles + pre_duration; 
}

static void save_profile(){
   int ii, jj;
   FILE *f = fopen("ProfileData", "w");
   if (f == NULL){
      printf("Error opening file!\n");
      exit(1);
   }
   for(ii=0;ii<20;ii++){
      for(jj=0;jj<10000;jj++){
         if(LibProfData[ii].Funcs[jj].CallingTimes!=0){
            fprintf(f, "%d %d %ld %f %ld\n", ii, jj,
               LibProfData[ii].Funcs[jj].CallingTimes,
               LibProfData[ii].Funcs[jj].TotalCycles,
               LibProfData[ii].Funcs[jj].TotalBBs);
         }
      }
   }
   fclose(f);
}

void count_bb(int LibID, int FunID){
   bb_number++;
}

void function_start(int LibID, int FunID){
   LibProfData[LibID].Funcs[FunID].CallingTimes = LibProfData[LibID].Funcs[FunID].CallingTimes + 1;
   if(Tracker.NumFuncInExec!=0){
      /*Accumulate execution duration for the function on the top of calling stack*/
      acc_time();
   }
   /*Push current function onto the top of the calling stack*/ 
   Tracker.LibID[Tracker.NumFuncInExec] = LibID;
   Tracker.FunID[Tracker.NumFuncInExec] = FunID;
   Tracker.NumFuncInExec++;
   current = now();
}

void function_exit(int LibID, int FunID){
   /*Record the execution duration and remove the function from the top of the calling stack*/
   acc_time();
   Tracker.NumFuncInExec--;
   if(Tracker.NumFuncInExec!=0)
   current = now();
}

void program_start(int LibID, int FunID){
   int ii,jj;
   for(ii=0;ii<20;ii++){
      for(jj=0;jj<10000;jj++){
         LibProfData[ii].Funcs[jj].CallingTimes=0;
         LibProfData[ii].Funcs[jj].TotalCycles=0;
      }
   }
   Tracker.NumFuncInExec=0;
   function_start(LibID, FunID);
}

void program_end(int LibID, int FunID){
   function_exit(LibID, FunID);
   save_profile();
}



