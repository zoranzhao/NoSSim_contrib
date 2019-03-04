#include "work_stealing_runtime.h"
#if IPV4_TASK
const char* addr_list[MAX_EDGE_NUM] = EDGE_ADDR_LIST;
#elif IPV6_TASK/*IPV4_TASK*/
const char* addr_list[MAX_EDGE_NUM] = {"100:0:200:0:300:0:400:", "100:0:200:0:300:0:500:", "100:0:200:0:300:0:600:", "100:0:200:0:300:0:700:", "100:0:200:0:300:0:800:", "100:0:200:0:300:0:900:"};
#endif/*IPV4_TASK*/   

void partition_frame_and_perform_inference_thread_no_reuse_no_gateway(void *arg){
   device_ctxt* ctxt = (device_ctxt*)arg;
   cnn_model* model = (cnn_model*)(ctxt->model);
   blob* temp;
   uint32_t frame_num;
   bool* reuse_data_is_required;   
   for(frame_num = 0; frame_num < FRAME_NUM; frame_num ++){
      /*Wait for i/o device input*/
      /*recv_img()*/
      /*Load image and partition, fill task queues*/
      load_image_as_model_input(model, frame_num);
      partition_and_enqueue(ctxt, frame_num);
      /*Dequeue and process task*/
      while(1){
         temp = try_dequeue(ctxt->task_queue);
         if(temp == NULL) break;
         bool data_ready = false;
         os_model_context* os_model = sim_ctxt.get_os_ctxt( sc_core::sc_get_current_process_handle() );
         os_model->os_port->timeWait(1200000000000, sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle()));
         os_model->os_port->syncGlobalTime(sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle()));
         process_task_single_device(ctxt, temp, data_ready);
         free_blob(temp);
      }
   }
}

void partition_frame_and_perform_inference_thread_no_reuse(void *arg){
   device_ctxt* ctxt = (device_ctxt*)arg;
   cnn_model* model = (cnn_model*)(ctxt->model);
   blob* temp;
   uint32_t frame_num;
   bool* reuse_data_is_required;   
   for(frame_num = 0; frame_num < FRAME_NUM; frame_num ++){
      /*Wait for i/o device input*/
      /*recv_img()*/
      /*Load image and partition, fill task queues*/
      load_image_as_model_input(model, frame_num);
      partition_and_enqueue(ctxt, frame_num);
      register_client(ctxt);
      /*Dequeue and process task*/
      while(1){
         temp = try_dequeue(ctxt->task_queue);
         if(temp == NULL) break;
         bool data_ready = false;
         os_model_context* os_model = sim_ctxt.get_os_ctxt( sc_core::sc_get_current_process_handle() );
         os_model->os_port->timeWait(1200000000000, sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle()));
         os_model->os_port->syncGlobalTime(sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle()));
         process_task_single_device(ctxt, temp, data_ready);
         free_blob(temp);
      }
      cancel_client(ctxt);
   }
}

void steal_partition_and_perform_inference_thread_no_reuse_no_gateway(void *arg){
   device_ctxt* ctxt = (device_ctxt*)arg;
   service_conn* conn;
   blob* temp;
   while(1){
      conn = connect_service(TCP, (const char *)addr_list[0], WORK_STEAL_PORT);
      //conn = connect_service(TCP, "192.168.4.1", WORK_STEAL_PORT);
      //int flags =1;
      //lwip_setsockopt(conn->sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags));
      send_request("steal_client", 20, conn);
      //printf("Stealing request sent, time is %f\n", sc_core::sc_time_stamp().to_seconds());
      temp = recv_data(conn);
      close_service_connection(conn);
      if(temp->id == -1){
         free_blob(temp);
         sys_sleep(100);
         continue;
      }
      bool data_ready = true;
      printf("Get task remotely, frame %d, task is %d, time is %f\n", get_blob_frame_seq(temp), 
             get_blob_task_id(temp), sc_core::sc_time_stamp().to_seconds());
      process_task_single_device(ctxt, temp, data_ready);
      os_model_context* os_model = sim_ctxt.get_os_ctxt( sc_core::sc_get_current_process_handle() );
      os_model->os_port->timeWait(3000000000000, sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle()));
      os_model->os_port->syncGlobalTime(sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle()));
      printf("Process task remotely, frame %d, task is %d, size is %d, time is %f\n", 
              get_blob_frame_seq(temp), get_blob_task_id(temp), temp->size, sc_core::sc_time_stamp().to_seconds());
      free_blob(temp);
   }
}

