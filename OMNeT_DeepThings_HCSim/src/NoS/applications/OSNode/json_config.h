#ifndef JSON_CONFIG_H
#define JSON_CONFIG_H
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

class deepthings_config {
   int N;
   int M;
   int layers;
   std::string type;
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
   deepthings_config(std::string json_name){
      std::ifstream ifs(json_name);
      std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
      ifs.close();
      Document d;
      d.Parse(content.c_str()); 
      std::cout << "======================================================================================" << std::endl;
      Value& item = d["N"];
      assert(item.IsInt());
      N = item.GetInt();
      std::cout << "N: " << N << std::endl;

      item = d["M"];
      assert(item.IsInt());
      M = item.GetInt();
      std::cout << "M: " << M << std::endl;

      item = d["FusedLayers"];
      assert(item.IsInt());
      layers = item.GetInt();
      std::cout << "layers: " << layers << std::endl;

      item = d["runtime"];
      assert(item.IsString());
      type = item.GetString();
      std::cout << "type: " << type << std::endl;
      std::cout << "======================================================================================" << std::endl;

   }
   int getN(){
      return N;
   }
   int getM(){
      return M;
   }
   int getFusedLayers(){
      return layers;
   }
   std::string getRuntimeType(){
      return type;
   }
   ~deepthings_config(){}
};

class sim_config {
   int device_id;
   std::mutex unique_id_mutex;
   int edge_total_number;
   std::vector<std::string> edge_ipv4_addr;
   std::vector<std::string> edge_ipv6_addr;
   std::vector<std::string> edge_mac_addr;
   std::vector<std::string> idle_device_list;

//Application specific configuration
   deepthings_config* deepthings_para;


public:
   sim_config(){
      device_id = 0; 
      deepthings_para = new deepthings_config("app_config.json");
      load_config_json("sim_config.json");
   }
   ~sim_config(){
      delete deepthings_para;
   }
   int get_client_number(){
      return edge_total_number;
   }
   int generate_unique_edge_id(){
      std::lock_guard<std::mutex> guard(unique_id_mutex);
      return device_id++;
   }
   int get_gateway_id(){
      return edge_total_number;
   }
   void load_config_json(std::string filename){
      std::ifstream ifs(filename);
      std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
      ifs.close();
      Document d;
      d.Parse(content.c_str()); 

      //Parsing parameter for edge devices
      std::cout << "======================================================================================" << std::endl;     
      Value& item = d["edge"]["total_number"];
      assert(item.IsInt());
      std::cout << "total_number: " << item.GetInt() << std::endl;

      item = d["edge"]["ipv4_address"];
      assert(item.IsArray());
      for (SizeType i = 0; i < item.Size(); i++) {
         assert(item[i].IsString());
         std::cout << "IPv4 address is: " << item[i].GetString() << std::endl;
      }

      item = d["edge"]["ipv6_address"];
      assert(item.IsArray());
      for (SizeType i = 0; i < item.Size(); i++) {
         assert(item[i].IsString());
         std::cout << "IPv6 address is: " << item[i].GetString() << std::endl;
      }

      item = d["edge"]["mac_address"];
      assert(item.IsArray());
      for (SizeType i = 0; i < item.Size(); i++) {
         assert(item[i].IsString());
         std::cout << "MAC address is: " << item[i].GetString() << std::endl;
         edge_mac_addr.push_back(item[i].GetString());
      }   
      for(std::string& it : edge_mac_addr){
         std::cout << "MAC address is: " << it << std::endl;
      }
 
   //Parsing parameter for gateway device
      item = d["gateway"]["total_number"];
      assert(item.IsInt());
      std::cout << "total_number: " << item.GetInt() << std::endl;

      item = d["gateway"]["gateway_id"];
      assert(item.IsInt());
      std::cout << "gateway_id: " << item.GetInt() << std::endl;

      item = d["gateway"]["ipv4_address"];
      assert(item.IsString());
      std::cout << "ipv4_address: " << item.GetString() << std::endl;

      item = d["gateway"]["ipv6_address"];
      assert(item.IsString());
      std::cout << "ipv6_address: " << item.GetString() << std::endl;

      item = d["gateway"]["mac_address"];
      assert(item.IsString());
      std::cout << "mac_address: " << item.GetString() << std::endl;
      std::cout << "======================================================================================" << std::endl;   

   }
/*
   void load_config_json(char* filename){
   }
   char* get_gateway_ip(){
   }   
*/
};
extern sim_config simulation_config;
#endif //JSON_CONFIG_H

