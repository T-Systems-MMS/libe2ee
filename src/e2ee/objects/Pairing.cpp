/*
 * This file is part of libe2ee.
 *
 * libe2ee is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libe2ee is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libe2ee.  If not, see <http://www.gnu.org/licenses/lgpl>.
 */

#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/MontFPField.hpp>
#include <e2ee/ObjectCatalog.hpp>

namespace e2ee {
  
  class MultiplicativeSubgroup;
  
  Pairing::Pairing(int32_t rBits, int32_t qBits, const boost::uuids::uuid id)
  : PbcObjectImpl("pairing", "", false, nullptr, id) {
    pairing_ptr ptr = allocate_unmanaged<pairing_s>();
    struct pbc_param_s params;
    bzero(&params, sizeof(params));
    
    pbc_param_init_a_gen(&params, rBits, qBits);
    
    pairing_init_pbc_param(ptr, &params);
    set(ptr);
    setObjectCatalog(ObjectCatalog::getInstance());
    
    G1 = AbstractField::constructFromNative(ptr->G1, getObjectCatalog());
    G2 = AbstractField::constructFromNative(ptr->G2, getObjectCatalog());
    GT = std::dynamic_pointer_cast<MultiplicativeSubgroup>(AbstractField::constructFromNative(ptr->GT, getObjectCatalog()));
    Zr = std::dynamic_pointer_cast<MontFPField>(AbstractField::constructFromNative(ptr->Zr, getObjectCatalog()));
    
    isFinal(true);
  }
  
  Pairing::~Pairing() {
    pairing_clear(get());
    set(nullptr);
  }
  
