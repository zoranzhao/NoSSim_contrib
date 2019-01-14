HCSIM=1

VPATH=./src
SLIB=libdistriot.so
ALIB=libdistriot.a
OBJDIR=./obj/

CC=gcc
CXX=g++
AR=ar
ARFLAGS=rcs
LDFLAGS= -lm -pthread 
COMMON= -Iinclude/ -Isrc/
CFLAGS=-Wall -fPIC

ifeq ($(HCSIM), 1) 
SYSTEMC_LIB=/home/slam/OMNET/systemc-2.3.1/lib-linux64
SYSTEMC_INC=/home/slam/OMNET/systemc-2.3.1
HCSIM_DIR=/home/slam/OMNET/HCSim
LDFLAGS+=-L$(SYSTEMC_LIB) -lsystemc -L$(HCSIM_DIR)/lib -lhcsim -Wl,-rpath,$(SYSTEMC_LIB)
CFLAGS+=-DSC_INCLUDE_DYNAMIC_PROCESSES -I$(SYSTEMC_INC)/include -I$(HCSIM_DIR)/include
endif

ifeq ($(DEBUG), 1) 
OPTS+=-O0 -g
else
OPTS+=-Ofast
endif

CFLAGS+=$(OPTS)
OBJ = thread_safe_queue.o thread_util.o data_blob.o network_util.o exec_ctrl.o gateway.o client.o global_context.o

OBJS = $(addprefix $(OBJDIR), $(OBJ))
DEPS = $(wildcard */*.h) Makefile

all: obj $(SLIB) $(ALIB) 

$(ALIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(SLIB): $(OBJS)
	$(CC) $(CFLAGS) -shared $^ -o $@

$(OBJDIR)%.o: %.c $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

.PHONY: clean

clean:
	rm -rf $(OBJS) $(SLIB) $(ALIB) obj *.log
