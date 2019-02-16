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

int main(){
   std::ifstream ifs("./conf.json");
   std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
   ifs.close();
   Document d;
   d.Parse(content.c_str());      
   Value& item = d["edge"];
// Value& items = item

   for (MemberIterator m = document["a"].MemberBegin(); m != document["a"].MemberEnd(); ++m) {
      std::cout << m.name << " " << (m.IsNumber()?m.GetNumber():m.GetString()) << endl;
   }

   return 0;

}