void steal_partition_and_perform_inference_thread_no_reuse(void *arg){
   device_ctxt* ctxt = (device_ctxt*)arg;
   service_conn* conn;
   blob* temp;
   while(1){
      conn = connect_service(TCP, ctxt->gateway_local_addr, WORK_STEAL_PORT);
      send_request("steal_gateway", 20, conn);
      temp = recv_data(conn);
      close_service_connection(conn);
      if(temp->id == -1){
         free_blob(temp);
         sys_sleep(100);
         continue;
      }

      conn = connect_service(TCP, (const char *)temp->data, WORK_STEAL_PORT);
      //lwip_setsockopt(conn->sockfd, IPPROTO_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags));
      send_request("steal_client", 20, conn);
      //printf("Stealing request sent, time is %f\n", sc_core::sc_time_stamp().to_seconds());
      free_blob(temp);
      temp = recv_data(conn);
      close_service_connection(conn);
      if(temp->id == -1){
         free_blob(temp);
         sys_sleep(100);
         continue;
      }
      bool data_ready = true;
      printf("Get task remotely, frame %d, task is %d, time is %f\n", get_blob_frame_seq(temp), 
             get_blob_task_id(temp), sc_core::sc_time_stamp().to_seconds());
      process_task_single_device(ctxt, temp, data_ready);
      os_model_context* os_model = sim_ctxt.get_os_ctxt( sc_core::sc_get_current_process_handle() );
      os_model->os_port->timeWait(3000000000000, sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle()));
      os_model->os_port->syncGlobalTime(sim_ctxt.get_task_id(sc_core::sc_get_current_process_handle()));
      printf("Process task remotely, frame %d, task is %d, size is %d, time is %f\n", 
              get_blob_frame_seq(temp), get_blob_task_id(temp), temp->size, sc_core::sc_time_stamp().to_seconds());
      free_blob(temp);
   }
}

void test_deepthings_stealer_edge(uint32_t edge_id){
   uint32_t N = 5;
   uint32_t M = 5;
   uint32_t fused_layers = 16;

   char network[30] = "models/yolo.cfg";
   char weights[30] = "models/yolo.weights";

   device_ctxt* ctxt = deepthings_edge_init(N, M, fused_layers, network, weights, edge_id);

   sys_thread_t t1 = sys_thread_new("steal_partition_and_perform_inference_thread", 
                                    steal_partition_and_perform_inference_thread, ctxt, 101, 0);
   sys_thread_t t2 = sys_thread_new("send_result_thread", send_result_thread, ctxt, 102, 0);

   //sys_thread_join(t1);
   //sys_thread_join(t2);
}

void test_deepthings_victim_edge(uint32_t edge_id){//edge_id == 0;
   uint32_t N = 5;
   uint32_t M = 5;
   uint32_t fused_layers = 16;

   char network[30] = "models/yolo.cfg";
   char weights[30] = "models/yolo.weights";
   //Load configuration here
   device_ctxt* ctxt = deepthings_edge_init(N, M, fused_layers, network, weights, edge_id);

   sys_thread_t t1 = sys_thread_new("partition_frame_and_perform_inference_thread", 
                                    partition_frame_and_perform_inference_thread, ctxt, 102, 0);
   sys_thread_t t2 = sys_thread_new("send_result_thread", send_result_thread, ctxt, 101, 0);
   sys_thread_t t3 = sys_thread_new("deepthings_serve_stealing_thread", deepthings_serve_stealing_thread, ctxt, 101, 0);

   //sys_thread_join(t1);
   //sys_thread_join(t2);
   //sys_thread_join(t3);
}

