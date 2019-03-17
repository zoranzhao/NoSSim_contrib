#include <stdint.h>
unsigned char* get_kp_data(unsigned char* kp_data);
uint32_t get_kp_size(unsigned char* kp_data);
uint32_t get_kp_byte_size(unsigned char* kp_data);
unsigned char* keypoints_extraction(const char* filename);
uint32_t keypoints_matching(unsigned char* kp_data1, unsigned char* kp_data2, const char* file1, const char* file2);

