#include "HCSim.h"
#include "hcsim_port.h"
#include "OmnetIf_pkt.h"

#ifndef SC_TASK_MODEL__H
#define SC_TASK_MODEL__H

void test(void* arg){

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
      sys_thread_new("test", test, NULL, 49, 0);
      os_port->taskTerminate(os_task_id);
   }
};



#endif // SC_TASK_MODEL__H 

