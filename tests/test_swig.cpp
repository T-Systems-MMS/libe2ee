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
#include <catch.hpp>
#include <e2ee/e2ee.hpp>
#include <e2ee/objects/KeyPair.hpp>

TEST_CASE("SWIG: test conversion between formats", "[conversion]") {

  SECTION("TestFirstLevelEncryption") {
    auto global = e2ee::createGlobal(160, 512);
    auto kp = e2ee::createKeyPair(global);
    auto dek = e2ee::generateDataEncryptionKey(global);
    //auto aesKey = e2ee::kdf(dek, 256/8);

    auto ciphertext = e2ee::encryptFirstLevel(kp->getPublicKey(), dek);

    //auto message2 = e2ee::decryptFirstLevel(kp->getSecretKey(), ciphertext);
    //REQUIRE(message1 == message2);
  }
}
