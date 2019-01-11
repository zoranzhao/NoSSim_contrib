#ifndef TEST_H
#define TEST_H
#include "darkiot.h"
#include "configure.h"

device_ctxt* deepthings_edge_init_single_device(uint32_t N, uint32_t M, uint32_t fused_layers, char* network, char* weights, uint32_t edge_id);
void process_task_single_device(device_ctxt* ctxt, blob* temp, bool is_reuse);

#endif
