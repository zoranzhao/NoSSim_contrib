#################################################################
#Helper files for instrumentation, profiling and back-annotation
#################################################################

HELPER=helper
PROFILER=profile
ANNOTATOR=annotation
HELPER_INC =  -Wno-c++11-compat-deprecated-writable-strings -Wno-write-strings  -I. 

####################################################################
# SystemC Options
####################################################################
# SystemC path
HELPER_INC:=$(HELPER_INC) $(DEFINES) 

all: $(PROFILER).cpp $(ANNOTATOR).cpp
	g++ -O3 $(HELPER_INC) -c  $(PROFILER).cpp -o $(PROFILER).o
	g++ -O3 $(HELPER_INC) -c  $(ANNOTATOR).cpp -o $(ANNOTATOR).o 
	/usr/bin/ar cr lib${HELPER}.a $(ANNOTATOR).o $(PROFILER).o
	/usr/bin/ranlib lib${HELPER}.a

clean:
	rm -f *.o *.a *.ll *.log *.bc *~




