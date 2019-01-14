#include "hcsim_port.h"

#ifndef SYS_ARCH_TIMEOUT
#define SYS_ARCH_TIMEOUT 1000000
#endif

#ifndef ERR_OK
#define ERR_OK 0
#endif

#ifndef ERR_MEM
#define ERR_MEM -1
#endif
/*Global table for simulation context lookup*/
simulation_context sim_ctxt;
static sc_dt::uint64 starttime;
static struct sys_thread *threads = NULL;
static sc_core::sc_mutex threads_mutex;

struct sys_thread {
  struct sys_thread *next;
  sc_core::sc_process_handle sc_thread;
};

/*Function wrapper for OS task model*/
typedef void (*os_wrapper_fn)(void *ctxt, thread_fn function, void* arg, int task_id);
void wrapper(void *ctxt, thread_fn function, void *arg, int task_id){
   os_model_context* os_model = sim_ctxt.get_os_ctxt( sc_core::sc_get_current_process_handle() );
   sim_ctxt.register_task(os_model, ctxt, task_id, sc_core::sc_get_current_process_handle());
   os_model->os_port->taskActivate(taskID);
   function(arg);
   os_model->os_port->taskTerminate(taskID);
}

sys_thread_t sys_thread_new(const char *name, thread_fn function, void *arg, int priority, int core){
   void* ctxt = sim_ctxt.get_app_ctxt( sc_core::sc_get_current_process_handle() );
   os_model_context* os_model = sim_ctxt.get_os_ctxt( sc_core::sc_get_current_process_handle() );

   int child_id  = ( os_model->os_port->taskCreate(
				sc_core::sc_gen_unique_name("child_task"), 
				HCSim::OS_RT_APERIODIC, priority, 0, 0, 
				HCSim::DEFAULT_TS, HCSim::ALL_CORES, core);

   os_model-> os_port -> dynamicStart(init_core);

   OS_wrapper_fn OS_fn = NULL;
   OS_fn = &wrapper;
   sc_core::sc_process_handle th_handle; = sc_core::sc_spawn(     
                                 sc_bind(  
                                         OS_fn,
                                         ctxt, function, arg, child_id 
                                         )         
                                ); 
   struct sys_thread *thread = new sys_thread;
   if (thread != NULL) {
      threads_mutex.lock();
      thread->next = ((LwipCntxt*)ctxt)->threads;
      thread->sc_thread = th_handle;
      threads = thread;
      threads_mutex.unlock();
   }
   return thread;
}

void sys_thread_join(sys_thread_t thread){

}

#define GLOBAL_SEMS 200
struct sys_sem {
   bool free;
   int id;
   unsigned int c;
   sc_core::sc_event cond;
   sc_core::sc_mutex mutex;
   void * ctxt;
   int blocking_task_id;
   int blocked_task_id;
} sems[GLOBAL_SEMS];

static struct sys_sem * sys_sem_new_internal(uint8_t count)
{
   int i = 0;
   for(i = 0; i<GLOBAL_SEMS; i++){
      if(sems[i].free == 1) break;
   }
  struct sys_sem *sem;
  sem = sems + i;
  sem->id = i;
  sem->blocking_task_id = -1;
  sem->blocked_task_id = -1; 
  sem->free=0;
  sem->c = count;
  return sem;
}

int32_t sys_sem_new(struct sys_sem **sem, uint8_t count)
{
   *sem = sys_sem_new_internal(count);
   if (*sem == NULL) {
      return ERR_MEM;
   }
   return ERR_OK;
}


void
sys_sem_signal(struct sys_sem **s)
{
   struct sys_sem *sem;
   sem = *s;
   int task_id = sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle());
   sem->blocking_task_id = task_id;
   sem->c++;
   if (sem->c > 1) {
      sem->c = 1;
   }
   sem->cond.notify(sc_core::SC_ZERO_TIME);
}

uint32_t
sys_arch_sem_wait(struct sys_sem **s, uint32_t timeout){
   sc_dt::uint64 start_time;
   u32_t time_needed = 0;
   struct sys_sem *sem;
   sem = *s;
   int task_id = sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle());
   os_model_context* os_model = sim_ctxt.get_os_ctxt( sc_core::sc_get_current_process_handle() );
   sem->blocked_task_id = task_id;

   while(sem->c <= 0){
      os_model->os_port->preWait(task_id, sem->blocking_task_id);	
      if(sem->c > 0){
         os_model->os_port->postWait(task_id);
         break;
      }
      if (timeout > 0) {
         start_time = sc_core::sc_time_stamp().value();
         sc_core::wait(timeout, sc_core::SC_MS, sem->cond);
         start_time = (sc_core::sc_time_stamp().value() - start_time);
         time_needed = (u32_t)(start_time/1000000000);
         if((time_needed == timeout) && (sem->c <= 0)){
            os_model->os_port->postWait(task_id);
	    return SYS_ARCH_TIMEOUT;
         }
      }else{
	   sc_core::wait(sem->cond);	
      }
      os_model->os_port->postWait(task_id);
   }
   sem->c--;
   return time_needed;
}

static void sys_sem_free_internal(struct sys_sem *sem)
{
   sem->free=1;
}

void sys_sem_free(struct sys_sem **sem)
{
   if (sem != NULL) {
    sys_sem_free_internal(*sem);
   }
}

void sys_sleep(uint32_t milliseconds){

}

uint32_t sys_now(void){
   /*Million seconds*/
   sc_dt::uint64 msec;
   msec = (sc_core::sc_time_stamp().value()/1000000000) - starttime;
   return (uint32_t)msec;
}

double sys_now_in_sec(void){
   /*Seconds*/
   return ((double)sys_now())/1000;
}
/*Intialization function used*/
void sys_init(void){
   starttime = (sc_dt::uint64) (sc_core::sc_time_stamp().value()/1000000000);
   for(i = 0; i<GLOBAL_SEMS; i++){
      sems[i].free = 1;
   }
}

