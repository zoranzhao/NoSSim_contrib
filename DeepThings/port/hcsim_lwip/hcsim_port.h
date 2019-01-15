#ifndef HCSIM_PORT_H
#define HCSIM_PORT_H
#include <systemc>
#include "HCSim.h"
#include <cstdint>
#define MAX_CORE_NUM 2

class sys_call_recv_if: virtual public sc_core::sc_interface{
public:
   virtual int get_node(int os_task_id) = 0;
   virtual int get_weight(int os_task_id) = 0;
   virtual int get_size(int os_task_id) = 0;
   virtual bool get_data(unsigned size, char* data, int os_task_id) = 0;
};

class sys_call_send_if: virtual public sc_core::sc_interface{
public:
   virtual void set_node(int NodeID, int os_task_id) = 0;
   virtual void set_weight(int weight, int os_task_id) = 0;
   virtual void set_size(int size, int os_task_id) = 0;
   virtual void set_data(unsigned size, char* data, int os_task_id) = 0;
};

class os_model_context{
public:
   int node_id;

   sc_core::sc_port<sys_call_recv_if> recv_port[MAX_CORE_NUM];
   sc_core::sc_port<sys_call_send_if> send_port[MAX_CORE_NUM]; 
   sc_core::sc_port< HCSim::OSAPI > os_port;

   int cli_id;
   int device_type;
   int core_num;
};

typedef struct sc_process_handler_context{
   os_model_context* os_ctxt;  
   void* app_ctxt;
   int task_id;  
} handler_context;

class simulation_context{
   std::vector< sc_core::sc_process_handle> handler_list;  
   std::vector<handler_context> handler_context_list;
   
public:
   void register_task(os_model_context* os_ctxt, void* app_ctxt, int task_id, sc_core::sc_process_handle handler){
      handler_context ctxt;
      ctxt.os_ctxt = os_ctxt;  
      ctxt.app_ctxt = app_ctxt;
      ctxt.task_id = task_id;
      handler_list.push_back(handler);
      handler_context_list.push_back(ctxt);
   }

   handler_context get_handler_context(sc_core::sc_process_handle handler){
      auto key = handler_list.begin();
      auto item =  handler_context_list.begin();
      for(; key!=handler_list.end() && item!= handler_context_list.end(); key++, item++){
         if(*key == handler) break;
      }
      return *item;
   }

   os_model_context* get_os_ctxt(sc_core::sc_process_handle handler){
      handler_context ctxt = get_handler_context(handler);
      return ctxt.os_ctxt;
   } 

   void* get_app_ctxt(sc_core::sc_process_handle handler){
      handler_context ctxt = get_handler_context(handler);
      return ctxt.app_ctxt;
   } 
	
   int get_task_id(sc_core::sc_process_handle handler){
      handler_context ctxt = get_handler_context(handler);
      return ctxt.task_id;
   } 
};


typedef void (*thread_fn)(void *arg);
struct sys_thread;
typedef struct sys_thread* sys_thread_t;
/*multithreading APIs*/
sys_thread_t sys_thread_new(const char *name, thread_fn function, void *arg, int stacksize, int prio);
void sys_thread_join(sys_thread_t thread);

/*Semaphore APIs*/
struct sys_sem;
typedef struct sys_sem* sys_sem_t;
int32_t sys_sem_new(struct sys_sem **sem, uint8_t count);
void sys_sem_signal(struct sys_sem **s);
uint32_t sys_arch_sem_wait(struct sys_sem **s, uint32_t timeout);
void sys_sem_free(struct sys_sem **sem);
void sys_sleep();
uint32_t sys_now(void);
double sys_now_in_sec(void);
#endif