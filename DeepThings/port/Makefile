
HCSIM=1
ifeq ($(HCSIM), 1) 
SYSTEMC_LIB=/home/slam/OMNET/systemc-2.3.1/lib-linux64
SYSTEMC_INC=/home/slam/OMNET/systemc-2.3.1
HCSIM_DIR=/home/slam/OMNET/HCSim
LDFLAGS+=-L$(SYSTEMC_LIB) -lsystemc -L$(HCSIM_DIR)/lib -lhcsim -Wl,-rpath,$(SYSTEMC_LIB)
CFLAGS+=-DSC_INCLUDE_DYNAMIC_PROCESSES -I$(SYSTEMC_INC)/include -I$(HCSIM_DIR)/include
endif
