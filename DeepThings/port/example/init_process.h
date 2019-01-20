#include "HCSim.h"
#include "hcsim_port.h"
#include "OmnetIf_pkt.h"
#include "test_utils.h"

#ifndef INIT_PROCESS_H
#define INIT_PROCESS_H
extern const char* addr_list[MAX_EDGE_NUM];

void test(void* arg){
   /*Initialize the data structure and network model*/
   uint32_t total_cli_num = 1;
   uint32_t this_cli_id = 0;

   uint32_t partitions_h = 3;
   uint32_t partitions_w = 3;
   uint32_t fused_layers = 8;

   char network_file[30] = "models/yolo.cfg";
   char weight_file[30] = "models/yolo.weights";

   device_ctxt* client_ctxt = deepthings_edge_init(partitions_h, partitions_w, fused_layers, network_file, weight_file, this_cli_id);
   device_ctxt* gateway_ctxt = deepthings_gateway_init(partitions_h, partitions_w, fused_layers, network_file, weight_file, total_cli_num, addr_list);

   //partition_frame_and_perform_inference_thread_single_device(client_ctxt);
   //transfer_data_with_number(client_ctxt, gateway_ctxt, FRAME_NUM*partitions_h*partitions_h);
   //deepthings_merge_result_thread_single_device(gateway_ctxt);
   

   sys_thread_t t2 = sys_thread_new("deepthings_merge_result_thread_single_device", deepthings_merge_result_thread_single_device, gateway_ctxt, 49, 0);
   sys_thread_t t1 = sys_thread_new("partition_frame_and_perform_inference_thread_single_device",
                                     partition_frame_and_perform_inference_thread_single_device, client_ctxt, 49, 0);
   transfer_data(client_ctxt, gateway_ctxt);
   sys_thread_join(t1);
   sys_thread_join(t2);




}

class IntrDriven_Task :public sc_core::sc_module,virtual public HCSim::OS_TASK_INIT{
public:
   sc_core::sc_vector< sc_core::sc_port< sys_call_recv_if > > recv_port;
   sc_core::sc_vector< sc_core::sc_port< sys_call_send_if > > send_port;
   sc_core::sc_port< HCSim::OSAPI > os_port;
   app_context* app_ctxt;
   os_model_context* os_ctxt;

   SC_HAS_PROCESS(IntrDriven_Task);
   IntrDriven_Task(const sc_core::sc_module_name name, 
            sc_dt::uint64 exe_cost, sc_dt::uint64 period, 
            unsigned int priority, int id, uint8_t init_core,
            sc_dt::uint64 end_sim_time, int NodeID)
            :sc_core::sc_module(name)
   {
      this->exe_cost = exe_cost;
      this->period = period;
      this->priority = priority;
      this->id = id;
      this->end_sim_time = end_sim_time;
      this->NodeID = NodeID;
      this->init_core = 1;
      recv_port.init(2);
      send_port.init(2);
      os_ctxt = new os_model_context(NodeID, this->recv_port, this->send_port, this->os_port);
      app_ctxt = new app_context();
      SC_THREAD(run_jobs);
   }
    
   ~IntrDriven_Task() {
       delete os_ctxt;
       delete app_ctxt;
   }

   void OSTaskCreate(void){
      os_task_id = os_port->taskCreate(sc_core::sc_gen_unique_name("intrdriven_task"), 
                                HCSim::OS_RT_APERIODIC, priority, period, exe_cost, 
                                HCSim::DEFAULT_TS, HCSim::ALL_CORES, init_core);
   }

private:
   int id;
   uint8_t init_core;
   sc_dt::uint64 exe_cost;
   sc_dt::uint64 period;
   unsigned int priority;
   sc_dt::uint64 end_sim_time;
   int os_task_id;
   int NodeID;

   void run_jobs(void){
      os_port->taskActivate(os_task_id);
      os_port->timeWait(0, os_task_id);
      os_port->syncGlobalTime(os_task_id);
      sim_ctxt.register_task(os_ctxt, app_ctxt, os_task_id, sc_core::sc_get_current_process_handle());
       
      if(NodeID==0)
         //sys_thread_new("test", test, NULL, 49, 0);
	 test(NULL);
      os_port->taskTerminate(os_task_id);
   }
};



#endif //INIT_PROCESS_H
 

