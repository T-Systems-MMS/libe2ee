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

#include <map>
#include <utility>
#include <algorithm>
#include <e2ee/objects/MultiplicativeSubgroup.hpp>
#include <e2ee/objects/SubField.hpp>
#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/JsonKey.hpp>
#include <e2ee/PbcContext.hpp>

namespace e2ee {

MultiplicativeSubgroup::MultiplicativeSubgroup(
        std::shared_ptr<PbcContext> context,
        const boost::uuids::uuid &id,
        const field_s *field,
        bool isFinal)
        : PbcObject(context, id, isFinal), SubField(context, field) {
}

void MultiplicativeSubgroup::updateMembers() {
  /*
  if (get()->pairing) {
    pairing = lockedContext()->fromNative(get()->pairing);
  }
   */
}

void MultiplicativeSubgroup::addToJson(Document& doc) const {
  assert(isFinal());
  if(documentContainsThis(doc)) {
    return;
  }
  SubField<MultiplicativeSubgroup>::addToJson(doc);
  auto& self = getJsonStub(doc);

  if (has_pairing()) {
    addJsonObject(doc, self, KEY_PAIRING, pairing());
  }
}

percent_t MultiplicativeSubgroup::finalize(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>> &values) {
  if (isFinal()) { return 100; }

  percent_t status = SubField<MultiplicativeSubgroup>::finalize(values);

  if (status < 100) {
    isFinal(false);
    return (status >> 1);
  }

  if(!has_pairing()) {
    set_pairing(pairingFromJson(values, KEY_PAIRING, false));
    assert(has_pairing());
  }
  status = 60;

  if (! pairing()->has_GT()) {
    /*
     * this field will be initialized by its pairing, when the pairing
     * is final. this is the reason why we don't call finalize on
     * the pairing here: we wait for the pairing to be final, than we
     * continue
     */
    isFinal(false);
    return status;
  }

  // assume that the pairing has a GT field
  assert(pairing()->has_GT());

  // assume that the current object is the pairing's GT field
  assert(pairing()->GT()->getId() == this->getId());

  status = 70;

  if (pairing()->get()->GT->init == NULL) {
    isFinal(false);
    return status;
  }
  get()[0] = pairing()->get()->GT[0];

  isFinal(true);
  if (isFinal()) {
    assert(get()->init != NULL);
  }

  // the superfield's superfield may not be final, so we check this here
  if (isFinal()) {
    return 100;
  } else {
    return 90;
  }
}

}
