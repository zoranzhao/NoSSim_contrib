DEBUG=0

OBJ=ezsift.o image_utility.o
EXECOBJA=test.o

INC_DIR=../include/
SRC_DIR=../src/
EXEC=vgraph

VPATH=./:$(INC_DIR):$(SRC_DIR)

SLIB=lib${EXEC}.so
ALIB=lib${EXEC}.a
OBJDIR=./obj/

CC=clang
CXX=clang++

AR=ar
ARFLAGS=rcs
OPTS=-Ofast
LDFLAGS= -lm -pthread 
COMMON= -I. -I$(INC_DIR) -I$(SRC_DIR)
CFLAGS=-Wall -Wno-unknown-pragmas -Wfatal-errors -fPIC

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)

EXECOBJ = $(addprefix $(OBJDIR), $(EXECOBJA))
EXECBC = $(EXECOBJ:.o=.bc) 
EXECCPP = $(EXECOBJA:.o=.cpp) 
OBJS = $(addprefix $(OBJDIR), $(OBJ))
BYTECODES = $(OBJS:.o=.bc) 
DEPS = $(wildcard $(SRC_DIR)/*.h) Makefile 

all: obj $(SLIB) $(ALIB) $(EXEC)

$(EXEC): $(EXECOBJ) $(ALIB)
	$(CXX) $(COMMON) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(ALIB)

$(ALIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(SLIB): $(OBJS)
	$(CXX) $(CFLAGS) -shared $^ -o $@ $(LDFLAGS)

$(OBJDIR)%.o: %.cpp $(DEPS)
	$(CXX) $(COMMON) $(CFLAGS) -c $< -o $@


obj:
	mkdir -p obj

test:
	./$(EXEC)

.PHONY: clean

clean:
	rm -rf $(OBJS) $(SLIB) $(ALIB) $(EXEC) $(OBJDIR) *.ppm *.key
