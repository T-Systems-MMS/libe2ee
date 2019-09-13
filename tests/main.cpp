//
//  main.cpp
//  AFGHCryptoTest
//
//  Created by Jan Starke on 29.10.18.
//  Copyright © 2018 T-Systems. All rights reserved.
//

#include <iostream>
#include <aixlog.hpp>
#include "gtest/gtest.h"

int main(int argc, char * argv[]) {
  AixLog::Log::init<AixLog::SinkCout>(
          AixLog::Severity::info,
          AixLog::Type::normal);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
