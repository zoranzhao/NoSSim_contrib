OPENMP=1
NNPACK=0
ARM_NEON=0
DEBUG=1
DEEPTHINGS=/home/slam/Desktop/DeepThings/DeepThings
DARKNET=$(DEEPTHINGS)/darknet-nnpack

VPATH=./src
DISTRIOT=$(DEEPTHINGS)/distriot
OBJDIR=./obj/
EXEC=deepthings
DARKNETLIB=libdarknet.a
DISTRIOTLIB=libdistriot.a

CC=gcc
LDFLAGS= -lm -pthread
CFLAGS=-Wall -fPIC
COMMON=-I$(DISTRIOT)/include/ -I$(DISTRIOT)/src/ -I$(DARKNET)/include/ -I$(DARKNET)/src/ -Iinclude/ -Isrc/ 
LDLIB=-L$(DISTRIOT) -l:$(DISTRIOTLIB) -L$(DARKNET) -l:$(DARKNETLIB)

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
DEPS = $(wildcard */*.h) Makefile

all: obj $(EXEC)

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