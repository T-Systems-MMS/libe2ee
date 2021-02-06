//
//  test_conversions.cpp
//  AFGHCryptoTest
//
//  Created by Jan Starke on 12.02.19.
//  Copyright © 2019 T-Systems. All rights reserved.
//

#include <gmp.h>
#include <string>
#include <memory>
#include <catch2/catch.hpp>
#include <e2ee/conversions.hpp>

static std::unique_ptr<std::string>
createRandomNumber(size_t length) {
  auto strObj = std::make_unique<std::string>();
  for(size_t k=0; k<length; ++k) {
    strObj->insert(strObj->end(),
                   e2ee::FORWARD_CONVERSION_MAP[(char)(std::rand()%e2ee::CONVERSION_BASE)]);
  }
  return strObj;
}

TEST_CASE("test conversion between formats", "[conversion]") {

  SECTION("TestConversionMap") {
    for (char c: e2ee::FORWARD_CONVERSION_MAP) {
      REQUIRE(c == e2ee::convert_forward(e2ee::convert_reverse(c)));
    }
  }

  SECTION("TestLimbSerialization1") {
    auto strObj = createRandomNumber(std::rand() % 50 + 10);
    size_t limbs = 0;
    std::unique_ptr<mp_limb_t> tmp = e2ee::str_to_limbs(strObj->cbegin(), strObj->cend(), &limbs);
    auto strObj2 = e2ee::limbs_to_str(tmp.get(), limbs);
    REQUIRE(*strObj == *strObj2);
  }

  SECTION("TestLimbSerialization3") {
    size_t limbs = 1;
    mp_limb_t value = random();
    auto strObj = e2ee::limbs_to_str(&value, limbs);
    auto value2 = e2ee::str_to_limbs(strObj->cbegin(), strObj->cend(), &limbs);
    REQUIRE(value == *value2);
  }

SECTION("TestLimbSerialization2") {
    size_t limbs1 = (std::rand() % 50 + 10) / GMP_NUMB_BITS;
    mp_limb_t lp[limbs1];
    mpn_random(&lp[0], limbs1);
    auto strObj = e2ee::limbs_to_str(&lp[0], limbs1);

    size_t limbs2 = 0;
    auto tmp = e2ee::str_to_limbs(strObj->cbegin(), strObj->cend(), &limbs2);

    REQUIRE(limbs1 == limbs2);
    REQUIRE(mpn_cmp(&lp[0], tmp.get(), limbs2) == 0);
    REQUIRE(mpn_cmp(tmp.get(), &lp[0], limbs2) == 0);
    auto strObj2 = e2ee::limbs_to_str(tmp.get(), limbs2);
    REQUIRE(*strObj == *strObj2);
  }

  SECTION("TestMpzSerialization") {
    auto strObj = createRandomNumber(std::rand() % 50 + 10);
    e2ee::afgh_mpz_t tmp = e2ee::str_to_mpz(strObj->cbegin(), strObj->cend());
    auto strObj2 = e2ee::mpz_to_str(tmp.get());
    REQUIRE(*strObj == strObj2);
  }

  SECTION("TestCrossSerialization1") {
    size_t limbs1 = (std::rand() % 50 + 10) / GMP_NUMB_BITS;
    mp_limb_t lp[limbs1];
    mpn_random(&lp[0], limbs1);
    auto strObjFromLimbs = e2ee::limbs_to_str(&lp[0], limbs1);

    e2ee::afgh_mpz_t tmp = e2ee::str_to_mpz(strObjFromLimbs->cbegin(), strObjFromLimbs->cend());
    REQUIRE(mpn_cmp(&lp[0], mpz_limbs_read(tmp.get()), limbs1) == 0);
  }
}
