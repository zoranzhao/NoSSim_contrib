#include "test_utils.h"

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

void partition_frame_and_perform_inference_thread_single_device(void *arg){
   device_ctxt* ctxt = (device_ctxt*)arg;
   cnn_model* model = (cnn_model*)(ctxt->model);
#ifdef NNPACK
   nnp_initialize();
   model->net->threadpool = pthreadpool_create(THREAD_NUM);
#endif
   blob* temp;
   uint32_t frame_num;
   /*bool* reuse_data_is_required;*/   
   for(frame_num = 0; frame_num < FRAME_NUM; frame_num ++){
      /*Wait for i/o device input*/
      /*recv_img()*/

      /*Load image and partition, fill task queues*/
      load_image_as_model_input(model, frame_num);
      partition_and_enqueue(ctxt, frame_num);
      /*register_client(ctxt);*/

      /*Dequeue and process task*/
      while(1){
         temp = try_dequeue(ctxt->task_queue);
         if(temp == NULL) break;
         bool data_ready = false;
         printf("====================Processing task id is %d, data source is %d, frame_seq is %d====================\n", get_blob_task_id(temp), get_blob_cli_id(temp), get_blob_frame_seq(temp));
#if DATA_REUSE
         data_ready = is_reuse_ready(model->ftp_para_reuse, get_blob_task_id(temp));
         if((model->ftp_para_reuse->schedule[get_blob_task_id(temp)] == 1) && data_ready) {
            blob* shrinked_temp = new_blob_and_copy_data(get_blob_task_id(temp), 
                       (model->ftp_para_reuse->shrinked_input_size[get_blob_task_id(temp)]),
                       (uint8_t*)(model->ftp_para_reuse->shrinked_input[get_blob_task_id(temp)]));
            copy_blob_meta(shrinked_temp, temp);
            free_blob(temp);
            temp = shrinked_temp;

            /*Assume all reusable data is generated locally*/
            /*
            reuse_data_is_required = check_missing_coverage(model, get_blob_task_id(temp), get_blob_frame_seq(temp));
            request_reuse_data(ctxt, temp, reuse_data_is_required);
            free(reuse_data_is_required);
	    */
         }
#if DEBUG_DEEP_EDGE
         if((model->ftp_para_reuse->schedule[get_blob_task_id(temp)] == 1) && (!data_ready))
            printf("The reuse data is not ready yet!\n");
#endif/*DEBUG_DEEP_EDGE*/

#endif/*DATA_REUSE*/
         /*process_task(ctxt, temp, data_ready);*/
         process_task_single_device(ctxt, temp, data_ready);
         free_blob(temp);
      }

      /*Unregister and prepare for next image*/
      /*cancel_client(ctxt);*/
   }
#ifdef NNPACK
   pthreadpool_destroy(model->net->threadpool);
   nnp_deinitialize();
#endif
}

