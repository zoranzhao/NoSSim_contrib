#include "test_utils.h"
#include "ftp.h"
#include "inference_engine_helper.h"
#include "frame_partitioner.h"
#include "reuse_data_serialization.h"


void process_everything_in_gateway(void *arg){
   cnn_model* model = (cnn_model*)(((device_ctxt*)(arg))->model);
#ifdef NNPACK
   nnp_initialize();
   model->net->threadpool = pthreadpool_create(THREAD_NUM);
#endif
   int32_t frame_num;
   for(frame_num = 0; frame_num < FRAME_NUM; frame_num ++){
      image_holder img = load_image_as_model_input(model, frame_num);
      forward_all(model, 0);   
      draw_object_boxes(model, frame_num);
      free_image_holder(model, img);
   }
#ifdef NNPACK
   pthreadpool_destroy(model->net->threadpool);
   nnp_deinitialize();
#endif
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
   /*send_reuse_data(ctxt, temp);*/
   /*if task doesn't generate any reuse_data*/
   blob* task_input_blob=temp;
   if(model->ftp_para_reuse->schedule[get_blob_task_id(task_input_blob)] != 1){
      printf("Serialize reuse data for task %d:%d \n", get_blob_cli_id(task_input_blob), get_blob_task_id(task_input_blob)); 
      blob* serialized_temp  = self_reuse_data_serialization(ctxt, get_blob_task_id(task_input_blob), get_blob_frame_seq(task_input_blob));
      copy_blob_meta(serialized_temp, task_input_blob);
      free_blob(serialized_temp);
   }
#endif
   copy_blob_meta(result, temp);
   enqueue(ctxt->result_queue, result); 
   free_blob(result);
}
