#include <systemc.h>
//#include "omnet_dummy.h"
#include "test_utils.h"

double power_cli[10];
double CliEnergy[10];

void test1(void* arg){
   /*Initialize the data structure and network model*/
   uint32_t total_cli_num = 1;
   uint32_t this_cli_id = 0;

   uint32_t partitions_h = 3;
   uint32_t partitions_w = 3;
   uint32_t fused_layers = 8;

   char network_file[30] = "models/yolo.cfg";
   char weight_file[30] = "models/yolo.weights";

   device_ctxt* client_ctxt = deepthings_edge_init(partitions_h, partitions_w, fused_layers, network_file, weight_file, this_cli_id);
   //device_ctxt* client_ctxt1 = deepthings_edge_init(partitions_h, partitions_w, fused_layers, network_file, weight_file, this_cli_id);
   //device_ctxt* gateway_ctxt = deepthings_gateway_init(partitions_h, partitions_w, fused_layers, network_file, weight_file, total_cli_num, addr_list);

   /*Single-thread version*/
//   partition_frame_and_perform_inference_thread_single_device(client_ctxt);
//   transfer_data_with_number(client_ctxt, gateway_ctxt, FRAME_NUM*partitions_h*partitions_h);
//   deepthings_merge_result_thread_single_device(gateway_ctxt);

}

class top : public sc_module
{
   public:
     sc_fifo<char> fifo_inst1;
     sc_fifo<char> fifo_inst2;


      //sc_fifo<OmnetIf_pkt*> fifo_inst1;
     //sc_fifo<OmnetIf_pkt*> fifo_inst2;

     //cSimpleModuleWrapper *prod_inst;
    // cSimpleModuleWrapper *cons_inst;

     top(sc_module_name name) : sc_module(name), fifo_inst1(100), fifo_inst2(100){
     //  prod_inst = new cSimpleModuleWrapper("Producer1", 0);
    //   prod_inst->data_out(fifo_inst1);
    //   prod_inst->data_in(fifo_inst2);

     //  cons_inst = new cSimpleModuleWrapper("Consumer1", 1);
     //  cons_inst->data_out(fifo_inst2);
     //  cons_inst->data_in(fifo_inst1);
       test1(NULL);       
     }
};

int sc_main (int argc , char *argv[]) {
   top top1("Top1");
   sc_start();
   return 0;
}
