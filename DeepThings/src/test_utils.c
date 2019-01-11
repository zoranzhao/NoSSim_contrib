#include "test_utils.h"
#include "ftp.h"
#include "inference_engine_helper.h"
#include "frame_partitioner.h"
#include "reuse_data_serialization.h"

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
