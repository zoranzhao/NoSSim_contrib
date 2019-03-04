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
#include <unordered_map>

using namespace rapidjson;
using namespace std;


class deepthings_config {
   int N;
   int M;
   int layers;
   string type;
public:
   deepthings_config(){
      N = 5;
      M = 5;
      layers = 16;
   }
   deepthings_config(int partitions_h, int partitions_w, int fused_layers){
      N = partitions_h;
      M = partitions_w;
      layers = fused_layers;
   }
   int getN(){
      return N;
   }
   int getM(){
      return N;
   }
   int getFusedLayers(){
      return layers;
   }
   ~deepthings_config(){}
};

class sim_config {
   int device_id;
   mutex unique_id_mutex;
   int edge_total_number;
   vector<string> edge_ipv4_addr;
   vector<string> edge_ipv6_addr;
   vector<string> edge_mac_addr;
   vector<string> idle_device_list;

//Application specific configuration
   deepthings_config* deepthings_para;


public:
   sim_config(){
      device_id = 0; 
      deepthings_para = new deepthings_config();
      //load_config_json("sim_config.json");
   }
   ~sim_config(){
      delete deepthings_para;
   }
/*
   void load_config_json(char* filename){
   
   }
   void load_profile(char* filename){
                  
   }
   int get_client_number(){

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
*/
};



extern sim_config simulation_config;
