//
//  test_element.cpp
//  AFGHCryptoTest
//
//  Created by Jan Starke on 29.10.18.
//  Copyright © 2018 T-Systems. All rights reserved.
//

#include <afgh/objects/Pairing.hpp>
#include <afgh/objects/Element.hpp>
#include <afgh/objects/PbcObjectImpl.hpp>
#include <afgh/ObjectCatalog.hpp>
#include <afgh/objects/KeyPair.hpp>
#include <afgh/objects/Tuple.hpp>
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
  const uuid& id2 = afgh::PbcObjectImpl<struct element_s>::idOf(global->g()->get());
  ASSERT_EQ(id, id2);
}

template <class T>
void testJsonExport(std::shared_ptr<T> obj1) {
  std::string json = obj1->exportJson();
  std::cout << json << std::endl;
  std::shared_ptr<afgh::ObjectCatalog> catalog = afgh::ObjectCatalog::getInstance();
  catalog->populate(json);
  std::shared_ptr<afgh::PbcObject> obj2 = catalog->root();
  
  ASSERT_EQ(*obj1, *obj2);
}

//TEST_F(ElementTest, TestGlobalElementg) { testJsonExport(global->g()); }
TEST_F(ElementTest, TestGlobalElementZ) { testJsonExport(global->Z()); }
/*
 TEST_F(ElementTest, TestPairingG1) { testJsonExport(global->pairing()->getG1()); }
TEST_F(ElementTest, TestPairingG2) { testJsonExport(global->pairing()->getGT()); }
TEST_F(ElementTest, TestPairing) { testJsonExport(global->pairing()); }

TEST_F(ElementTest, TestFirstLevelEncryption) {
  auto kp = std::make_unique<afgh::KeyPair>(global);
  
  auto message1 = std::make_shared<afgh::Element>(global->pairing()->getGT());
  message1->randomize();
  
  auto ciphertext = std::make_shared<afgh::Tuple>(message1, kp->getPublicKey(), global, false);
  auto message2 = ciphertext->decryptFirstLevel(kp->getSecretKey());
  ASSERT_EQ(*message1, *message2);
}

TEST_F(ElementTest, TestSecondLevelEncryption) {
  std::shared_ptr<afgh::KeyPair> sender = std::make_unique<afgh::KeyPair>(global);
  std::shared_ptr<afgh::KeyPair> receiver = std::make_unique<afgh::KeyPair>(global);
  
  std::shared_ptr<afgh::Element> message1 = std::make_shared<afgh::Element>(global->pairing()->getGT());
  message1->randomize();
  
  std::shared_ptr<afgh::Tuple> ciphertext1 = std::make_shared<afgh::Tuple>(message1, sender->getPublicKey(), global, true);
  std::shared_ptr<afgh::Element> rk = sender->getReEncryptionKeyFor(receiver->getPublicKey());
  std::shared_ptr<afgh::Tuple> ciphertext2 = ciphertext1->reEncrypt(rk);
  
  std::shared_ptr<afgh::Element> message2 = ciphertext2->decryptFirstLevel(receiver->getSecretKey());
  ASSERT_EQ(*message1, *message2);
}
*/
