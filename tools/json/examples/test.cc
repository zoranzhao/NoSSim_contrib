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

using namespace rapidjson;
using namespace std;

vector<string> edge_ipv4_addr;
vector<string> edge_ipv6_addr;
vector<string> edge_mac_addr;

int main(){
   std::ifstream ifs("../template/address.json");
   std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
   ifs.close();
   Document d;
   d.Parse(content.c_str()); 

   //Parsing parameter for edge devices
     
   Value& item = d["edge"]["total_number"];
   assert(item.IsInt());
   cout << "total_number: " << item.GetInt() << endl;

   item = d["edge"]["ipv4_address"];
   assert(item.IsArray());
   for (SizeType i = 0; i < item.Size(); i++) {
      assert(item[i].IsString());
      cout << "IPv4 address is: " << item[i].GetString() << endl;
   }

   item = d["edge"]["ipv6_address"];
   assert(item.IsArray());
   for (SizeType i = 0; i < item.Size(); i++) {
      assert(item[i].IsString());
      cout << "IPv6 address is: " << item[i].GetString() << endl;
   }

   item = d["edge"]["mac_address"];
   assert(item.IsArray());
   for (SizeType i = 0; i < item.Size(); i++) {
      assert(item[i].IsString());
      //cout << "MAC address is: " << item[i].GetString() << endl;
      edge_mac_addr.push_back(item[i].GetString());
   }
   for(string& it : edge_mac_addr){
      cout << "MAC address is: " << it << endl;
   }
 
   //Parsing parameter for gateway device
   item = d["gateway"]["total_number"];
   assert(item.IsInt());
   cout << "total_number: " << item.GetInt() << endl;

   item = d["gateway"]["ipv4_address"];
   assert(item.IsString());
   cout << "ipv4_address: " << item.GetString() << endl;

   item = d["gateway"]["ipv6_address"];
   assert(item.IsString());
   cout << "ipv6_address: " << item.GetString() << endl;

   item = d["gateway"]["mac_address"];
   assert(item.IsString());
   cout << "mac_address: " << item.GetString() << endl;

   return 0;

}
