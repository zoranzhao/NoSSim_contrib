#include "darkiot.h"
#include "configure.h"
#include <string.h>
#ifndef TEST_CASES_H
#define TEST_CASES_H

#ifndef BATCH_SIZE
#define BATCH_SIZE 16
#endif

extern const char* addr_list[MAX_EDGE_NUM];

#ifndef UDP_TRANS_SIZE
#define UDP_TRANS_SIZE 512
#endif
static inline void read_from_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, struct sockaddr *from, socklen_t *fromlen);
static inline void write_to_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, const struct sockaddr *to, socklen_t tolen);

void test_socket_client_thread(void *arg);
void test_socket_client(uint32_t edge_id);

blob* test_socket_server_impl(int sockfd);
void test_socket_server_thread(void *arg);
void test_socket_server(uint32_t edge_id);


#endif /*TEST_CASES_H*/

