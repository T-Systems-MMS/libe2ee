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

#include <e2ee/PbcContext.hpp>
#include <memory>
#include <iostream>
#include <utility>
#include <aixlog.hpp>
#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/errors.hpp>
#include <e2ee/json.hpp>
#include <e2ee/uuid.hpp>
#include <e2ee/objects/MontFPField.hpp>
#include <e2ee/objects/CurveField.hpp>
#include <e2ee/objects/MultiplicativeSubgroup.hpp>
#include <e2ee/objects/QuadraticField.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/GlobalParameters.hpp>
#include <boost/uuid/uuid.hpp>

namespace e2ee {

const std::map<std::string_view, std::shared_ptr<FieldFactory>>
        PbcContext::constructors = {
        {CurveField::SUBTYPE,
                std::make_shared<FieldFactoryImpl<CurveField>>()},
        {MontFPField::SUBTYPE,
                std::make_shared<FieldFactoryImpl<MontFPField>>()},
        {QuadraticField::SUBTYPE,
                std::make_shared<FieldFactoryImpl<QuadraticField>>()},
        {MultiplicativeSubgroup::SUBTYPE,
                std::make_shared<FieldFactoryImpl<MultiplicativeSubgroup>>()}};

PbcContext::PbcContext() :
        rootId(boost::uuids::nil_uuid()) {
  addParser<MontFPField>();
  addParser<CurveField>();
  addParser<QuadraticField>();
  addParser<MultiplicativeSubgroup>();
  addParser<Pairing>();
  addParser<Element>();
}

context_ptr PbcContext::createInstance() {
  auto ctx = context_ptr(new PbcContext());
  ctx->self = ctx;
  ctx->_global = std::make_shared<e2ee::GlobalParameters>(ctx, 160, 512);
  return ctx;
}

void PbcContext::constructObject(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
        const boost::uuids::uuid &id,
        std::shared_ptr<rapidjson::Value> value) {
  afgh_check(value->HasMember(KEY_TYPE), "missing 'type' key");
  afgh_check((*value)[KEY_TYPE.c_str()].IsString(), "invalid 'type' key");
  std::string_view type = (*value)[KEY_TYPE.c_str()].GetString();

  if (KEY_FIELD == type) {
    afgh_check(value->HasMember(KEY_SUBTYPE), "missing 'subtype' key");
    afgh_check((*value)[KEY_SUBTYPE.c_str()].IsString(),
               "invalid 'subtype' key");
    type = (*value)[KEY_SUBTYPE.c_str()].GetString();
  }

  const auto parser = parsers.find(type);
  afgh_check(parser != parsers.cend(),
             "missing parser for 'type' key: '%s'", type);

  auto newObject = parser->second(self.lock(), values, *value, id);

  newObject->setJsonObject(value);
  objects.insert(std::make_pair(id, newObject));

  /* parsed objects inherit their ids from the json document,
   * but they must also be resolvable using their native id
   */
  if (id == newObject->getNativeId()) {
    objects.insert(std::make_pair(newObject->getNativeId(), newObject));
  }
}

std::shared_ptr<PbcObject>
PbcContext::parseJson(const std::string &str) {
  rapidjson::Document doc;
  doc.Parse(str.c_str());

  afgh_check(doc.HasMember(KEY_ROOT), "missing 'root' key");
  auto root = doc[KEY_ROOT.c_str()].GetObject();

  afgh_check(root.HasMember(KEY_TYPE), "missing 'type' key");
  afgh_check(root[KEY_TYPE.c_str()].IsString(), "invalid 'type' key");

  rootId = parse_uuid(JSON_GET_STRING(root, KEY_ID));

  /*
   * create mapped list of json values
   */
  std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>> values;
  for (const auto& obj : doc.GetObject()) {
    afgh_check(obj.value.IsObject(), "invalid object");
    boost::uuids::uuid id = parse_uuid(obj.name.GetString(), false);
    if (id == boost::uuids::nil_uuid()) {
      continue;
    }
    values.insert(std::make_pair(id, std::make_shared<rapidjson::Value>(obj.value, doc.GetAllocator())));
  }

  for (const auto& p : values) {
    const auto& id = p.first;
    constructObject(values, id, p.second);

    assert(objects.find(id) != objects.end());
    assert(objects.find(id)->second->getId() == id);
  }

  assert(rootId != boost::uuids::nil_uuid());
  assert(hasObject(rootId));
  finalizeObjects(values);
  return this->at(rootId);
}

std::shared_ptr<PbcObject>
PbcContext::getObjectFromJson(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
        const rapidjson::Value& value,
        const JsonKey &key, bool requireFinal) {
  boost::uuids::uuid id = getIdFromJson(value, key);

  auto iter = objects.find(id);
  if (iter == objects.end()) {
    /*
     * this object does not exist yet, we need to create it from JSON
     */
    auto json_value = values.find(id);
    if (json_value == values.end()) {
      afgh_throw_line("missing object for id '%s'", key.c_str());
    }
    constructObject(values, id, json_value->second);
    iter = objects.find(id);
    assert (iter != objects.end());
  }

  if (requireFinal) {
    if (!iter->second->isFinal()) {
      //afgh_throw_line("no such element");
      iter->second->finalize(values);
      //finalizeObjects(values);
    }
    //assert(iter->second->isFinal());
  }
  return iter->second;
}

boost::uuids::uuid PbcContext::getIdFromJson(const rapidjson::Value &value, const JsonKey &key) const {
  static boost::uuids::string_generator gen;
  const boost::uuids::uuid id = gen(JSON_GET_STRING(value, key));
  return id;
}

void PbcContext::finalizeObjects(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values) {
  bool isFinal;
  percent_t status, old_status = 0;

  do {
    isFinal = true;
    status = 0;
    for (const auto& i : objects) {
      if (i.second->isFinal()) {
        status += 100;
      } else {
        const percent_t new_status = i.second->finalize(values);
        LOG(TRACE) << i.second->getType();
        LOG(TRACE) << COND(! i.second->getSubtype().empty()) << "::" << i.second->getSubtype();
        LOG(TRACE) << "::finalize('"
                  << i.second->getId()
                  << "'/'"
                  << static_cast<PbcObject*>(i.second.get())
                  << "')"
                  << " => " << new_status << "%" << std::endl;
        assert(i.second->isFinal() == (new_status == 100));
        status += new_status;
        isFinal &= i.second->isFinal();
      }
    }
    LOG(DEBUG) << status/objects.size() << "% " << std::endl;

    /*
     * prevent endless loops
     */
    assert(isFinal || status > old_status);
    old_status = status;
  } while (!isFinal);

  assert(status == 100 * objects.size());
  for (const auto& i : objects) {
    assert(i.second->isFinal());
    assert(i.second->isValid());
  }
}

bool PbcContext::hasObject(const boost::uuids::uuid &id) const {
  return ((objects.find(id) != objects.end()) ||
          (nativeObjects.find(id) != nativeObjects.end()));
}

void PbcContext::addObject(const std::shared_ptr<PbcObject> &obj) {
  auto iter = objects.find(obj->getId());
  if (iter == objects.end()) {
    objects.insert(std::make_pair(obj->getId(), obj));
  } else {
    afgh_throw_line("object with id '%s' already exists", obj->getIdString().c_str());
  }
  //std::cout << "added object with id " << obj->getIdString() << std::endl;
  assert(hasObject(obj->getId()));
}

void PbcContext::addNativeObject(const boost::uuids::uuid &id, PbcObject *obj) {
  auto iter = nativeObjects.find(id);
  if (iter == nativeObjects.end()) {
    nativeObjects.insert(std::make_pair(id, obj));
  }
  assert(hasObject(id));
}


std::shared_ptr<PbcObject> PbcContext::at(const boost::uuids::uuid &id) {
  auto iter1 = objects.find(id);
  if (iter1 != objects.end()) {
    return iter1->second;
  }
  throw std::out_of_range(boost::uuids::to_string(id));
}

std::shared_ptr<PbcObject> PbcContext::operator[](const boost::uuids::uuid &id) {
  /*
  auto iter2 = nativeObjects.find(id);
  if (iter2 != nativeObjects.end()) {
    return iter2->second;
  }
*/
  return objects[id];
}

std::shared_ptr<AbstractField> PbcContext::fromNative(const field_s *ptr, const boost::uuids::uuid &id) {
  auto iter = objects.find(id);
  if(iter != objects.end()) {
    return e2ee::dynamic_pointer_cast<AbstractField>(iter->second);
  }

  /*
   * find matching parser for field type
   */
  assert(ptr->name != nullptr);
  auto c = constructors.find(ptr->name);
  if (c == constructors.end()) {
    afgh_throw_line("no constructor found for field type '%s'", ptr->name);
  }

  /*
   * construct object and return its id
   */
  std::shared_ptr<AbstractField> f = (*c->second)(self.lock(), id, ptr, true);
  assert(f != nullptr);
  assert(f->getId() == id);
  addObject(f);
  f->updateMembers();
  return f;
}

std::shared_ptr<Pairing> PbcContext::createPairing(int32_t rBits, int32_t qBits) {
  auto obj = std::make_shared<Pairing>(self.lock(), rBits, qBits);
  addObject(std::dynamic_pointer_cast<PbcObject>(obj));
  return obj;
}

std::shared_ptr<Pairing> PbcContext::createPairing(
        const boost::uuids::uuid &id,
        const pairing_s* pairing,
        bool isFinal) {
  auto obj = std::make_shared<Pairing>(self.lock(), id, pairing, isFinal);
  addObject(std::dynamic_pointer_cast<PbcObject>(obj));
  return obj;
}

std::shared_ptr<Element> PbcContext::createElement(const boost::uuids::uuid &id,
                                       element_ptr element, bool isFinal) {
  auto obj = std::make_shared<Element>(self.lock(), id, element, isFinal);
  addObject(std::dynamic_pointer_cast<PbcObject>(obj));
  return obj;
}

std::shared_ptr<Pairing> PbcContext::fromNative(const pairing_s* obj, const boost::uuids::uuid &id) {
  auto iter = objects.find(id);
  if (iter != objects.end()) {
    return e2ee::dynamic_pointer_cast<Pairing>(iter->second);
  }

  return createPairing(id, obj, true);
}


std::shared_ptr<Element> PbcContext::fromNative(const element_s* obj, const boost::uuids::uuid &id) {
  auto iter = objects.find(id);
  if (iter != objects.end()) {
    return e2ee::dynamic_pointer_cast<Element>(iter->second);
  }

  return createElement(id, const_cast<element_ptr>(obj), true);
}

std::shared_ptr<Element> PbcContext::element(const boost::uuids::uuid &id) {
  return e2ee::dynamic_pointer_cast<Element>(at(id));
}
std::shared_ptr<AbstractField>   PbcContext::field  (const boost::uuids::uuid &id) {
  return e2ee::dynamic_pointer_cast<AbstractField>(at(id));
}
std::shared_ptr<Pairing> PbcContext::pairing(const boost::uuids::uuid &id) {
  return e2ee::dynamic_pointer_cast<Pairing>(at(id));
}
}  // namespace e2ee