void* test_deepthings_result_gateway(void* srv_conn, void* arg){
   device_ctxt* ctxt = (device_ctxt*)arg;
   service_conn *conn = (service_conn *)srv_conn;
   int32_t cli_id;
   int32_t frame_seq;
   blob* temp = recv_data(conn);

   cli_id = get_blob_cli_id(temp);
   frame_seq = get_blob_frame_seq(temp);

/*DEBUG*/
   char ip_addr[100];
   get_dest_ip_string(ip_addr, conn);
   std::cout << "Getting client " << cli_id << "'s result of partition " << get_blob_task_id(temp) << ",  frame " << frame_seq 
             << " from: " << ip_addr << ", at time: " << sc_core::sc_time_stamp().to_seconds()  << std::endl;
/*DEBUG*/

   enqueue(ctxt->results_pool[cli_id], temp);
   free_blob(temp);
   ctxt->results_counter[cli_id]++;
   if(ctxt->results_counter[cli_id] == ctxt->batch_size){
      temp = new_empty_blob(cli_id);
      enqueue(ctxt->ready_pool, temp);
      free_blob(temp);
      ctxt->results_counter[cli_id] = 0;
   }

   return NULL;
}

void test_deepthings_collect_result_thread(void *arg){
   const char* request_types[]={"result_gateway"};
   void* (*handlers[])(void*, void*) = {deepthings_result_gateway};
   int result_service = service_init(RESULT_COLLECT_PORT, TCP);
   start_service(result_service, TCP, request_types, 1, handlers, arg);
   close_service(result_service);
}

void test_deepthings_merge_result_thread(void *arg){
   cnn_model* model = (cnn_model*)(((device_ctxt*)(arg))->model);
   blob* temp;
   int32_t cli_id = 0;
   int32_t frame_seq = 0;
   while(1){
      temp = dequeue_and_merge((device_ctxt*)arg);
      printf("dequeue_and_merge, Client %d, frame sequence number %d, all partitions are merged in deepthings_merge_result_thread at time %f\n", 
             cli_id, frame_seq,  sc_core::sc_time_stamp().to_seconds());
      cli_id = get_blob_cli_id(temp);
      frame_seq = get_blob_frame_seq(temp);
      printf("Client %d, frame sequence number %d, all partitions are merged in deepthings_merge_result_thread at time %f\n", 
             cli_id, frame_seq,  sc_core::sc_time_stamp().to_seconds());
      float* fused_output = (float*)(temp->data);
      image_holder img = load_image_as_model_input(model, get_blob_frame_seq(temp));
      set_model_input(model, fused_output);
      forward_all(model, model->ftp_para->fused_layers);   
      draw_object_boxes(model, get_blob_frame_seq(temp));
      free_image_holder(model, img);
      free_blob(temp);
      printf("Client %d, frame sequence number %d, finish processing at time %f\n", cli_id, frame_seq, sc_core::sc_time_stamp().to_seconds());
      if(frame_seq == 3) {
         os_model_context* os_model = sim_ctxt.get_os_ctxt( sc_core::sc_get_current_process_handle() );
         os_model -> ctrl_out1->write(0);
      }
   }
   
}

void test_deepthings_gateway(uint32_t total_edge_number){
   uint32_t N = 5;
   uint32_t M = 5;
   uint32_t fused_layers = 16;

   char network[30] = "models/yolo.cfg";
   char weights[30] = "models/yolo.weights";

   device_ctxt* ctxt = deepthings_gateway_init(N, M, fused_layers, network, weights, total_edge_number, addr_list);
   sys_thread_t t1 = sys_thread_new("deepthings_collect_result_thread", deepthings_collect_result_thread, ctxt, 102, 0);
   sys_thread_t t2 = sys_thread_new("deepthings_merge_result_thread", test_deepthings_merge_result_thread, ctxt, 102, 0);
   sys_thread_t t3 = sys_thread_new("deepthings_work_stealing_thread", deepthings_work_stealing_thread, ctxt, 101, 0);
   //sys_thread_t t3 = sys_thread_new("work_stealing_thread", work_stealing_thread, ctxt, 101, 0);

   //sys_thread_join(t1);
   //sys_thread_join(t2);

}























