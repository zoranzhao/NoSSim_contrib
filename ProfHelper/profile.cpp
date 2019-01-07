#include "profile.h"

long long results[N];
char* events[] = {"PAPI_TOT_CYC"};
int event_codes[N];
bool init_already = 0;

ProfData LibProfData[20]; //Number of libs used, indexed by lib ID

CallingTracker Tracker;

long ProfTotalBBs;


void DumpProf(){
  int ii, jj;
  FILE *f = fopen("ProfileData", "w");

  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  for(ii=0;ii<20;ii++){
     for(jj=0;jj<10000;jj++){
         if(LibProfData[ii].Funcs[jj].CallingTimes!=0){
                fprintf(f, "%d %d %ld %ld %ld\n", ii, jj,
                                LibProfData[ii].Funcs[jj].CallingTimes,
                                LibProfData[ii].Funcs[jj].TotalCycles,
                                LibProfData[ii].Funcs[jj].TotalBBs);
//Format is LibID, FunID, CallingTimes, TotalCycles, TotalBBS
/*
                printf("%d %d %ld\n", ii, jj, 
                        LibProfData[ii].Funcs[jj].TotalCycles/LibProfData[ii].Funcs[jj].CallingTimes);
                printf("%d %d %d\n", ii, jj, 
                        LibProfData[ii].Funcs[jj].TotalCycles/LibProfData[ii].Funcs[jj].CallingTimes > MIN_GRANU);
*/
         }

     }
  }
  fclose(f);
}


//Initialize the papi low level api
void papi_init() {
  init_already = 1;
  ProfTotalBBs = 0;
  int retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval != PAPI_VER_CURRENT && retval > 0) {
    std::cerr << "PAPI library version mismatch!" << std::endl;
    exit(1);
  }


  if (N > MAX_ALLOWED){
    std::cerr << "Only <= 3 counter events allowed!" << std::endl;
    exit(1);
  }
  for (int i = 0; i < N; i++){
    PAPI_event_name_to_code(events[i], &event_codes[i]);
  }


  int ii;
  int jj;
  for(ii=0;ii<20;ii++){
	for(jj=0;jj<10000;jj++){
		LibProfData[ii].Funcs[jj].CallingTimes=0;
		LibProfData[ii].Funcs[jj].TotalCycles=0;
	}
  }

  Tracker.NumFuncInExec=0;


}


void papi_start(){
  PAPI_start_counters(event_codes, N);
}

void papi_stop(int LibID, int FunID){
  PAPI_stop_counters(results, N);
//  printf("Calling Papi Stop %ld\n", results[0]);
  LibProfData[LibID].Funcs[FunID].TotalBBs = LibProfData[LibID].Funcs[FunID].TotalBBs + ProfTotalBBs;
  ProfTotalBBs = 0;
  LibProfData[LibID].Funcs[FunID].TotalCycles = LibProfData[LibID].Funcs[FunID].TotalCycles + results[0];
}


extern "C"{

void CountBB(int LibID, int FunID){
	ProfTotalBBs++;
}

void papi_instrument_func_entry(int LibID, int FunID){
   
//	if( (LibID == 0) && (FunID ==4) ) return;
    LibProfData[LibID].Funcs[FunID].CallingTimes = LibProfData[LibID].Funcs[FunID].CallingTimes + 1;
    if(Tracker.NumFuncInExec!=0)
//    	{papi_stop(CallingStack.back());}
    	{papi_stop(Tracker.LibID[Tracker.NumFuncInExec-1], Tracker.FunID[Tracker.NumFuncInExec-1]);}
//    CallingStack.push_back(FunID);
    Tracker.LibID[Tracker.NumFuncInExec] = LibID;
    Tracker.FunID[Tracker.NumFuncInExec] = FunID;
    Tracker.NumFuncInExec++;
    papi_start();
}

void papi_instrument_func_exit(int LibID, int FunID){

//	if( (LibID == 0) && (FunID ==4) ) return;
    //papi_stop( CallingStack.back());
    papi_stop(Tracker.LibID[Tracker.NumFuncInExec-1], Tracker.FunID[Tracker.NumFuncInExec-1]);
    //CallingStack.pop_back();
    Tracker.NumFuncInExec--;
   // if(!CallingStack.empty())
    if(Tracker.NumFuncInExec!=0)
     papi_start();
}

void program_start(int LibID, int FunID){
    if(!init_already)
	{papi_init();}
    papi_instrument_func_entry(LibID, FunID);
}

void program_end(int LibID, int FunID){
    papi_instrument_func_exit(LibID, FunID);
    DumpProf();

}

}

