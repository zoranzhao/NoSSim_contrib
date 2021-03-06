HCSIM=1
OPENMP=1
NNPACK=0
ARM_NEON=0
DEBUG=1
DARKNET=./darknet-nnpack
#DistrIoT ported with lwIP/HCSim
DISTRIOT=./port/hcsim_lwip
DEEPTHINGS=./DeepThings

VPATH=$(DEEPTHINGS)/src:./src
OBJDIR=./obj/
EXEC=deepthings
DARKNETLIB=libdarknet.a
DISTRIOTLIB=libdistriot.a
SLIB=lib$(EXEC).so
ALIB=lib$(EXEC).a

CC=g++
AR=ar
ARFLAGS=rcs
LDFLAGS= -lm -pthread
CFLAGS=-Wall -fPIC
COMMON=-I$(DEEPTHINGS)/include/ -I$(DEEPTHINGS)/src/ -I$(DISTRIOT)/include/ -I$(DISTRIOT)/src/ -I$(DARKNET)/include/ -I$(DARKNET)/src/ -Iinclude/ -Isrc/ 
LDLIB=-L$(DISTRIOT) -l:$(DISTRIOTLIB) -L$(DARKNET) -l:$(DARKNETLIB)

ifeq ($(HCSIM), 1)
LWIP=../../lwip-hcsim
COMMON+=-I$(DISTRIOT)/lwip -I$(DISTRIOT)/hcsim -I$(LWIP)/src/include -I$(LWIP)/ports/hcsim -I$(LWIP)/ports/hcsim/context -I$(LWIP)/ports/port/include
SYSTEMC_LIB=/home/slam/OMNET/systemc-2.3.1/lib-linux64
SYSTEMC=/home/slam/OMNET/systemc-2.3.1
HCSIM_DIR=/home/slam/OMNET/HCSim
LDFLAGS+=-L$(SYSTEMC_LIB) -lsystemc -L$(HCSIM_DIR)/lib -lhcsim -Wl,-rpath,$(SYSTEMC_LIB)
COMMON+=-DSC_INCLUDE_DYNAMIC_PROCESSES -I$(SYSTEMC)/include -I$(HCSIM_DIR)/include
endif

ifeq ($(OPENMP), 1) 
CFLAGS+= -fopenmp
endif

ifeq ($(DEBUG), 1) 
OPTS+=-O0 -g
else
OPTS+=-Ofast
endif

ifeq ($(NNPACK), 1)
COMMON+= -DNNPACK
CFLAGS+= -DNNPACK
LDFLAGS+= -lnnpack -lpthreadpool
endif

ifeq ($(ARM_NEON), 1)
COMMON+= -DARM_NEON
CFLAGS+= -DARM_NEON -mfpu=neon-vfpv4 -funsafe-math-optimizations -ftree-vectorize
endif

CFLAGS+=$(OPTS)
OBJS = test.o test_utils.o ftp.o inference_engine_helper.o frame_partitioner.o adjacent_reuse_data_serialization.o self_reuse_data_serialization.o deepthings_edge.o deepthings_gateway.o cmd_line_parser.o
EXECOBJ = $(addprefix $(OBJDIR), $(OBJS))
LIBOBJS = test_utils.o ftp.o inference_engine_helper.o frame_partitioner.o adjacent_reuse_data_serialization.o self_reuse_data_serialization.o deepthings_edge.o deepthings_gateway.o cmd_line_parser.o
LIBOBJ = $(addprefix $(OBJDIR), $(LIBOBJS))

DEPS = $(wildcard */*.h) Makefile

all: obj $(SLIB) $(ALIB) 

$(ALIB): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^

$(SLIB): $(LIBOBJ)
	$(CC) $(CFLAGS) -shared $^ -o $@

$(EXEC): $(EXECOBJ) $(DARKNETLIB) $(DISTRIOTLIB)
	$(CC) $(COMMON) $(CFLAGS) $(EXECOBJ) -o $@  $(LDLIB) $(LDFLAGS)

$(DARKNETLIB):
	$(MAKE) -C $(DARKNET) NNPACK=$(NNPACK) ARM_NEON=$(ARM_NEON) DEBUG=$(DEBUG) OPENMP=$(OPENMP) all

$(DISTRIOTLIB):
	$(MAKE) -C $(DISTRIOT) DEBUG=$(DEBUG) all

$(OBJDIR)%.o: %.c $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

test:
	./deepthings ${ARGS}

.PHONY: clean clean_all

clean_all:
	make -C $(DISTRIOT) clean
	make -C $(DARKNET) clean
	rm -rf $(EXEC) $(EXECOBJ) *.log $(OBJDIR) *.png 

clean:
	rm -rf $(EXEC) $(EXECOBJ) *.log $(OBJDIR) *.png 
