#include "application.h"
#include "data_blob.h"
/*

typedef struct results_data {
   uint32_t rpeaks;
   uint32_t normal;
   uint32_t abnormal;
} result_data;

typedef struct signal_data {
   uint32_t size;
   char* data;
   struct signal_data* next;
} sig_data;

typedef struct offloading_data {
   uint32_t rpeaks;
   uint32_t item_number;
   sig_data* head;
   sig_data* tail;
} offload_data;
offload_data* make_offload_data();
void free_offload_data(offload_data* data);
sig_data* get_offload_data(offload_data* data, uint32_t pos);
void add_offload_data(offload_data* data, char* blob, uint32_t blob_size);
*/

#define FRAME_NUM 20

#define ECG_EDGE_PORT 11117
#define ECG_GATEWAY_PORT 11119
#define UDP_TRANS_SIZE 100

static inline void read_from_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, struct sockaddr *from, socklen_t *fromlen){
   uint32_t bytes_read = 0;
   int32_t n = 0;
   while (bytes_read < bytes_length){
      if(proto == TCP){
         n = lwip_recv(sock, buffer + bytes_read, bytes_length - bytes_read, 0);
         if( n < 0 ) printf("ERROR reading socket\n");
      }else if(proto == UDP){
         if((bytes_length - bytes_read) < UDP_TRANS_SIZE) { n = bytes_length - bytes_read; }
         else { n = UDP_TRANS_SIZE; }
         if( lwip_recvfrom(sock, buffer + bytes_read, n, 0, from, fromlen) < 0) printf("ERROR reading socket\n");
      }else{printf("Protocol is not supported\n");}
      bytes_read += n;
   }
}
static inline void write_to_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, const struct sockaddr *to, socklen_t tolen){
   uint32_t bytes_written = 0;
   int32_t n = 0;
   while (bytes_written < bytes_length) {
      if(proto == TCP){
         n = lwip_send(sock, buffer + bytes_written, bytes_length - bytes_written, 0);
         if( n < 0 ) printf("ERROR writing socket\n");
      }else if(proto == UDP){
         if((bytes_length - bytes_written) < UDP_TRANS_SIZE) { n = bytes_length - bytes_written; }
         else { n = UDP_TRANS_SIZE; }
         if(lwip_sendto(sock, buffer + bytes_written, n, 0, to, tolen)< 0) 
	   printf("ERROR writing socket\n");
      }else{printf("Protocol is not supported\n"); return;}
      bytes_written += n;
   }
}

//TODO implemente UDP version
void send_offload_data(service_conn* conn, offload_data* data){
   uint32_t rpeaks = data->rpeaks;
   uint32_t item_number = data->item_number;
   sig_data* sig = data->head;
   write_to_sock(conn->sockfd, TCP, (uint8_t*)(&rpeaks), sizeof(uint32_t), NULL, 0);
   write_to_sock(conn->sockfd, TCP, (uint8_t*)(&item_number), sizeof(uint32_t), NULL, 0);

   uint32_t i = 0;
   for(i = 0; i < item_number; i++){
      uint32_t size = sig->size;
      write_to_sock(conn->sockfd, TCP, (uint8_t*)(&size), sizeof(uint32_t), NULL, 0);
      write_to_sock(conn->sockfd, TCP, (uint8_t*)(sig->data), sig->size, NULL, 0);
      sig = sig->next;
   }
}

offload_data* recv_offload_data(service_conn* conn){
   offload_data* data = make_offload_data();  

   uint32_t rpeaks;
   uint32_t item_number;

   read_from_sock(conn->sockfd, TCP, (uint8_t*)&rpeaks, sizeof(uint32_t), NULL, 0);
   read_from_sock(conn->sockfd, TCP, (uint8_t*)&item_number, sizeof(uint32_t), NULL, 0);
   data->rpeaks = rpeaks;

   uint32_t i = 0;
   for(i = 0; i < item_number; i++){
      uint32_t bytes_length;
      read_from_sock(conn->sockfd, TCP, (uint8_t*)&bytes_length, sizeof(bytes_length), NULL, 0);
      char* buffer = (char*)malloc(bytes_length);
      read_from_sock(conn->sockfd, TCP, (uint8_t*)(buffer), bytes_length, NULL, 0);
      add_offload_data(data, buffer, bytes_length);
      free(buffer);
   }
   assert(item_number == data->item_number);
   return data;
 
}

void send_result(service_conn* conn, result_data* data){
   uint32_t rpeaks = data->rpeaks;
   uint32_t normal = data->normal;
   uint32_t abnormal = data->abnormal;

   write_to_sock(conn->sockfd, TCP, (uint8_t*)(&rpeaks), sizeof(uint32_t), NULL, 0);
   write_to_sock(conn->sockfd, TCP, (uint8_t*)(&normal), sizeof(uint32_t), NULL, 0);
   write_to_sock(conn->sockfd, TCP, (uint8_t*)(&abnormal), sizeof(uint32_t), NULL, 0);
}

