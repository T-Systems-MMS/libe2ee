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
#include <e2ee/PbcContext.hpp>
#include <iostream>
extern "C" {
#include <pbc.h>
}

#define INIT_FIELD(name, dst, src) do {\
  if (!has_##name()) { \
    set_##name(fieldFromJson(values, KEY_##name)); \
  } \
  status.push_back(has_##name()); \
  \
  if (has_##name()) { \
    if (name()->isFinal()) { \
      (dst) = (src); \
      status.push_back(true); \
    } else { \
      status.push_back(false); \
    } \
  } else { \
    status.push_back(false); \
  } \
  }while(0)

namespace e2ee {

class MultiplicativeSubgroup;

Pairing::Pairing(std::shared_ptr<PbcContext> context, int32_t rBits, int32_t qBits, const boost::uuids::uuid &id)
: PbcObject(context, id, true), PbcObjectImpl(nullptr) {
  pairing_ptr ptr = allocate_unmanaged<pairing_s>();
  struct pbc_param_s params;
  bzero(&params, sizeof(params));

  pbc_param_init_a_gen(&params, rBits, qBits);

  pairing_init_pbc_param(ptr, &params);

  set(ptr);
  if (id == boost::uuids::nil_uuid()) {
    setId(idOf(ptr));
  }

  initFields(context, ptr);
  initMethod = 1;
}

void Pairing::initFields(const std::shared_ptr<PbcContext> &ctx, const pairing_s *ptr) {
  auto pdata = reinterpret_cast<a_pairing_data_ptr>(ptr->data);

  set_G1(ctx->fromNative(ptr->G1));
  set_G2(ctx->fromNative(ptr->G2));
  set_GT(ctx->fromNative(ptr->GT));
  set_Zr(ctx->fromNative(ptr->Zr));
  set_Eq(ctx->fromNative(pdata->Eq));
  set_Fq(ctx->fromNative(pdata->Fq));
  set_Fq2(ctx->fromNative(pdata->Fq2));
}

Pairing::Pairing(std::shared_ptr<PbcContext> context,
                 const boost::uuids::uuid& id, const pairing_s* pairing, bool isFinal):
                 PbcObject(context, ((id == boost::uuids::nil_uuid()) ? (idOf(pairing)) : (id)), isFinal),
                 PbcObjectImpl(pairing){

  initFields(context, pairing);
  initMethod = 2;
}


Pairing::Pairing(std::shared_ptr<PbcContext> context,
                 const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
                 const boost::uuids::uuid &id,
                 const rapidjson::Value &value)
                 : PbcObject(context, id, false), PbcObjectImpl(nullptr){
  //context->addObject(shared_from_this());
  afgh_mpz_t ptr;
  pairing_ptr pairing = allocate_unmanaged<struct pairing_s>();
  struct pbc_param_s params;
  bzero(&params, sizeof(params));
  pbc_param_init_a_gen(&params, 512, 160);
  pairing_init_pbc_param(pairing, &params);
  set(pairing);

  ptr = json_to_mpz(value[KEY_r.c_str()]);
  mpz_set(pairing->r, ptr.get());
  ptr.reset();

  ptr = json_to_mpz(value[KEY_phikonr.c_str()]);
  mpz_set(pairing->phikonr, ptr.get());
  ptr.reset();

  pairing->data = allocate_unmanaged<a_pairing_data>(sizeof(a_pairing_data));

  /* make sure finalize() is being called */
  this->isFinal(false);
  initMethod = 3;
}

Pairing::~Pairing() {
  pairing_clear(get());
  set(nullptr);
}

void Pairing::addToJson(Document& doc) const {
  assert(isFinal());
  if (documentContainsThis(doc)) {
    return;
  }

  auto& self = getJsonStub(doc);
  addJsonObject(doc, self, KEY_G1, G1());
  addJsonObject(doc, self, KEY_G2, G2());
  addJsonObject(doc, self, KEY_GT, GT());
  addJsonObject(doc, self, KEY_Zr, Zr());
  addJsonObject(doc, self, KEY_Eq, Eq());
  addJsonObject(doc, self, KEY_Fq, Fq());
  addJsonObject(doc, self, KEY_Fq2, Fq2());

  self.AddMember(KEY_r,
                 mpz_to_json(get()->r, doc.GetAllocator()).Move(),
                 doc.GetAllocator());
  self.AddMember(KEY_phikonr,
                 mpz_to_json(get()->phikonr, doc.GetAllocator()).Move(),
                 doc.GetAllocator());
}

percent_t
Pairing::finalize(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values) {
  std::vector<bool> status;
  auto pdata = reinterpret_cast<a_pairing_data_ptr>(get()->data);
  assert(!isFinal());
  INIT_FIELD(G1, get()->G1, G1()->get());
  INIT_FIELD(G2, get()->G2, G2()->get());
  INIT_FIELD(Zr, get()->Zr[0], Zr()->get()[0]);
  INIT_FIELD(Eq, pdata->Eq[0], Eq()->get()[0]);
  INIT_FIELD(Fq, pdata->Fq[0], Fq()->get()[0]);
  INIT_FIELD(Fq2, pdata->Fq2[0], Fq2()->get()[0]);

  if (!has_GT()) {
    set_GT(fieldFromJson(values, KEY_GT));
    get()->GT[0] = GT()->get()[0];
  }
  status.push_back(has_GT());

  if (! GT()->isFinal()) {
    auto gt_ptr = GT();
    auto mulg = e2ee::dynamic_pointer_cast<MultiplicativeSubgroup>(gt_ptr);

    if (mulg == nullptr) { return 10; }
    if (!mulg->has_superField()) { return 20; }
    if (!mulg->superField()->get()) { return 30; }

    pairing_GT_init(get(), mulg->superField()->get());
    gt_ptr->finalize(values);
  }
  status.push_back(GT()->isFinal());

  auto successes = std::count(status.cbegin(), status.cend(), true);
  auto result = (successes * 100) / status.size();
  if (result == 100) {
    isFinal(true);
  }
  return result;
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

bool Pairing::equals(const Pairing &other) const {
  FAIL_IF(other.get() == nullptr);
  FAIL_UNLESS(*G1() == *(other.G1()));
  FAIL_UNLESS(*G2() == *(other.G2()));
  FAIL_UNLESS(*GT() == *(other.GT()));
  FAIL_UNLESS(*Zr() == *(other.Zr()));
  FAIL_IF(0 != mpz_cmp(get()->r, other.get()->r));
  FAIL_IF(0 != mpz_cmp(get()->phikonr, other.get()->phikonr));
  return true;
}

std::shared_ptr<Element> Pairing::initG1() {
  element_ptr element = allocate_unmanaged<element_s>();
  element_init_G1(element, get());
  return lockedContext()->fromNative(element);
}

std::shared_ptr<Element> Pairing::initG2() {
  element_ptr element = allocate_unmanaged<element_s>();
  element_init_G2(element, get());
  return lockedContext()->fromNative(element);
}

std::shared_ptr<Element> Pairing::initGT() {
  element_ptr element = allocate_unmanaged<element_s>();
  element_init_GT(element, get());
  return lockedContext()->fromNative(element);
}

std::shared_ptr<Element> Pairing::initZr() {
  element_ptr element = allocate_unmanaged<element_s>();
  element_init_Zr(element, get());
  return lockedContext()->fromNative(element);
}

std::shared_ptr<Element>
Pairing::apply(const std::shared_ptr<Element> &e1,
               const std::shared_ptr<Element> &e2) const {
  element_ptr element = allocate_unmanaged<element_s>();
  element_init_GT(element, const_cast<pairing_ptr>(get()));
  pairing_apply(element,
                const_cast<element_ptr>(e1->get()),
                const_cast<element_ptr>(e2->get()),
                const_cast<pairing_ptr>(get()));
  return lockedContext()->fromNative(element);
}

std::shared_ptr<Pairing> PbcContext::getPairingFromJson(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
        const rapidjson::Value &value,
        const JsonKey &key, bool requireFinal) {
  return e2ee::dynamic_pointer_cast<Pairing>
          (getObjectFromJson(values, value, key, requireFinal));
}

}  // namespace e2ee