  struct json_object*
  Pairing::toJson(json_object* root, bool returnIdOnly) const {
    json_object* jobj = getJsonStub(root, getId());
    if (jobj) { RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly); }
    else      {
      jobj = createJsonStub(root, getId());
      addJsonObject(jobj, KEY_TYPE, json_object_new_string("pairing"));
    }
    addJsonObject(jobj, KEY_G1, G1->toJson(root, true));
    addJsonObject(jobj, KEY_G2, G2->toJson(root, true));
    addJsonObject(jobj, KEY_GT, GT->toJson(root, true));
    addJsonObject(jobj, KEY_Zr, Zr->toJson(root, true));
    addJsonObject(jobj, KEY_r, mpz_to_json(get()->r));
    addJsonObject(jobj, KEY_phikonr, mpz_to_json(get()->phikonr));
    RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly);
  }
  
  std::shared_ptr<Pairing>
  Pairing::construct(struct json_object* jobj, std::shared_ptr<ObjectCatalog>& catalog, const boost::uuids::uuid& id) {
    afgh_mpz_t ptr;
    pairing_ptr pairing = allocate_unmanaged<struct pairing_s>();
    auto newPairing = std::make_shared<Pairing>(512, 160, id);
    newPairing->setObjectCatalog(catalog);
    newPairing->set(pairing);
    
    ptr = getMpzFromJson(jobj, KEY_r);
    mpz_set(pairing->r, ptr.get());
    ptr.reset();
    
    ptr = getMpzFromJson(jobj, KEY_phikonr);
    mpz_set(pairing->phikonr, ptr.get());

    /* make sure finalize() is being called */
    newPairing->isFinal(false);
    newPairing->G1 = nullptr;
    newPairing->G2 = nullptr;
    newPairing->GT = nullptr;
    newPairing->Zr = nullptr;
    return newPairing;
  }
  
  percent_t
  Pairing::finalize() {
    percent_t status = 0;
    assert(!isFinal());
    
    if (! G1) {
      G1 = getObjectFromJson<AbstractField>(KEY_G1);
      get()->G1 = G1->get();
    }
    
    if (! G2) {
      G2 = getObjectFromJson<AbstractField>(KEY_G2);
      get()->G2 = G2->get();
    }
    
    if (! GT) {
      GT = std::dynamic_pointer_cast<MultiplicativeSubgroup>(getObjectFromJson<AbstractField>(KEY_GT));
      get()->GT[0] = GT->get()[0];
    }
    
    if (! Zr) {
      Zr = std::dynamic_pointer_cast<MontFPField>(getObjectFromJson<AbstractField>(KEY_Zr));
      get()->Zr[0] = Zr->get()[0];
    }
    
    if (! isGTinitialized) {
      auto mulg = std::dynamic_pointer_cast<MultiplicativeSubgroup>(GT);
      
      if (! mulg )                         { return 10; }
      if (! mulg->getSuperField() )        { return 20; }
      if (! mulg->getSuperField()->get() ) { return 30; }
      pairing_GT_init(get(), mulg->getSuperField()->get());
      GT->isFinal(false);
      status = std::max(40, GT->finalize());
      if (status == 100) {
        isGTinitialized = true;
        isFinal(true);
      }
    }
    return status;
  }
  
  void
  Pairing::isFinal(bool f) {
    if (f) {
      assert(get() != nullptr);
      assert(get()->G1 != nullptr);
      assert(get()->G2 != nullptr);
    }
    PbcObject::isFinal(f);
  }
  
  bool Pairing::equals(const std::shared_ptr<PbcObject>& other) const {
    FAIL_UNLESS(isFinal());
    std::shared_ptr<Pairing> o = std::dynamic_pointer_cast<Pairing>(other);
    FAIL_UNLESS(o != nullptr);
    FAIL_UNLESS(o->isFinal());
    
    FAIL_UNLESS(G1->equals(o->getG1()));
    FAIL_UNLESS(G1->equals(o->getG2()));
    FAIL_UNLESS(G1->equals(o->getGT()));
    FAIL_UNLESS(G1->equals(o->getZr()));
    FAIL_IF(0 != mpz_cmp(get()->r, o->get()->r));
    FAIL_IF(0 != mpz_cmp(get()->phikonr, o->get()->phikonr));
    return true;
  }
  
  bool Pairing::operator==(const Pairing& other) const {
    assert(isFinal());
    assert(other.PbcObject::isFinal());
    
    FAIL_IF(other.get() == nullptr);
    FAIL_UNLESS(G1->equals(other.getG1()));
    FAIL_UNLESS(G2->equals(other.getG2()));
    FAIL_UNLESS(GT->equals(other.getGT()));
    FAIL_UNLESS(Zr->equals(other.getZr()));
    FAIL_IF(0 != mpz_cmp(get()->r, other.get()->r));
    FAIL_IF(0 != mpz_cmp(get()->phikonr, other.get()->phikonr));
    return true;
  }
  
  std::shared_ptr<Element> Pairing::initG1() {
    element_ptr element = allocate_unmanaged<element_s>();
    element_init_G1(element, get());
    return std::make_shared<Element>(element, getObjectCatalog(), true, Element::idOf(element));
  }
  std::shared_ptr<Element> Pairing::initG2() {
    element_ptr element = allocate_unmanaged<element_s>();
    element_init_G2(element, get());
    return std::make_shared<Element>(element, getObjectCatalog(), true, Element::idOf(element));
  }
  std::shared_ptr<Element> Pairing::initGT() {
    element_ptr element = allocate_unmanaged<element_s>();
    element_init_GT(element, get());
    return std::make_shared<Element>(element, getObjectCatalog(), true, Element::idOf(element));
  }
  std::shared_ptr<Element> Pairing::initZr() {
    element_ptr element = allocate_unmanaged<element_s>();
    element_init_Zr(element, get());
    return std::make_shared<Element>(element, getObjectCatalog(), true, Element::idOf(element));
  }
  std::unique_ptr<Element>
  Pairing::apply(const std::shared_ptr<Element>& e1,
                 const std::shared_ptr<Element>& e2) const {
    element_ptr element = allocate_unmanaged<element_s>();
    element_init_GT(element, const_cast<pairing_ptr>(get()));
    pairing_apply(element,
            const_cast<element_ptr>(e1->get()),
            const_cast<element_ptr>(e2->get()),
            const_cast<pairing_ptr>(get()));
    return std::make_unique<Element>(element, getObjectCatalog(), true, Element::idOf(element));
  }
}
