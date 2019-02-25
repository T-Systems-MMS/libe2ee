//
//  test_element.hpp
//  AFGHCryptoTest
//
//  Created by Jan Starke on 29.10.18.
//  Copyright © 2018 T-Systems. All rights reserved.
//

#ifndef test_element_hpp
#define test_element_hpp

#include "gtest/gtest.h"
#include <e2ee/objects/GlobalParameters.hpp>
#include <memory>

class ElementTest : public ::testing::Test {
protected:
  void SetUp() {
    global = std::make_shared<e2ee::GlobalParameters>(160, 512);
  }
  
  std::shared_ptr<e2ee::GlobalParameters> global;
};

#endif /* test_element_hpp */
