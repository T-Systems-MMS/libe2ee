/*
 * Copyright 2018-2019 T-Systems Multimedia Solutions GmbH
 *
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

#include <iostream>
#include <memory>
#include <map>
#include <e2ee/json.hpp>
#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/CurveField.hpp>
#include <e2ee/objects/MontFPField.hpp>
#include <e2ee/objects/QuadraticField.hpp>
#include <e2ee/objects/MultiplicativeSubgroup.hpp>
#include <e2ee/objects/Pairing.hpp>

namespace e2ee {

void
AbstractField::addToJson(Document& doc) const {
  assert(isFinal());
  if (documentContainsThis(doc)) {
    return;
  }

  auto& self = getJsonStub(doc);
  self.AddMember(KEY_ORDER,
                 mpz_to_json(get()->order, doc.GetAllocator()).Move(),
                 doc.GetAllocator());

  if (get()->pairing != NULL) {
    auto pairing = lockedContext()->fromNative(get()->pairing);
    addJsonObject(doc, self, KEY_PAIRING, pairing);
  }
}

field_ptr
AbstractField::parse_native(const rapidjson::Value& jobj) {
  auto field = allocate_unmanaged<struct field_s>();

  field_init(field);
  const auto type =       JSON_GET_STRING(jobj, KEY_TYPE);
  const auto subtype =    JSON_GET_STRING(jobj, KEY_SUBTYPE);
  auto order = str_to_mpz(JSON_GET_STRING(jobj, KEY_ORDER));
  mpz_set(field->order, order.get());
  assert(!mpz_is0(&(field->order[0])));
  field->name = strdup(subtype.c_str());

  return field;
}

bool AbstractField::equals(const AbstractField &other) const {
  if (0 != strcmp(get()->name, other.get()->name)) { return false; }
  return (0 == mpz_cmp(get()->order, other.get()->order));
}

bool
AbstractField::compareField(
        field_cptr aptr,
        field_cptr bptr,
        std::weak_ptr<PbcContext> context) {
  auto ma = const_cast<field_ptr>(aptr);
  auto mb = const_cast<field_ptr>(bptr);

  if (aptr == nullptr || bptr == nullptr) { return -1; }

  const std::string_view a(aptr->name);
  const std::string_view b(bptr->name);
  if (a != b) { return false; }

  if (auto ctx = context.lock()) {
    return *(ctx->fromNative(aptr)) == *(ctx->fromNative(bptr));
  } else {
    return false;
  }
}

std::shared_ptr<AbstractField> PbcContext::getFieldFromJson(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
        const rapidjson::Value &value,
        const JsonKey &key, bool requireFinal) {
  return e2ee::dynamic_pointer_cast<AbstractField>
          (getObjectFromJson(values, value, key, requireFinal));
}

}  // namespace e2ee
