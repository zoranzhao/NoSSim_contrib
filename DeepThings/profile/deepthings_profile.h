#ifndef DEEPTHINGS_PROFILE_H
#define DEEPTHINGS_PROFILE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdbool.h> 

#ifndef PARTITIONS_W_MAX
#define PARTITIONS_W_MAX 6
#endif

#ifndef PARTITIONS_H_MAX
#define PARTITIONS_H_MAX 6
#endif

#ifndef PARTITIONS_MAX
#define PARTITIONS_MAX 36
#endif

#ifndef FRAME_NUM
#define FRAME_NUM 4
#endif

#ifndef FUSED_LAYERS_MAX
#define FUSED_LAYERS_MAX 16
#endif

#define NUM_OF_FUNCTIONS 6

typedef struct dp_profile_data {
  double start_time;
  bool valid[FRAME_NUM][PARTITIONS_MAX][2];
  double total_duration[FRAME_NUM][PARTITIONS_MAX][2]; /*0 no data-reuse, 1 data-reuse*/
  double avg_duration[FRAME_NUM][PARTITIONS_MAX][2]; /*0 no data-reuse, 1 data-reuse*/
  long calling_times[FRAME_NUM][PARTITIONS_MAX][2]; /*0 no data-reuse, 1 data-reuse*/
} deepthings_profile_data;


extern deepthings_profile_data deepthings_prof_data[NUM_OF_FUNCTIONS];
extern char function_list[NUM_OF_FUNCTIONS][40];

uint32_t get_function_id(char* function_name);
char* get_function_name(uint32_t id);
void dump_profile(char* filename);
void load_profile(char * filename);
void profile_start();
void profile_end(uint32_t partition_h, uint32_t partition_w, uint32_t layers);
void start_timer(char* function_name, uint32_t frame_number, uint32_t partition_number, uint32_t data_reuse);
void stop_timer(char* function_name, uint32_t frame_number, uint32_t partition_number, uint32_t data_reuse);

#endif