void deepthings_merge_result_thread_single_device(void *arg){
   cnn_model* model = (cnn_model*)(((device_ctxt*)(arg))->model);
#ifdef NNPACK
   nnp_initialize();
   model->net->threadpool = pthreadpool_create(THREAD_NUM);
#endif
   blob* temp;
   int32_t cli_id;
   int32_t frame_seq;
   int32_t count = 0;
   for(count = 0; count < FRAME_NUM; count ++){
      temp = dequeue_and_merge((device_ctxt*)arg);
      cli_id = get_blob_cli_id(temp);
      frame_seq = get_blob_frame_seq(temp);
#if DEBUG_FLAG
      printf("Client %d, frame sequence number %d, all partitions are merged in deepthings_merge_result_thread\n", cli_id, frame_seq);
#endif
      float* fused_output = (float*)(temp->data);
      image_holder img = load_image_as_model_input(model, get_blob_frame_seq(temp));
      set_model_input(model, fused_output);
      forward_all(model, model->ftp_para->fused_layers);   
      draw_object_boxes(model, get_blob_frame_seq(temp));
      free_image_holder(model, img);
      free_blob(temp);
#if DEBUG_FLAG
      printf("Client %d, frame sequence number %d, finish processing\n", cli_id, frame_seq);
#endif
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

void transfer_data_with_number(device_ctxt* client, device_ctxt* gateway, int32_t task_num){
   int32_t cli_id = client->this_cli_id;
   int32_t count = 0;
   for(count = 0; count < task_num; count ++){
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


/*Reuse data serialization method in gateway device*/
/*
//Collect reuse data from edge devices
static overlapped_tile_data* overlapped_data_pool[MAX_EDGE_NUM][PARTITIONS_MAX];
void recv_reuse_data_from_edge_single_device(device_ctxt* gateway, blob* recv_data_blob){
   printf("collecting_reuse_data ... ... \n");
   cnn_model* gateway_model = gateway->model;
   blob* temp = recv_data_blob;
   int32_t cli_id = 0;
   int32_t task_id = get_blob_task_id(temp);
   if(overlapped_data_pool[cli_id][task_id] != NULL)
      free_self_overlapped_tile_data(gateway_model,  overlapped_data_pool[cli_id][task_id]);
   overlapped_data_pool[cli_id][task_id] = self_reuse_data_deserialization(gateway_model, task_id, (float*)temp->data, get_blob_frame_seq(temp));
   //Should I?//free_blob(temp);
}


//Hand out reuse data to edge devices
void* send_reuse_data_to_edge(device_ctxt* ctxt, blob* recv_data_blob){
   printf("handing_out_reuse_data ... ... \n");

   cnn_model* gateway_model = (cnn_model*)(ctxt->model);

   int32_t cli_id;
   int32_t task_id;
   uint32_t frame_num;
   blob* temp = recv_data_blob;
   cli_id = get_blob_cli_id(temp);
   task_id = get_blob_task_id(temp);
   frame_num = get_blob_frame_seq(temp);
   free_blob(temp);

   blob* reuse_info_blob = recv_data_blob;
   bool* reuse_data_is_required = (bool*)(reuse_info_blob->data);

   uint32_t position;
   int32_t* adjacent_id = get_adjacent_task_id_list(gateway_model, task_id);

   for(position = 0; position < 4; position++){
      if(adjacent_id[position]==-1) continue;
      if(reuse_data_is_required[position]){
         place_self_deserialized_data(gateway_model, adjacent_id[position], overlapped_data_pool[cli_id][adjacent_id[position]]);
      }
   }
   free(adjacent_id);
   temp = adjacent_reuse_data_serialization(ctxt, task_id, frame_num, reuse_data_is_required);
   free_blob(reuse_info_blob);
   free_blob(temp);
}
*/


/*Reuse data serialization method in edge device*/ 
/*
//Request reuse data from gateway
void request_reuse_data_single_device(device_ctxt* gateway_ctxt, blob* task_input_blob, blob* recv_data_blob, bool* reuse_data_is_required){
   cnn_model* model = (cnn_model*)(gateway_ctxt->model);
   if(model->ftp_para_reuse->schedule[get_blob_task_id(task_input_blob)] == 0) return;//Task without any dependency
   temp = recv_data_blob;
   copy_blob_meta(temp, task_input_blob);
   overlapped_tile_data** temp_region_and_data = adjacent_reuse_data_deserialization(model, get_blob_task_id(temp), (float*)temp->data, get_blob_frame_seq(temp), reuse_data_is_required);
   place_adjacent_deserialized_data(model, get_blob_task_id(temp), temp_region_and_data, reuse_data_is_required);
   //Should I?//free_blob(temp);
   close_service_connection(conn);
}
*/


/*
//Send generated reuse data to gateway 
void send_reuse_data_single_device(device_ctxt* edge_ctxt, device_ctxt* gateway_ctxt, blob* task_input_blob){
   cnn_model* model = (cnn_model*)(edge_ctxt->model);
   if(model->ftp_para_reuse->schedule[get_blob_task_id(task_input_blob)] == 1) return;
   blob* temp  = self_reuse_data_serialization(edge_ctxt, get_blob_task_id(task_input_blob), get_blob_frame_seq(task_input_blob));
   copy_blob_meta(temp, task_input_blob);

   //Transfer it to the gateway  
   recv_reuse_data_from_edge_single_device(gateway_ctxt, temp);
   //Transfer it to the gateway 

   free_blob(temp);
}
*/

