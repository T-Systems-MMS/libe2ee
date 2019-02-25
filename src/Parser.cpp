//
//  AfghParser.cpp
//  AFGHCrypto
//
//  Created by Jan Starke on 11.11.18.
//  Copyright © 2018 T-Systems. All rights reserved.
//

#include "Parser.hpp"
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

namespace afgh {
  
  void Parser::init() {
    objects.clear();
  }
  
  void Parser::parse(const char* jsonString) {
    struct json_object* base = json_tokener_parse(jsonString);
    struct json_object* root, *type, *idobj;
    struct json_object* structureobj;
    
    if (! json_object_object_get_ex(base, "root", &root)) {
      json_object_put(base);
      throw_line("missing 'root' key");
    }
    
    if (! json_object_object_get_ex(root, "type", &type)) {
      json_object_put(base);
      throw_line("missing 'type' key");
    }
    
    if (! json_object_object_get_ex(root, "id", &idobj)) {
      json_object_put(base);
      throw_line("missing 'id' key");
    }
    
    std::string id = json_object_get_string(idobj);
    if (! json_object_object_get_ex(base, id.c_str(), &structureobj)) {
      json_object_put(base);
      throw_line("id refers to missing object");
    }
    
    
    json_object_put(base);
  }
  
}
