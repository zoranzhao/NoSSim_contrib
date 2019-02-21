#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>

using namespace rapidjson;
using namespace std;


class sim_config {
   int device_id;
   mutex unique_id_mutex;
   int edge_total_number;
   vector<string> edge_ipv4_addr;
   vector<string> edge_ipv6_addr;
   vector<string> edge_mac_addr;


public:
   sim_config(){
      device_id = 0; 
      load_config_json("sim_config.json");
   }

   void load_config_json(char* filename){

   }

   int get_client_number(){

   }

   char* get_gateway_ip(){

   }   

   char* get_gateway_ip(){

   }   

   int generate_unique_edge_id(){
      lock_guard<mutex> guard(unique_id_mutex);
      return device_id++;
   }

   int generate_gateway_id(){
      return edge_total_number;
   }
}
