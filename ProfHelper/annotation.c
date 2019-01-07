#include "annotation.h"
#include "profile.h"
#define OS_MODEL_ANNOTATION

long long cycles = 0;
long AnnotTotalBBs;
long FuncBBs[10][10000];
int CurFunID = 0;
int CurLibID = 0;
CallingTracker AnnotationTracker;

extern "C"{

	void CountBB_BA(int LibID, int FunID){	
		AnnotTotalBBs++;
		//FuncBBs[LibID][FunID] = FuncBBs[LibID][FunID] + AnnotTotalBBs;
		//AnnotTotalBBs = 0;
	}

	void RecordFuncBBs( int LibID, int FunID){


	    if(AnnotationTracker.NumFuncInExec!=0)
	//    	{CallingStack.back();}
	    	{ 
			FuncBBs[AnnotationTracker.LibID[AnnotationTracker.NumFuncInExec-1]][AnnotationTracker.FunID[AnnotationTracker.NumFuncInExec-1]] = 
				FuncBBs[AnnotationTracker.LibID[AnnotationTracker.NumFuncInExec-1]][AnnotationTracker.FunID[AnnotationTracker.NumFuncInExec-1]] + AnnotTotalBBs;

		}
	//    CallingStack.push_back(FunID);
	    AnnotationTracker.LibID[AnnotationTracker.NumFuncInExec] = LibID;
	    AnnotationTracker.FunID[AnnotationTracker.NumFuncInExec] = FunID;
	    AnnotationTracker.NumFuncInExec++;
            AnnotTotalBBs = 0;
	}

	void FunctionDelay(double alpha, long delay, int LibID, int FunID){

		FuncBBs[LibID][FunID] = FuncBBs[LibID][FunID] + AnnotTotalBBs;
		AnnotTotalBBs = 0;
		printf("%d:%d:%ld\n", LibID, FunID, FuncBBs[LibID][FunID]);
		//cycles = cycles + (long)(alpha*((double) FuncBBs[LibID][FunID] )) + delay;
		FuncBBs[LibID][FunID] = 0;

		//CallingStack.pop_back();
		AnnotationTracker.NumFuncInExec--;
	}

	void program_start_BA(int LibID, int FunID){

		int ii;
		int jj;
		for(ii=0;ii<10;ii++)
		  for(jj=0;jj<10000;jj++){
			FuncBBs[ii][jj] = 0;	
		  }
		AnnotTotalBBs = 0;
		AnnotationTracker.NumFuncInExec=0;
		RecordFuncBBs(LibID, FunID);
	}

	void program_end_BA(int LibID, int FunID){
		//FunctionDelay(0,0,LibID, FunID);
		//printf("Total cycles is: %lld\n", cycles);
	}

}

