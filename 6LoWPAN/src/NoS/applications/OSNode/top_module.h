/*********************************************
 * Mix Task Set Example
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: July 2013
 ********************************************/


#include "Nic.h"
#include "AppTasks.h"


#ifndef TOP_MODULE__H
#define TOP_MODULE__H

class artificial_example
    :sc_core::sc_module
{
 public:

    int NodeID; 

    Nic* NetworkInterfaceCard1;
    //Nic* NetworkInterfaceCard2;



    artificial_example(const sc_core::sc_module_name name, int NodeID)
        :sc_core::sc_module(name)
    {


        this->NodeID = NodeID;
	NetworkInterfaceCard1 =  new Nic("nic1", NodeID); 
	//NetworkInterfaceCard2 =  new Nic("nic2", NodeID); 

        intr_gen_1 = new IntrDriven_Task("intr_gen_1", NodeID);  

	intr_gen_1 -> size_out(size_send_fifo1); 
	NetworkInterfaceCard1 -> size_in(size_send_fifo1); 
	intr_gen_1 -> size_in(size_recv_fifo1); 
	NetworkInterfaceCard1 -> size_out(size_recv_fifo1); 

	intr_gen_1 -> data_out(data_send_fifo1); 
	NetworkInterfaceCard1 -> data_in(data_send_fifo1); 
	intr_gen_1 -> data_in(data_recv_fifo1); 
	NetworkInterfaceCard1 -> data_out(data_recv_fifo1); 


           
    }    
    ~artificial_example(){
	delete NetworkInterfaceCard1;
	//delete NetworkInterfaceCard2;
    }

 private:

    IntrDriven_Task* intr_gen_1;

    sc_fifo<char* > data_send_fifo1;
    sc_fifo<int > size_send_fifo1;
    sc_fifo<char* > data_recv_fifo1;
    sc_fifo<int > size_recv_fifo1;


    //sc_fifo<char* > data_send_fifo2;
    //sc_fifo<int > size_send_fifo2;
    //sc_fifo<char* > data_recv_fifo2;
    //sc_fifo<int > size_recv_fifo2;


};    


#endif

