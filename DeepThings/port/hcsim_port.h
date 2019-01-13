#ifndef HCSIM_PORT_H
#define HCSIM_PORT_H
#include "HCSim.h"
#include <cstdint>
#include <unordered_map>
#define MAX_CORE_NUM 2

class lwip_recv_if: virtual public sc_core::sc_interface{
public:
   virtual int GetNode(int os_task_id) = 0;
   virtual int GetWeight(int os_task_id) = 0;
   virtual int GetSize(int os_task_id) = 0;
   virtual bool GetData(unsigned size, char* data, int os_task_id) = 0;
};

class lwip_send_if: virtual public sc_core::sc_interface{
public:
   virtual void SetNode(int NodeID, int os_task_id) = 0;
   virtual void SetWeight(int weight, int os_task_id) = 0;
   virtual void SetSize(int size, int os_task_id) = 0;
   virtual void SetData(unsigned size, char* data, int os_task_id) = 0;
};


class os_model_context{
public:
   int node_id;

   sc_core::sc_port<lwip_recv_if> recv_port[MAX_CORE_NUM];
   sc_core::sc_port<lwip_send_if> send_port[MAX_CORE_NUM]; 
   sc_core::sc_port< HCSim::OSAPI > os_port;

   int cli_id;
   int device_type;
   int core_num;
};


class simulation_context{
public:
   std::vector< sc_core::sc_process_handle> handler_list;  
   std::vector< int > os_task_id_list;  
   std::vector<os_model_context* > os_ctxt_list;  
   std::vector<void* > app_ctxt_list;


   void register_task(os_model_context* os_ctxt, void* app_ctxt, int os_task_id, sc_core::sc_process_handle handler){
      app_ctxt_list.push_back(app_ctxt);
      os_ctxt_list.push_back(os_ctxt);
      os_task_id_list.push_back(os_task_id);
      handler_list.push_back(handler);
   }


   void* get_app_ctxt(sc_core::sc_process_handle handler){
      std::vector< sc_core::sc_process_handle >::iterator handlerIt = taskHandlerList.begin();
      std::vector< void* >::iterator idIt = lwipList.begin();
      for(; (handlerIt!=taskHandlerList.end() && idIt!= lwipList.end() ) ;handlerIt++, idIt++){
			if(*handlerIt == taskHandler)
				return *idIt;	
      }

      return NULL;
   } 
	

	int getTaskID(sc_core::sc_process_handle taskHandler){
		std::vector< sc_core::sc_process_handle >::iterator handlerIt = taskHandlerList.begin();
		std::vector< int >::iterator idIt = taskIDList.begin();
		for(; (handlerIt!=taskHandlerList.end() && idIt!=taskIDList.end() ) ;handlerIt++, idIt++){
			if(*handlerIt == taskHandler)
				return *idIt;	
		}
		return -1;
	} 


	OSModelCtxt* getTaskCtxt(sc_core::sc_process_handle taskHandler){
		std::vector< sc_core::sc_process_handle >::iterator handlerIt = taskHandlerList.begin();
		std::vector< OSModelCtxt* >::iterator idIt = ctxtIDList.begin();
		for(; (handlerIt!=taskHandlerList.end() && idIt!=ctxtIDList.end() ) ;handlerIt++, idIt++)
		{
			if(*handlerIt == taskHandler)
				return *idIt;	
		}
		//printf("Error: no task ctxt existing in the global recorder\n");
		return NULL;
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
