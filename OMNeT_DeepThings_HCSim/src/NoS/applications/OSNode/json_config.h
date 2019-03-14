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

#include "os_ctxt.h"

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
//Application specific configuration
public:
   deepthings_config* deepthings_para;
   cluster_config* cluster;
   int data_source;
   sim_results* result;
   sim_config(){
      device_id = 0; 
      data_source = 0;

      deepthings_para = new deepthings_config("app_config.json");

      cluster = new cluster_config();
      sim_ctxt.cluster = cluster; //The cluster address table should also be included in the simulation context
      load_config_json("sim_config.json");

      result = new sim_results(cluster->total_number);
      sim_ctxt.result = result; //The result table should also be included in the simulation context
   }
   ~sim_config(){
      delete deepthings_para;
      delete cluster;
      delete result;
   }
   int generate_unique_edge_id(){
      std::lock_guard<std::mutex> guard(unique_id_mutex);
      return device_id++;
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
      cluster->total_number = item.GetInt();

      item = d["edge"]["edge_id"];
      assert(item.IsArray());
      for (SizeType i = 0; i < item.Size(); i++) {
         assert(item[i].IsInt());
         (cluster->edge_id).push_back(item[i].GetInt());
         //std::cout << "IPv4 address is: " << item[i].GetString() << std::endl;
      }

      item = d["edge"]["edge_type"];
      assert(item.IsArray());
      for (SizeType i = 0; i < item.Size(); i++) {
         assert(item[i].IsString());
         (cluster->edge_type)[(cluster->edge_id)[i]] = item[i].GetString();
         std::cout << "Device type is: " << item[i].GetString() << std::endl;
         std::cout << "Is victim or not: " << (std::string("victim") == std::string(item[i].GetString())) << std::endl;
         if(std::string("victim") == std::string(item[i].GetString())) data_source++;
         std::cout << "data_source number is: " << data_source << std::endl;
      }

      item = d["edge"]["edge_core_number"];
      assert(item.IsArray());
      for (SizeType i = 0; i < item.Size(); i++) {
         assert(item[i].IsInt());
         assert(item.Size() == (cluster->edge_id).size());
         (cluster->edge_core_number)[(cluster->edge_id)[i]] = item[i].GetInt();
         //std::cout << "IPv4 address is: " << item[i].GetString() << std::endl;
      }

      item = d["edge"]["edge_ipv4_address"];
      assert(item.IsArray());
      for (SizeType i = 0; i < item.Size(); i++) {
         assert(item[i].IsString());
         (cluster->edge_ipv4_address)[(cluster->edge_id)[i]] = item[i].GetString();
         //std::cout << "IPv4 address is: " << item[i].GetString() << std::endl;
      }

      item = d["edge"]["edge_ipv6_address"];
      assert(item.IsArray());
      for (SizeType i = 0; i < item.Size(); i++) {
         assert(item[i].IsString());
         (cluster->edge_ipv6_address)[(cluster->edge_id)[i]] = item[i].GetString();
      }

      item = d["edge"]["edge_mac_address"];
      assert(item.IsArray());
      for (SizeType i = 0; i < item.Size(); i++) {
         assert(item[i].IsString());
         (cluster->edge_mac_address)[(cluster->edge_id)[i]] = item[i].GetString();
         (cluster->edge_mac_address_to_id)[item[i].GetString()] = (cluster->edge_id)[i];
      }   

 
      //Parsing parameter for gateway device
      item = d["gateway"]["gateway_id"];
      assert(item.IsInt());
      cluster->gateway_id = item.GetInt();
      //std::cout << "gateway_id: " << std::endl;
      item = d["gateway"]["gateway_core_number"];
      assert(item.IsInt());
      cluster->gateway_core_number = item.GetInt();

      item = d["gateway"]["gateway_ipv4_address"];
      assert(item.IsString());
      cluster->gateway_ipv4_address = item.GetString();

      item = d["gateway"]["gateway_ipv6_address"];
      assert(item.IsString());
      cluster->gateway_ipv6_address = item.GetString();

      item = d["gateway"]["gateway_mac_address"];
      assert(item.IsString());
      cluster->gateway_mac_address = item.GetString();

      item = d["ap_mac_address"];
      assert(item.IsString());
      cluster->ap_mac_address = item.GetString();
      std::cout << "======================================================================================" << std::endl;   

      cluster->print();
   }

   void write_result_json(std::string filename){
      Document document; 
      document.SetObject();
      Document::AllocatorType& allocator = document.GetAllocator();
      for(int node_id = 0; node_id < cluster->total_number; node_id ++){
         Value object(kObjectType);
         std::unordered_map<std::string, double> edge = result->get_edge_result(node_id);
         for ( auto it = edge.begin(); it != edge.end(); ++it ){
            Value metric_name((it->first).c_str(), (it->first).size(), allocator); 
            object.AddMember(metric_name, it->second, allocator);
         }

         Value key(std::string("edge_" + std::to_string(node_id) ).c_str(), std::string("edge_" + std::to_string(node_id) ).size(), allocator); 
         document.AddMember(key, object, allocator);
      }

      Value object(kObjectType);
      std::unordered_map<std::string, double> gateway = result->get_gateway_result();
      for ( auto it = gateway.begin(); it != gateway.end(); ++it ){
            Value metric_name((it->first).c_str(), (it->first).size(), allocator); 
            object.AddMember(metric_name, it->second, allocator);
      }
      Value key("gateway");
      document.AddMember(key, object, allocator);

      StringBuffer sb;
      PrettyWriter<StringBuffer> writer(sb);
      document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
      puts(sb.GetString());

      std::string json (sb.GetString(), sb.GetSize());
      std::ofstream ofs(filename);
      ofs << json;
   }   


};
extern sim_config simulation_config;
#endif //JSON_CONFIG_H

