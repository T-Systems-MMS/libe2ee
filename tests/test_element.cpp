//
//  test_element.cpp
//  AFGHCryptoTest
//
//  Created by Jan Starke on 29.10.18.
//  Copyright © 2018 T-Systems. All rights reserved.
//

#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/ObjectCatalog.hpp>
#include <e2ee/objects/KeyPair.hpp>
#include <e2ee/objects/Tuple.hpp>
#include "test_element.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <cstring>
#include <strings.h>
#include <cmath>
#include <boost/uuid/uuid.hpp>


TEST_F(ElementTest, TestElementIdUnambiguity) {
  using namespace boost::uuids;
  const uuid& id = global->g()->getId();
  const uuid& id2 = e2ee::PbcObjectImpl<struct element_s>::idOf(global->g()->get());
  ASSERT_EQ(id, id2);
}

template <class T>
void testJsonExport(std::shared_ptr<T> obj1, bool show=false) {
  std::string json = obj1->exportJson();
  if (show) {
    std::cout << json << std::endl;
  }
  std::shared_ptr<e2ee::ObjectCatalog> catalog = e2ee::ObjectCatalog::getInstance();
  catalog->populate(json);
  std::shared_ptr<e2ee::PbcObject> obj2 = catalog->root();

  if (show) {
    std::cout << obj2->exportJson() << std::endl;
  }
  ASSERT_EQ(*obj1, *obj2);
}

TEST_F(ElementTest, TestGlobalElementg) { testJsonExport(global->g()); }
//TEST_F(ElementTest, TestGlobalElementZ) { testJsonExport(global->Z()); }

TEST_F(ElementTest, TestPairingG1) { testJsonExport(global->pairing()->getG1()); }
TEST_F(ElementTest, TestPairingG2) { testJsonExport(global->pairing()->getG2()); }
TEST_F(ElementTest, TestPairing) { testJsonExport(global->pairing()); }

TEST_F(ElementTest, TestFirstLevelEncryption) {
  auto kp = std::make_unique<e2ee::KeyPair>(global);
  
  auto message1 = std::make_shared<e2ee::Element>(global->pairing()->getGT());
  message1->randomize();
  
  auto ciphertext = std::make_shared<e2ee::Tuple>(message1, kp->getPublicKey(), global, false);
  auto message2 = ciphertext->decryptFirstLevel(kp->getSecretKey());
  ASSERT_EQ(*message1, *message2);
}

TEST_F(ElementTest, TestSecondLevelEncryption) {
  std::shared_ptr<e2ee::KeyPair> sender = std::make_unique<e2ee::KeyPair>(global);
  std::shared_ptr<e2ee::KeyPair> receiver = std::make_unique<e2ee::KeyPair>(global);
  
  std::shared_ptr<e2ee::Element> message1 = std::make_shared<e2ee::Element>(global->pairing()->getGT());
  message1->randomize();
  
  std::shared_ptr<e2ee::Tuple> ciphertext1 = std::make_shared<e2ee::Tuple>(message1, sender->getPublicKey(), global, true);
  std::shared_ptr<e2ee::Element> rk = sender->getReEncryptionKeyFor(receiver->getPublicKey());
  std::shared_ptr<e2ee::Tuple> ciphertext2 = ciphertext1->reEncrypt(rk);
  
  std::shared_ptr<e2ee::Element> message2 = ciphertext2->decryptFirstLevel(receiver->getSecretKey());
  ASSERT_EQ(*message1, *message2);
}

