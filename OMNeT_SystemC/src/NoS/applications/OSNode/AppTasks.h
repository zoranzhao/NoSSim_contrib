/*********************************************                                                       
 * VisionGraph Tasks Based on Interrupt-Driven Task Model                                                                     
 * Zhuoran Zhao, UT Austin, zhuoran@utexas.edu                                                    
 * Last update: July 2016                                                                            
 ********************************************/
#include <omnetpp.h>
#include <systemc>

#ifndef SC_VISION_TASK__H
#define SC_VISION_TASK__H

#define CLI_NUM_MAX 10
#define CLI_NUM 6
#define IMG_NUM 4
extern unsigned long cli_commu[CLI_NUM_MAX];
extern int schd[CLI_NUM_MAX];
extern double CliEnergy[10];

void cli_wait(int NodeID);
void srv_wait(int NodeID);
void srv_send(int NodeID, int cliID);
void cli_send(int NodeID, int cliID);
void srv_send_dat(int NodeID,  int from_cliID, int cliID,  int image_count);




class IntrDriven_Task :public sc_core::sc_module
{
 public:


    sc_core::sc_port< sc_core::sc_fifo_out_if<int> > size_out;
    sc_core::sc_port< sc_core::sc_fifo_in_if<int> > size_in;

    sc_core::sc_port< sc_core::sc_fifo_out_if<char*> > data_out;
    sc_core::sc_port< sc_core::sc_fifo_in_if<char*> > data_in;


    SC_HAS_PROCESS(IntrDriven_Task);
    IntrDriven_Task(const sc_core::sc_module_name name, int NodeID)
    :sc_core::sc_module(name)
    {
	this->NodeID = NodeID;
        SC_THREAD(recv_imgs);
        SC_THREAD(send_imgs);

    }
    
    ~IntrDriven_Task() {}







 private:
	int NodeID;
	char* data_send;
	int size_send;
	char* data_recv;
	int size_recv;

	void recv_imgs()
	{
	  int count = 0;
	  while(1){
		size_recv = size_in -> read();
		data_recv = data_in -> read();
		count ++;
	        std::cout << "Recvd at Node: "<< NodeID << ", size is: " 
			<< size_recv <<", total number is: "
			<< count << std::endl;
	  }
	}
	void send_imgs()
	{
	  std::cout << "Sending at Node: "<< NodeID << std::endl;
          int total = 1000;
	  if (NodeID==0) total = 6000;

	  for(int i = 0; i < total; i++){//Client running times 
		size_send = 20;
		data_send = (char *)malloc(size_send);
	        printf("The number is %d\n", NodeID);
		if(NodeID==1)
			data_send[0]='1';
		if(NodeID==2)
			data_send[0]='2';
		data_send[1]='9';
		data_send[2]='9';
		data_send[3]='9';
		data_send[4]='9';
		data_send[5]='9';
		data_send[6]='9';
		data_send[7]='9';
		data_send[8]='9';
		data_send[9]='9';
		size_out -> write(size_send);
		data_out -> write(data_send);

	  }
	}


	void run_jobs(void)
	{
	   	recv_imgs();
		if(NodeID!=0) send_imgs();
	}



};

#endif // SC_VISION_TASK__H 
