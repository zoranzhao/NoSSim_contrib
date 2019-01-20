SYSTEMC_LIB=/home/slam/OMNET/systemc-2.3.1/lib-linux64
SYSTEMC=/home/slam/OMNET/systemc-2.3.1
HCSIM_DIR=/home/slam/OMNET/HCSim
DARKNET=../../darknet-nnpack
APP=../..
DEEPTHINGS=../../DeepThings
DISTRIOT=../../DistrIoT

LDFLAGS=-L$(SYSTEMC_LIB) -lsystemc -L$(HCSIM_DIR)/lib -lhcsim  -L$(APP) -ldeepthings -L$(DARKNET) -ldarknet -L../hcsim_lwip -ldistriot -Wl,-rpath,$(HCSIM_DIR)/lib:$(SYSTEMC_LIB):$(APP):$(DARKNET):../hcsim_lwip
CFLAGS=-DSC_INCLUDE_DYNAMIC_PROCESSES -I$(SYSTEMC)/include -I$(HCSIM_DIR)/include -I$(DEEPTHINGS)/include -I$(DEEPTHINGS)/src -I$(APP)/include -I$(APP)/src -I$(DISTRIOT)/src -I$(DISTRIOT)/include -I$(DARKNET)/include -I$(DARKNET)/src 

MODULE 	= simple_fifo
OBJS	= $(MODULE).o
EXE	= $(MODULE)

# --- Macros
CFLAGS+= -I. -I../hcsim_lwip
LDFLAGS+= -lm

# --- Tools
CC = clang++ -g

# --- Rules
DEPS = Makefile $(wildcard *.h)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS) 

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $<

%.o: %.cc $(DEPS)
	$(CC) $(CFLAGS) -c $<

test:
	./$(MODULE)

clean:
	rm -f $(OBJS) *.log *~ $(EXE) core

