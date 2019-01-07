#include "profile.h"

static prof_data lib_prof_data[TOTAL_LIB];
static call_stack func_stack;

static long bb_number;
static double current;

static inline double now(){
   struct timeval time;
   if (gettimeofday(&time,NULL)){
      return 0;
   }
   return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

static void acc_time(){
   double pre_duration = now() - current;
   int pre_lib_id = func_stack.lib_id[func_stack.size-1];
   int pre_fun_id = func_stack.fun_id[func_stack.size-1];
   lib_prof_data[pre_lib_id].funcs[pre_fun_id].total_duration = lib_prof_data[pre_lib_id].funcs[pre_fun_id].total_duration + pre_duration; 
}

static void save_profile(){
   int ii, jj;
   FILE *f = fopen("time.prof", "w");
   if (f == NULL){
      printf("Error opening file!\n");
      exit(1);
   }
   for(ii=0;ii<TOTAL_LIB;ii++){
      for(jj=0;jj<MAX_DEPTH;jj++){
         if(lib_prof_data[ii].funcs[jj].call_times!=0){
            fprintf(f, "%d %d %ld %f %ld\n", ii, jj,
               lib_prof_data[ii].funcs[jj].call_times,
               lib_prof_data[ii].funcs[jj].total_duration,
               lib_prof_data[ii].funcs[jj].total_bbs);
         }
      }
   }
   fclose(f);
}

void count_bb(int lib_id, int fun_id){
   bb_number++;
}

void function_start(int lib_id, int fun_id){
   lib_prof_data[lib_id].funcs[fun_id].call_times = lib_prof_data[lib_id].funcs[fun_id].call_times + 1;
   if(func_stack.size!=0){
      /*Accumulate execution duration for the function on the top of calling stack*/
      acc_time();
   }
   /*Push current function onto the top of the calling stack*/ 
   func_stack.lib_id[func_stack.size] = lib_id;
   func_stack.fun_id[func_stack.size] = fun_id;
   func_stack.size++;
   current = now();
}

void function_exit(int lib_id, int fun_id){
   /*Record the execution duration and remove the function from the top of the calling stack*/
   acc_time();
   func_stack.size--;
   if(func_stack.size!=0)
   current = now();
}

void program_start(int lib_id, int fun_id){
   int ii,jj;
   for(ii=0;ii<TOTAL_LIB;ii++){
      for(jj=0;jj<MAX_DEPTH;jj++){
         lib_prof_data[ii].funcs[jj].total_bbs=0;
         lib_prof_data[ii].funcs[jj].call_times=0;
         lib_prof_data[ii].funcs[jj].total_duration=0;
      }
   }
   func_stack.size=0;
   function_start(lib_id, fun_id);
}

void program_end(int lib_id, int fun_id){
   function_exit(lib_id, fun_id);
   save_profile();
}