result_data* recv_result(service_conn* conn){
   uint32_t rpeaks;
   uint32_t normal;
   uint32_t abnormal;

   read_from_sock(conn->sockfd, TCP, (uint8_t*)(&rpeaks), sizeof(uint32_t), NULL, 0);
   read_from_sock(conn->sockfd, TCP, (uint8_t*)(&normal), sizeof(uint32_t), NULL, 0);
   read_from_sock(conn->sockfd, TCP, (uint8_t*)(&abnormal), sizeof(uint32_t), NULL, 0);
   result_data* data = (result_data*)malloc(sizeof(result_data));
   data->rpeaks = rpeaks;
   data->normal = normal;
   data->abnormal = abnormal;
   return data;
}

void ecg_edge_process(void* arg){
   ecg_context* ctxt = (ecg_context*)arg; 
   FILE *fresult;
   char result_file[50];
   sprintf(result_file, "./out_%d.log", ctxt->node_id );
   fresult = fopen(result_file,"w"); 
   fclose(fresult);

   int seq_num = 0;
   for(seq_num = 0; seq_num <FRAME_NUM; seq_num++){
      //printf("======ecg_edge_process ready for frame: %d at time %f\n", seq_num, sc_core::sc_time_stamp().to_seconds());
      if(ctxt->offload_level[ctxt->node_id] == 0){	
         offload_data* tmp = heartbeat_detection(get_signal_block(seq_num, FRAME_NUM));
         tmp = heartbeat_segmentation(tmp);
         tmp = feature_extraction(tmp);
         result_data* results = classification(tmp, result_file);
         //printf("rpeaks: %d\n", results->rpeaks);
         //printf("\t\t\tnormal: %d\n\t\t\tabnormal: %d\n", results->normal, results->abnormal);
         blob* data_to_be_sent = new_blob_and_copy_data(ctxt->node_id, sizeof(result_data*), (uint8_t*)(&results));
         enqueue(ctxt->ready, data_to_be_sent);
         free_blob(data_to_be_sent);
      }
      else if(ctxt->offload_level[ctxt->node_id] == 1){		
         offload_data* tmp = heartbeat_detection(get_signal_block(seq_num, FRAME_NUM));
         tmp = heartbeat_segmentation(tmp);
         tmp = feature_extraction(tmp);
         blob* data_to_be_sent = new_blob_and_copy_data(ctxt->node_id, sizeof(offload_data*), (uint8_t*)(&tmp));
         enqueue(ctxt->ready, data_to_be_sent);
         free_blob(data_to_be_sent);
      }
      else if(ctxt->offload_level[ctxt->node_id] == 2){	
         offload_data* tmp = heartbeat_detection(get_signal_block(seq_num, FRAME_NUM));
         tmp = heartbeat_segmentation(tmp);
         blob* data_to_be_sent = new_blob_and_copy_data(ctxt->node_id, sizeof(offload_data*), (uint8_t*)(&tmp));
         enqueue(ctxt->ready, data_to_be_sent);
         free_blob(data_to_be_sent);
      }
      else if(ctxt->offload_level[ctxt->node_id] == 3){		
         offload_data* tmp = heartbeat_detection(get_signal_block(seq_num, FRAME_NUM));
         blob* data_to_be_sent = new_blob_and_copy_data(ctxt->node_id, sizeof(offload_data*), (uint8_t*)(&tmp));
         enqueue(ctxt->ready, data_to_be_sent);
         free_blob(data_to_be_sent);
      }
      else {
         //Nothing to be done
      }

   }  
}

void* ecg_edge_offload(void* srv_conn, void* app_ctxt){
   ecg_context* ctxt = (ecg_context*)app_ctxt;
   service_conn* conn = (service_conn*)srv_conn;

   blob* data_to_be_sent = dequeue(ctxt->ready);
   if(ctxt->offload_level[ctxt->node_id] == 0){	
         result_data* results;
         memcpy((&results), data_to_be_sent->data, data_to_be_sent->size); 
         send_result(conn, results);
   }else{		
         offload_data* tmp;
         memcpy((&tmp), data_to_be_sent->data, data_to_be_sent->size); 
         send_offload_data(conn, tmp);
   }
   free_blob(data_to_be_sent);

   return NULL;
}



void ecg_edge_service(void *arg){
   const char* request_types[]={"offload_data"};
   void* (*handlers[])(void*, void*) = {ecg_edge_offload};
   int ecg_service = service_init(ECG_EDGE_PORT, TCP);
   start_service(ecg_service, TCP, request_types, 1, handlers, arg);
   close_service(ecg_service);
}


