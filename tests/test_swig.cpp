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
    auto context = e2ee::createContext();
    auto global = e2ee::getGlobal(context);

    auto kp = e2ee::createKeyPair(global);
    auto dek = e2ee::generateDataEncryptionKey(global);
    auto aesKey1 = e2ee::kdf256(dek);

    auto ciphertext = e2ee::encryptFirstLevel(kp->getPublicKey(), dek);

    auto decryptedDek = e2ee::decryptFirstLevel(kp->getSecretKey(), ciphertext);
    auto aesKey2 = e2ee::kdf256(decryptedDek);

    REQUIRE_THAT(aesKey2, Catch::Equals<std::byte>(aesKey1));
  }
}
