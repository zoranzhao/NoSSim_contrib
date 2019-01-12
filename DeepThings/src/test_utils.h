#ifndef TEST_H
#define TEST_H
#include "darkiot.h"
#include "configure.h"
#include "cmd_line_parser.h"
#include "test_utils.h"
#include "frame_partitioner.h"

/*Functions defined in the DeepThings library*/
device_ctxt* deepthings_edge_init(uint32_t N, uint32_t M, uint32_t fused_layers, char* network, char* weights, uint32_t edge_id);
device_ctxt* deepthings_gateway_init(uint32_t N, uint32_t M, uint32_t fused_layers, char* network, char* weights, uint32_t total_edge_number, const char** addr_list);

/*Functions defined for testing and profiling*/
void process_task_single_device(device_ctxt* ctxt, blob* temp, bool is_reuse);
void process_everything_in_gateway(void *arg);

#endif
