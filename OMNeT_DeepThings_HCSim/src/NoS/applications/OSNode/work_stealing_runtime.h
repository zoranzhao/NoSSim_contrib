#ifndef WORK_STEALING_RUNTIME_H
#define WORK_STEALING_RUNTIME_H

#include "darkiot.h"
#include "configure.h"
#include <string.h>
#include "test_utils.h"
#include "json_config.h"

#ifndef BATCH_SIZE
#define BATCH_SIZE 8
#endif

extern const char* addr_list[MAX_EDGE_NUM];
/*
void test_gateway(uint32_t total_number){
   device_ctxt* ctxt = init_gateway(total_number, addr_list);
   set_gateway_local_addr(ctxt, GATEWAY_LOCAL_ADDR);
   set_gateway_public_addr(ctxt, GATEWAY_PUBLIC_ADDR);
   set_total_frames(ctxt, FRAME_NUM);
   set_batch_size(ctxt, BATCH_SIZE);

   sys_thread_t t3 = sys_thread_new("work_stealing_thread", work_stealing_thread, ctxt, 0, 0);
   sys_thread_t t1 = sys_thread_new("collect_result_thread", collect_result_thread, ctxt, 0, 0);
   sys_thread_t t2 = sys_thread_new("merge_result_thread", merge_result_thread, ctxt, 0, 0);
   exec_barrier(START_CTRL, TCP, ctxt);

   sys_thread_join(t1);
   sys_thread_join(t2);
   sys_thread_join(t3);
}
*/

/*
void all_in_one_device_test(void* arg){
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
*/

void partition_frame_and_perform_inference_thread_no_reuse_no_gateway(void *arg);
void steal_partition_and_perform_inference_thread_no_reuse_no_gateway(void *arg);
void test_deepthings_stealer_edge(uint32_t N, uint32_t M, uint32_t fused_layers, uint32_t edge_id);
void test_deepthings_victim_edge(uint32_t N, uint32_t M, uint32_t fused_layers, uint32_t edge_id);


/*------------------------------------------------------*/
/*---Test functions for distributed work stealing runtime system---*/
static void process_task(blob* temp, device_ctxt* ctxt);
void generate_and_process_thread_no_gateway(void *arg);
void steal_and_process_thread_no_gateway(void *arg);
void test_stealer_client(uint32_t edge_id);
void test_victim_client(uint32_t edge_id);
void test_deepthings_gateway(uint32_t N, uint32_t M, uint32_t fused_layers, uint32_t total_edge_number);


#endif /*WORK_STEALING_RUNTIME_H*/

