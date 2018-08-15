/*********************************************
 * Network Interface Card Model
 * Zhuoran Zhao, UT Austin, zhuoran@utexas.edu
 * Last update: July 2015
 ********************************************/

#include <string.h>

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


    	        //std::cout << "Client sends images ... ... :::::" << NodeID <<std::endl;
		cSimpleModule* wrapper = (cSimpleModule*)(OmnetWrapper);
		cContextSwitcher dummy1(wrapper); //VERY IMPORTANT
		OmnetIf_pkt* pkt = new OmnetIf_pkt();
		pkt->setFileBufferArraySize(size_send);
		for(int ii=0; ii<size_send; ii++){
				pkt->setFileBuffer(ii, ((char*)data_send)[ii]);
		}
    	        //std::cout << "Client sends images ... ... :::::" << NodeID <<std::endl;
		free(data_send);
    	        //std::cout << "Client sends images ... ... :::::" << NodeID <<std::endl;
		cMessage *startMsg = new cMessage("ServerToCli");
		startMsg->setContextPointer(pkt);
		wrapper->scheduleAt(simTime(), startMsg);  //Notify immediately
		wait(this->sent);
	  }
		
/*
		int pkt_total=100;
		int pkt_count=0;
		if(NodeID!=0 && (strcmp("nic1", this->basename())==0)){

		  for(pkt_count=0; pkt_count<pkt_total; pkt_count++){
			//size_send = size_in -> read();
			//data_send = data_in -> read();
		        std::cout << " NicSend 1... ... ... " <<std::endl;
			//Generating data 
			//Sending packets out to MAC layer

			size_send = 500;
			data_send = (char *)malloc(size_send);
			//for(int ii=0; ii<size_send; ii++)
			//	data_send[ii] = ii;


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
			//std::cout << "sc_core::wait(sent); time is: " << sc_time_stamp().value() << std::endl;
	   		//if (sent_flag==0) 
			wait(this->sent);
			//std::cout << "sc_core::after_ wait(sent); time is: " << sc_time_stamp().value() << std::endl;
			//sent_flag--;			
		  }
		}
*/
	}

	void NicRecv()
	{

		int recv_count = 0;
		int recv_count_1 = 0;
		int recv_count_2 = 0;
		while (1) {
			//std::cout << "sc_core::wait(recvd); time is: " << sc_time_stamp().value() << std::endl;
			wait(recvd);
			/*
				recv_count++;
				if(NodeID==0){
					std::cout << "recv_count ============================="<<recv_count<<std::endl;
					std::cout << "Node is " << " : "<< (data_recv[0])<<std::endl;
					if(data_recv[0]=='1') recv_count_1++;
					if(data_recv[0]=='2') recv_count_2++;
					std::cout << recv_count_1 << " : "<< recv_count_2 <<std::endl;
					for(int ii=0; ii<size_recv; ii++)
							    {
								//if((unsigned int)(data_recv[ii]) != ii) errorCount++;
								//std::cout << ii << " : "<< (data_recv[ii])<<std::endl;
							    }
		   		    //std::cout << "recved size is ... ... ... : "<< recv_count <<std::endl;
				    //std::cout << " size_recv : "<< size_recv<<std::endl;

				}
			*/


			size_out -> write(size_recv);
			data_out -> write(data_recv);
			//Data sink here
	    	}   
	}



	void notify_sending(){  
		//std::cout << "notify_sending() time is: " << sc_time_stamp().value() << std::endl;

   		//sent_flag++; 
		this->sent.notify(); 


	}
	void notify_receiving(char* fileBuffer, unsigned int size){  
		//std::cout << "notify_receiving() time is: " << sc_time_stamp().value() << std::endl;
        	data_recv = fileBuffer;
		size_recv = size;
		recvd.notify(); 
	}






};



#endif // SC_NIC__H


