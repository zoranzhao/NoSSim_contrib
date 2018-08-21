//***************************************************************                                                       
// * A (lwip) socket-based service API for IoT cluster middleware                                                                     
// * Zhuoran Zhao, UT Austin, zhuoran@utexas.edu                                                    
// * Last update: July 2018                                                                         
//***************************************************************

//Header file for sockets, drivers and OS APIs
#ifndef SERVICE_API__H
#define SERVICE_API__H

#include "HCSim.h"
#include "lwip_ctxt.h"

#define IPV4_TASK 1
#define IPV6_TASK !(IPV4_TASK)
#define LOWPAN6_TASK IPV6_TASK

typedef enum proto{
   TCP,
   UDP
} ctrl_proto;

typedef struct data_buffer{
   void* data;
   uint32_t size;
} raw_data;


raw_data* make_raw_data(uint32_t size);
void free_raw_data(raw_data* blob);
void copy_raw_data(raw_data* blob1, raw_data* blob2);
raw_data* pack_raw_data(void* data, uint32_t size);
void extract_raw_data(raw_data* blob, void** data_ptr, uint32_t* size_ptr);
raw_data* write_file_to_raw_data(const char *filename);
void write_raw_data_to_file(const char *filename, raw_data* blob);
int service_init(int portno, ctrl_proto proto);
void send_data(raw_data *blob, ctrl_proto proto, const char *dest_ip, int portno);
raw_data* recv_data(int sockfd, ctrl_proto proto);

static inline void read_from_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, struct sockaddr *from, socklen_t *fromlen);
static inline void write_to_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, const struct sockaddr *to, socklen_t tolen);



#endif // SERVICE_API__H 
