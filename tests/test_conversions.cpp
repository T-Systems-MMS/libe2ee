//
//  test_conversions.cpp
//  AFGHCryptoTest
//
//  Created by Jan Starke on 12.02.19.
//  Copyright © 2019 T-Systems. All rights reserved.
//

#include "test_conversions.hpp"
#include "gtest/gtest.h"
#include <afgh/conversions.hpp>
#include <gmp.h>
#include <cmath>
#include <string>
#include <memory>

static std::unique_ptr<std::string>
createRandomNumber(size_t length) {
  auto strObj = std::make_unique<std::string>();
  for(size_t k=0; k<length; ++k) {
    strObj->insert(strObj->end(),
                   afgh::FORWARD_CONVERSION_MAP[(char)(std::rand()%afgh::CONVERSION_BASE)]);
  }
  return strObj;
}


TEST(SerializationTest, TestConversionMap) {
  for (char c: afgh::FORWARD_CONVERSION_MAP) {
    ASSERT_EQ(c, afgh::convert_forward(afgh::convert_reverse(c)));
  }
}

TEST(SerializationTest, TestLimbSerialization1) {
  auto strObj = createRandomNumber(std::rand() % 50 + 10);
  size_t limbs = 0;
  mp_limb_t* tmp = afgh::str_to_limbs(strObj->cbegin(), strObj->cend(), &limbs);
  auto strObj2 = afgh::limbs_to_str(tmp, limbs);
  ASSERT_EQ(*strObj, *strObj2);
}

TEST(SerializationTest, TestLimbSerialization3) {
  size_t limbs = 1;
  mp_limb_t value = random();
  auto strObj = afgh::limbs_to_str(&value, limbs);
  mp_limb_t* value2 = afgh::str_to_limbs(strObj->cbegin(), strObj->cend(), &limbs);
  ASSERT_EQ(value, *value2);
}

TEST(SerializationTest, TestLimbSerialization2) {
  size_t limbs1 = (std::rand() % 50 + 10) / GMP_NUMB_BITS;
  mp_limb_t lp[limbs1];
  mpn_random(&lp[0], limbs1);
  auto strObj = afgh::limbs_to_str(&lp[0], limbs1);
  
  size_t limbs2 = 0;
  mp_limb_t* tmp = afgh::str_to_limbs(strObj->cbegin(), strObj->cend(), &limbs2);
  
  ASSERT_EQ(limbs1, limbs2);
  ASSERT_EQ(mpn_cmp(&lp[0], tmp, limbs2), 0);
  ASSERT_EQ(mpn_cmp(tmp, &lp[0], limbs2), 0);
  auto strObj2 = afgh::limbs_to_str(tmp, limbs2);
  free(tmp);
  ASSERT_EQ(*strObj, *strObj2);
}

TEST(SerializationTest, TestMpzSerialization) {
  auto strObj = createRandomNumber(std::rand() % 50 + 10);
  afgh::afgh_mpz_t tmp = afgh::str_to_mpz(strObj->cbegin(), strObj->cend());
  auto strObj2 = afgh::mpz_to_str(tmp.get());
  ASSERT_EQ(*strObj, *strObj2);
}

TEST(SerializationTest, TestCrossSerialization1) {
  size_t limbs1 = (std::rand() % 50 + 10) / GMP_NUMB_BITS;
  mp_limb_t lp[limbs1];
  mpn_random(&lp[0], limbs1);
  auto strObjFromLimbs = afgh::limbs_to_str(&lp[0], limbs1);
  
  afgh::afgh_mpz_t tmp = afgh::str_to_mpz(strObjFromLimbs->cbegin(), strObjFromLimbs->cend());
  ASSERT_EQ(mpn_cmp(&lp[0], mpz_limbs_read(tmp.get()), limbs1), 0);
}