void ecg_edge(uint32_t edge_id){
   /*exec_barrier(START_CTRL, TCP, ctxt);*/
   /*TODO some sort of barrier must exist here*/
   ECG_init();
   ecg_context* ctxt = (ecg_context*)malloc(sizeof(ecg_context));
   ctxt->node_id = edge_id;
   ctxt->total_edge_number = 1;
   ctxt->offload_level[edge_id] = std::stoi((sim_ctxt.cluster)->edge_type[edge_id]);
   ctxt->ready = new_queue(1);

   sys_thread_t t1 = sys_thread_new("ecg_edge_service", ecg_edge_service, ctxt, 101, 0);
   sys_thread_t t2 = sys_thread_new("ecg_edge_process", ecg_edge_process, ctxt, 101, 0);

   sys_thread_join(t1);
   sys_thread_join(t2);
}

void ecg_gateway_process(void *arg){
   ecg_context* ctxt = (ecg_context*)arg; 
   #if IPV4_TASK
   const char* addr_list[MAX_EDGE_NUM] = {"192.168.4.9", "192.168.4.8", "192.168.4.4", "192.168.4.14", "192.168.4.15", "192.168.4.16"};
   #else 
   const char* addr_list[MAX_EDGE_NUM] = {"100:0:200:0:300:0:400:", "100:0:200:0:300:0:500:", "100:0:200:0:300:0:600:",
                                           "100:0:200:0:300:0:700:", "100:0:200:0:300:0:800:", "100:0:200:0:300:0:900:"};
   #endif
   offload_data* data[MAX_EDGE_NUM];
   result_data* results[MAX_EDGE_NUM];
   int node_id;
   char request_type[20] = "offload_data";

   int seq_num = 0;
   for(seq_num = 0; seq_num <FRAME_NUM; seq_num++){
      for(node_id = 0; node_id < ctxt->total_edge_number; node_id++){
         service_conn* conn = connect_service(TCP, addr_list[node_id], ECG_EDGE_PORT);
         send_request(request_type, 20, conn);
         if(ctxt->offload_level[node_id]==0)
            results[node_id] = recv_result(conn);
         else
            data[node_id] = recv_offload_data(conn);
         close_service_connection(conn);  
      }
      for(node_id = 0; node_id < ctxt->total_edge_number; node_id++){
         char result_file[50];
         sprintf(result_file, "./out_%d.log", node_id);
         if(ctxt->offload_level[node_id] == 3){	
            printf("Data form device %d, offload_level is 3 at time %f\n", node_id, sc_core::sc_time_stamp().to_seconds());
            offloading_data* tmp = heartbeat_segmentation(data[node_id]);
            tmp = feature_extraction(tmp);
            results[node_id] = classification(tmp, result_file);
         }
         else if(ctxt->offload_level[node_id] == 2){
            printf("Data form device %d, offload_level is 2 at time %f\n", node_id, sc_core::sc_time_stamp().to_seconds());	
            offloading_data* tmp = feature_extraction(data[node_id]);
            results[node_id] = classification(tmp, result_file);
         }
         else if(ctxt->offload_level[node_id] == 1){
            printf("Data form device %d, offload_level is 1 at time %f\n", node_id, sc_core::sc_time_stamp().to_seconds());			
            results[node_id] = classification(data[node_id], result_file);
            //printf("rpeaks: %d\n", results[node_id]->rpeaks);
            //printf("\t\t\tnormal: %d\n\t\t\tabnormal: %d\n", results[node_id]->normal, results[node_id]->abnormal);
         }
         else if(ctxt->offload_level[node_id] == 0){ 
            printf("Data form device %d, offload_level is 0 at time %f\n", node_id, sc_core::sc_time_stamp().to_seconds());			
         }else{
            //Doing nothing
         }
      }

   }

}

void ecg_gateway(uint32_t gateway_id){
   /*exec_barrier(START_CTRL, TCP, ctxt);*/
   /*TODO some sort of barrier must exist here*/
   ECG_init();
   ecg_context* ctxt = (ecg_context*)malloc(sizeof(ecg_context));
   ctxt->node_id = gateway_id;
   ctxt->total_edge_number = (sim_ctxt.cluster)->total_number;
   for(int node_id = 0; node_id < ctxt->total_edge_number; node_id++){
      ctxt->offload_level[node_id] = std::stoi((sim_ctxt.cluster)->edge_type[node_id]);
   }
   sys_thread_t t1 = sys_thread_new("ecg_gateway_process", ecg_gateway_process, ctxt, 101, 0);
   sys_thread_join(t1);
}



