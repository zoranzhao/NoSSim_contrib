INS_LIB = ../InstrumentLLVM/build/FunctionInstr/libLLVMFunctionInstr.so
LLVM_INSTR_PASS = -functionInstr
INS_STAGE=annot#preprof#prof#annot

COPTS=-emit-llvm
LLVM_CC=clang++
LLVM_LINK=llvm-link-6.0
LLVM_LLC=llc-6.0
CFLAGS=-g -Wall

SRC_FILES=test.c
BC_FILES=test.bc

all: $(SRC_FILES)
	$(LLVM_CC) $(COPTS) $(CFLAGS) -c $(SRC_FILES)
	$(LLVM_LINK) $(BC_FILES) -o test.all.bc
	opt -load $(INS_LIB) $(LLVM_INSTR_PASS) -libname=12 -InstruProf=$(INS_STAGE) < test.all.bc > test.instru.bc
	$(LLVM_LLC) -filetype=obj test.instru.bc -o test.o
	$(LLVM_CC) -o test test.o

