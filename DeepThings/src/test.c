#include "darkiot.h"
#include "configure.h"
#include "cmd_line_parser.h"
#include "ftp.h"
#include "inference_engine_helper.h"
#include "frame_partitioner.h"
#include "reuse_data_serialization.h"
#include "deepthings_edge.h"
#include "deepthings_gateway.h"

static const char* addr_list[MAX_EDGE_NUM] = EDGE_ADDR_LIST;

void process_task_single_device(device_ctxt* ctxt, blob* temp, bool is_reuse){
   printf("Task is: %d, frame number is %d\n", get_blob_task_id(temp), get_blob_frame_seq(temp));
   cnn_model* model = (cnn_model*)(ctxt->model);
   blob* result;
   set_model_input(model, (float*)temp->data);
   forward_partition(model, get_blob_task_id(temp), is_reuse);  
   result = new_blob_and_copy_data(0, 
                                      get_model_byte_size(model, model->ftp_para->fused_layers-1), 
                                      (uint8_t*)(get_model_output(model, model->ftp_para->fused_layers-1))
                                     );
#if DATA_REUSE
   //send_reuse_data(ctxt, temp);
   
#endif
   copy_blob_meta(result, temp);
   enqueue(ctxt->result_queue, result); 
   free_blob(result);
}

void partition_frame_and_perform_inference_thread_single_device(void *arg){
   device_ctxt* ctxt = (device_ctxt*)arg;
   cnn_model* model = (cnn_model*)(ctxt->model);
#ifdef NNPACK
   nnp_initialize();
   model->net->threadpool = pthreadpool_create(THREAD_NUM);
#endif
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
         process_task_single_device(ctxt, temp, data_ready);
         free_blob(temp);
      }
   }
#ifdef NNPACK
   pthreadpool_destroy(model->net->threadpool);
   nnp_deinitialize();
#endif
}

void transfer_data(device_ctxt* client, device_ctxt* gateway){
   int32_t cli_id = client->this_cli_id;
   while(1){
      blob* temp = dequeue(client->result_queue);
      enqueue(gateway->results_pool[cli_id], temp);
      gateway->results_counter[cli_id]++;
      free_blob(temp);
      if(gateway->results_counter[cli_id] == gateway->batch_size){
         temp = new_empty_blob(cli_id);
         enqueue(gateway->ready_pool, temp);
         free_blob(temp);
         gateway->results_counter[cli_id] = 0;
      }
   }
}


void deepthings_merge_result_thread(void *arg);

device_ctxt* deepthings_edge_init_single_device(uint32_t N, uint32_t M, uint32_t fused_layers, char* network, char* weights, uint32_t edge_id){
   device_ctxt* ctxt = init_client(edge_id);
   cnn_model* model = load_cnn_model(network, weights);
   model->ftp_para = preform_ftp(N, M, fused_layers, model->net_para);
#if DATA_REUSE
   model->ftp_para_reuse = preform_ftp_reuse(model->net_para, model->ftp_para);
#endif
   ctxt->model = model;
   set_gateway_local_addr(ctxt, GATEWAY_LOCAL_ADDR);
   set_gateway_public_addr(ctxt, GATEWAY_PUBLIC_ADDR);
   set_total_frames(ctxt, FRAME_NUM);
   set_batch_size(ctxt, N*M);

   return ctxt;
}

int main(int argc, char **argv){
   /*Initialize the data structure and network model*/
   uint32_t total_cli_num = get_int_arg(argc, argv, "-total_edge", 1);
   uint32_t this_cli_id = get_int_arg(argc, argv, "-edge_id", 0);

   uint32_t partitions_h = get_int_arg(argc, argv, "-n", 5);
   uint32_t partitions_w = get_int_arg(argc, argv, "-m", 5);
   uint32_t fused_layers = get_int_arg(argc, argv, "-l", 16);

   char network_file[30] = "models/yolo.cfg";
   char weight_file[30] = "models/yolo.weights";

   device_ctxt* client_ctxt = deepthings_edge_init_single_device(partitions_h, partitions_w, fused_layers, network_file, weight_file, this_cli_id);
   //device_ctxt* gateway_ctxt = deepthings_gateway_init(partitions_h, partitions_w, fused_layers, network_file, weight_file, total_cli_num, addr_list);
   partition_frame_and_perform_inference_thread_single_device(client_ctxt);
   //sys_thread_t t1 = sys_thread_new("partition_frame_and_perform_inference_thread_single_device", 
                        //             partition_frame_and_perform_inference_thread_single_device, client_ctxt, 0, 0);
   //sys_thread_t t2 = sys_thread_new("deepthings_merge_result_thread", deepthings_merge_result_thread, gateway_ctxt, 0, 0);

   //transfer_data(client_ctxt, gateway_ctxt);
   

   return 0;
}

