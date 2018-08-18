#include "OmnetIf_pkt.h"
#include <omnetpp.h>
#include <systemc>

using namespace omnetpp;
using namespace ::sc_core;
using namespace ::std;

#ifndef SC_NIC__H
#define SC_NIC__H

class Nic : public sc_module {
   public:
	sc_core::sc_port< sc_fifo_out_if<int> > size_out;
	sc_core::sc_port< sc_fifo_in_if<int> > size_in;

        sc_core::sc_port< sc_fifo_out_if<char*> > data_out;
        sc_core::sc_port< sc_fifo_in_if<char*> > data_in;


	int NodeID;
   	sc_event recvd; 
   	sc_event sent; 
   	int recvd_flag; 
   	int sent_flag; 

	char* data_send;
	int size_send;
	char* data_recv;
	int size_recv;
	cSimpleModule* OmnetWrapper;

	SC_HAS_PROCESS(Nic);

	Nic(sc_module_name name, int NodeID) : sc_module(name)
	{
		recvd_flag = 0; 
   		sent_flag = 0; 
		this -> NodeID = NodeID;
		SC_THREAD(NicRecv);
		SC_THREAD(NicSend);
	}

	void NicSend()
	{
	  while (1) {
		size_send = size_in -> read();
		data_send = data_in -> read();
		cSimpleModule* wrapper = (cSimpleModule*)(OmnetWrapper);
		cContextSwitcher dummy1(wrapper); //VERY IMPORTANT
		OmnetIf_pkt* pkt = new OmnetIf_pkt();
		pkt->setFileBufferArraySize(size_send);
		for(int ii=0; ii<size_send; ii++){
			pkt->setFileBuffer(ii, ((char*)data_send)[ii]);
		}
		free(data_send);
		cMessage *startMsg = new cMessage("ServerToCli");
		startMsg->setContextPointer(pkt);
		wrapper->scheduleAt(simTime(), startMsg);  //Notify immediately
		wait(this->sent);
	  }
	}
	void NicRecv()
	{
		while (1) {
			wait(recvd);
			size_out -> write(size_recv);
			data_out -> write(data_recv);
	    	}   
	}
	void notify_sending(){  
		this->sent.notify(); 


	}
	void notify_receiving(char* fileBuffer, unsigned int size){  
        	data_recv = fileBuffer;
		size_recv = size;
		recvd.notify(); 
	}


};



#endif // SC_NIC__H


