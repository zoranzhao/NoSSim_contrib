DEBUG=0

OBJ=ezsift.o image_utility.o
EXECOBJA=image_match.o

INC_DIR=../../../include/
SRC_DIR=../../../src/
APP_DIR=../../../examples/image_match/
EXEC=vgprah

VPATH=$(INC_DIR):$(SRC_DIR):$(APP_DIR)

SLIB=lib${EXEC}.so
ALIB=lib${EXEC}.a
OBJDIR=./obj/

CC=clang
CXX=clang++
AR=ar
ARFLAGS=rcs
OPTS=-Ofast
LDFLAGS= -lm -pthread 
COMMON= -I$(INC_DIR) -I$(SRC_DIR)
CFLAGS=-Wall -Wno-unknown-pragmas -Wfatal-errors -fPIC

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)

EXECOBJ = $(addprefix $(OBJDIR), $(EXECOBJA))
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
	echo $(BYTECODES)
	./$(EXEC) ./data/img1.pgm ./data/img2.pgm

.PHONY: clean

clean:
	rm -rf $(OBJS) $(SLIB) $(ALIB) $(EXEC) $(OBJDIR) *.ppm *.key
