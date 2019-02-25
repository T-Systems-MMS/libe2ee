//
//  AfghParser.hpp
//  AFGHCrypto
//
//  Created by Jan Starke on 11.11.18.
//  Copyright © 2018 T-Systems. All rights reserved.
//

#ifndef AfghParser_hpp
#define AfghParser_hpp

#include "objects/PbcObject.hpp"
#include <map>

namespace afgh {
  class Parser {
  protected:
    void init();
  public:
    
    Parser() {init();}
    void parse(const char* jsonString);
    
    
  private:
    std::map<std::string, PbcObject*> objects;
  };
  
}

#endif /* AfghParser_hpp */
