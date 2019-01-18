SYSTEMC_LIB=/home/slam/OMNET/systemc-2.3.1/lib-linux64
SYSTEMC=/home/slam/OMNET/systemc-2.3.1
HCSIM_DIR=/home/slam/OMNET/HCSim
DEEPTHINGS=../..
DARKNET=../../darknet-nnpack


LDFLAGS=-L$(SYSTEMC_LIB) -lsystemc -L$(HCSIM_DIR)/lib -lhcsim  -L$(DEEPTHINGS) -ldeepthings -L$(DARKNET) -ldarknet -L../hcsim_lwip -ldistriot -Wl,-rpath,$(HCSIM_DIR)/lib:$(SYSTEMC_LIB):$(DEEPTHINGS):$(DARKNET):../hcsim_lwip
CFLAGS=-DSC_INCLUDE_DYNAMIC_PROCESSES -I$(SYSTEMC)/include -I$(HCSIM_DIR)/include -I$(DEEPTHINGS)/include -I$(DEEPTHINGS)/src 

MODULE 	= simple_fifo
OBJS	= $(MODULE).o
EXE	= $(MODULE)

# --- Macros
CFLAGS+= -I. -I../hcsim_lwip
LDFLAGS+= -lm

# --- Tools
CC = clang++

# --- Rules
.SUFFIXES: .cc .cpp .o

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS) 

.cpp.o:
	$(CC) $(CFLAGS) -c $<

.cc.o:
	$(CC) $(CFLAGS) -c $<

test:
	./$(MODULE)

clean::
	rm -f $(OBJS) *.log *~ $(EXE) core


