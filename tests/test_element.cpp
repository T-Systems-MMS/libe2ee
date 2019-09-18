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
