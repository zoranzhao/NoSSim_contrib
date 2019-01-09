#include "darkiot.h"
#include "config.h"
#include "configure.h"
#include "ftp.h"
#include "inference_engine_helper.h"
#include "frame_partitioner.h"
#include "reuse_data_serialization.h"
#include <string.h>
#include "deepthings_edge.h"
#include "deepthings_gateway.h"

void local_ftp(int argc, char **argv){
   this_cli_id = 0;
   total_cli_num = 1;
   init_queues(total_cli_num);
   cnn_model* model = load_cnn_model((char*)"models/yolo.cfg", (char*)"models/yolo.weights");
   model->ftp_para = preform_ftp(3, 3, 4, model->net_para);

   for(int frame_seq = 0; frame_seq < 4; frame_seq++){
      image_holder img = load_image_as_model_input(model, frame_seq);
      partition_and_enqueue(model, frame_seq);
      blob* temp;
      blob* result;
      while(1){
         temp = try_dequeue(task_queue);
         if(temp == NULL) break;
         printf("Task id is %d\n", temp->id);
         set_model_input(model, (float*)temp->data);
         forward_partition(model, get_blob_task_id(temp), 
                         model->ftp_para_reuse->schedule[get_blob_task_id(temp)] == 1 && is_reuse_ready(model->ftp_para_reuse, get_blob_task_id(temp))
                         );  
         result = new_blob_and_copy_data(0, 
                                      get_model_byte_size(model, model->ftp_para->fused_layers-1), 
                                      (uint8_t*)(get_model_output(model, model->ftp_para->fused_layers-1))
                                     );
         copy_blob_meta(result, temp);
         enqueue(results_pool[this_cli_id], result);
         free_blob(result);
         free_blob(temp);
      }

      enqueue(ready_pool, new_empty_blob(this_cli_id));
      temp = (dequeue_and_merge(model));
      float* fused_output = (float* )temp->data;
      set_model_input(model, fused_output);
      forward_all(model, model->ftp_para->fused_layers);   
      free(fused_output);
      draw_object_boxes(model, frame_seq);
      free_image_holder(model, img);
   }
}


int main(int argc, char **argv){
   local_ftp(argc, argv);
   return 0;
}

