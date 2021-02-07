//
//  test_element.cpp
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
#include <e2ee/objects/CurveField.hpp>

template<class T>
void testJsonExport(std::shared_ptr<T> obj1, bool show = false) {

  static_assert(std::is_base_of<e2ee::PbcComparable<T>, T>::value);
  /*
   * keep shared_ptr, because otherwise clear() will get rid of obj1
   */

  std::string json = obj1->exportJson();
  if (show) {
    std::cout << json << std::endl;
  }
  std::shared_ptr<e2ee::PbcContext> catalog = e2ee::PbcContext::createInstance();
  catalog->clear();
  catalog->populate(json);
  auto obj2 = catalog->root();

  if (show) {
    std::cout << obj2->exportJson() << std::endl;
  }

  REQUIRE(*obj1 == dynamic_cast<const T &>(*obj2));
}

TEST_CASE("test element serialization", "[conversion][json]") {

  auto context = e2ee::PbcContext::createInstance();
  auto global = std::make_shared<e2ee::GlobalParameters>(context, 160, 512);
  auto dummy = std::make_unique<e2ee::KeyPair>(global);

  SECTION("TestElementIdUnambiguity") {
    using namespace boost::uuids;
    const uuid &id = global->g()->getId();
    const uuid &id2 = e2ee::PbcObjectImpl<struct element_s>::idOf(global->g()->get());
    REQUIRE(id == id2);
  }

  SECTION("TestGlobalElementg") { testJsonExport(global->g()); }
  SECTION("TestGlobalElementZ") { testJsonExport(global->Z()); }
  SECTION("TestPairingG1") { testJsonExport(global->pairing()->G1()); }
  SECTION("TestPairingG2") { testJsonExport(global->pairing()->G2()); }
  SECTION("TestPairing") { testJsonExport(global->pairing()); }

  SECTION("TestPublicKey") { testJsonExport(dummy->getPublicKey()); }
  SECTION("TestSecretKey") { testJsonExport(dummy->getSecretKey()); }
}

void testByteExport(std::shared_ptr<e2ee::Element> obj1) {
  auto b = obj1->toBytes();
  auto ptr = std::dynamic_pointer_cast<e2ee::PbcSerializableField>(obj1->field());
  REQUIRE(ptr != nullptr);

  auto e = ptr->elementFromBytes(b.begin(), b.end());
  if (!(*e == *obj1)) {
    e = !*e;
  }
  REQUIRE(*e == *obj1);
}

void testByteExport(std::shared_ptr<e2ee::AbstractField> f) {
  testByteExport(f->randomElement());
}

TEST_CASE("test binary conversion of g", "[conversion][json]") {
  auto context = e2ee::PbcContext::createInstance();
  auto global = std::make_shared<e2ee::GlobalParameters>(context, 160, 512);
  auto dummy = std::make_unique<e2ee::KeyPair>(global);

  testByteExport(global->g());
}

TEST_CASE("test binary conversion of pk", "[conversion][json]") {
  auto context = e2ee::PbcContext::createInstance();
  auto global = std::make_shared<e2ee::GlobalParameters>(context, 160, 512);
  auto dummy = std::make_unique<e2ee::KeyPair>(global);

  testByteExport(dummy->getPublicKey());
}

TEST_CASE("test binary conversion of sk", "[conversion][json]") {
  auto context = e2ee::PbcContext::createInstance();
  auto global = std::make_shared<e2ee::GlobalParameters>(context, 160, 512);
  auto dummy = std::make_unique<e2ee::KeyPair>(global);

  testByteExport(dummy->getSecretKey());

  //testByteExport(global->pairing()->G1());
  //testByteExport(global->pairing()->G2());
}

TEST_CASE("test binary conversion of G1", "[conversion][json]") {
  auto context = e2ee::PbcContext::createInstance();
  auto global = std::make_shared<e2ee::GlobalParameters>(context, 160, 512);
  auto dummy = std::make_unique<e2ee::KeyPair>(global);

  SECTION("TestReverseConversionG1") {
    for (auto counter=0; counter<10; ++counter) {
      testByteExport(global->pairing()->G1());
    }
  }
}
