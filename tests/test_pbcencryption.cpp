//
//  test_pbcencryption.cpp
//  AFGHCryptoTest
//
//  Created by Jan Starke on 29.10.18.
//  Copyright © 2018 T-Systems. All rights reserved.
//

#include <string>
#include <iostream>
#include <cmath>
#include <catch.hpp>
#include <boost/uuid/uuid.hpp>
#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/PbcContext.hpp>
#include <e2ee/objects/KeyPair.hpp>
#include <e2ee/objects/Tuple.hpp>

TEST_CASE("test element encryption", "[conversion][json]") {

  auto context = e2ee::PbcContext::createInstance();
  auto global = std::make_shared<e2ee::GlobalParameters>(context, 160, 512);

  SECTION("TestFirstLevelEncryption") {
    auto kp = std::make_unique<e2ee::KeyPair>(global);
    auto message1 = std::make_shared<e2ee::Element>(global->lockedContext(), global->pairing()->GT());
    message1->randomize();

    auto ciphertext = std::make_shared<e2ee::Tuple>(
            message1,
            kp->getPublicKey(),
            global,
            false);
    auto message2 = ciphertext->decryptFirstLevel(kp->getSecretKey());
    REQUIRE(*message1 == *message2);
  }

  SECTION("TestSecondLevelEncryption") {
    auto sender = std::make_unique<e2ee::KeyPair>(global);
    auto receiver = std::make_unique<e2ee::KeyPair>(global);

    auto message1 = std::make_shared<e2ee::Element>(global->lockedContext(), global->pairing()->GT());
    message1->randomize();

    auto ciphertext1 = std::make_shared<e2ee::Tuple>(
            message1,
            sender->getPublicKey(),
            global,
            true);
    auto rk = sender->getReEncryptionKeyFor(receiver->getPublicKey());
    auto ciphertext2 = ciphertext1->reEncrypt(rk);

    auto message2 = ciphertext2->decryptFirstLevel(receiver->getSecretKey());
    REQUIRE(*message1 == *message2);
  }
}
