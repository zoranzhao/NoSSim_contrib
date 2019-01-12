#include "test_utils.h"

static const char* addr_list[MAX_EDGE_NUM] = EDGE_ADDR_LIST;


void transfer_data(device_ctxt* client, device_ctxt* gateway){
   int32_t cli_id = client->this_cli_id;
   while(1){
      blob* temp = dequeue(client->result_queue);
      printf("Transfering data from client %d to gateway\n", cli_id);
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


int main(int argc, char **argv){
   /*Initialize the data structure and network model*/
   uint32_t total_cli_num = get_int_arg(argc, argv, "-total_edge", 1);
   uint32_t this_cli_id = get_int_arg(argc, argv, "-edge_id", 0);

   uint32_t partitions_h = get_int_arg(argc, argv, "-n", 3);
   uint32_t partitions_w = get_int_arg(argc, argv, "-m", 3);
   uint32_t fused_layers = get_int_arg(argc, argv, "-l", 8);

   char network_file[30] = "models/yolo.cfg";
   char weight_file[30] = "models/yolo.weights";

   device_ctxt* client_ctxt = deepthings_edge_init(partitions_h, partitions_w, fused_layers, network_file, weight_file, this_cli_id);
   device_ctxt* gateway_ctxt = deepthings_gateway_init(partitions_h, partitions_w, fused_layers, network_file, weight_file, total_cli_num, addr_list);
   sys_thread_t t1 = sys_thread_new("partition_frame_and_perform_inference_thread_single_device", 
                                     partition_frame_and_perform_inference_thread_single_device, client_ctxt, 0, 0);
   sys_thread_t t2 = sys_thread_new("deepthings_merge_result_thread_single_device", deepthings_merge_result_thread_single_device, gateway_ctxt, 0, 0);
   transfer_data(client_ctxt, gateway_ctxt);

   //sys_thread_t t = sys_thread_new("process_everything_in_gateway", process_everything_in_gateway, gateway_ctxt, 0, 0);
   //sys_thread_join(t);

   return 0;
}

