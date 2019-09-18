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

#include <pbc.h>
#include <gmp.h>
#include <iostream>
#include <sstream>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/PbcContext.hpp>
#include <e2ee/errors.hpp>
#include <e2ee/conversions.hpp>

namespace e2ee {

boost::uuids::random_generator PbcObject::gen;

boost::uuids::uuid
PbcObject::idOf(const void *item) {
  static boost::uuids::name_generator_latest gen(boost::uuids::ns::url());
  std::stringstream ss;
  ss << "urn:address:" << std::hex << item;
  return gen(ss.str());
}

std::string PbcObject::exportJson() const {
  Document doc;
  doc.SetObject();

  Value root;
  root.SetObject();

  Value id, type;
  id.SetString(getIdString().c_str(), doc.GetAllocator());
  type.SetString(getType().c_str(), doc.GetAllocator());
  root.AddMember(KEY_ID, id, doc.GetAllocator());
  root.AddMember(KEY_TYPE, type, doc.GetAllocator());
  doc.AddMember(KEY_ROOT, root, doc.GetAllocator());

  Value self;
  self.SetObject();
  this->addToJson(doc);

  rapidjson::StringBuffer s;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
  doc.Accept(writer);
  return s.GetString();
}

Value& PbcObject::getJsonStub(Document& doc) const {
  if (!documentContainsThis(doc)) {
    Value value;
    value.SetObject();
    value.AddMember(
            KEY_TYPE,
            Value(getType().c_str(), doc.GetAllocator()).Move(),
            doc.GetAllocator());
    if (!getSubtype().empty()) {
      value.AddMember(
              KEY_SUBTYPE,
              Value(getSubtype().c_str(), doc.GetAllocator()).Move(),
              doc.GetAllocator());
    }
    value.AddMember(
            KEY_ID,
            Value(getIdString().c_str(), doc.GetAllocator()).Move(),
            doc.GetAllocator());

    doc.AddMember(
            Value(getIdString().c_str(), doc.GetAllocator()).Move(),
            value,
            doc.GetAllocator());
  }
  return doc[getIdString().c_str()];
}

void PbcObject::addJsonObject(Document& doc, Value& dst,
        const JsonKey& key, const std::shared_ptr<PbcObject>& object) const {
  object->addToJson(doc);
  dst.AddMember(key,
          Value(object->getIdString().c_str(), doc.GetAllocator()).Move(),
          doc.GetAllocator());
}

Value PbcObject::mpz_to_json(const mpz_t number,
        rapidjson::MemoryPoolAllocator<>& allocator) {
  auto str = mpz_to_str(number);
  Value value;
  value.SetString(str.c_str(), str.size(), allocator);
  return value;
}

Value PbcObject::limbs_to_json(const mp_limb_t *limbs, mp_size_t size,
        rapidjson::MemoryPoolAllocator<>& allocator) {
  auto str = limbs_to_str(limbs, size);
  Value value;
  value.SetString(str->c_str(), str->size(), allocator);
  return value;
}

void PbcObject::setId(const std::string &idstr) {
  static boost::uuids::string_generator gen;
  this->id = gen(idstr);

  if (nativeId.is_nil()) {
    nativeId = id;
  }
}

void PbcObject::setId(const boost::uuids::uuid &id) {
  this->id = id;
  if (nativeId.is_nil()) {
    nativeId = id;
  }
}
}
