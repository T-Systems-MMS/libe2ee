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

#ifndef SRC_E2EE_PBCCONTEXT_HPP_
#define SRC_E2EE_PBCCONTEXT_HPP_
#include <rapidjson/document.h>
#include <map>
#include <string>
#include <string_view>
#include <memory>
#include <functional>
#include <utility>
#include <boost/uuid/uuid.hpp>
#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/memory.hpp>
#include <e2ee/conversions.hpp>
#include <e2ee/json.hpp>
#include <e2ee/objects/FieldFactory.hpp>

namespace e2ee {

class Element;
class Pairing;
class AbstractField;
class PbcObject;
class PbcContext;
class GlobalParameters;

typedef std::shared_ptr<PbcContext> context_ptr;

typedef std::function<std::shared_ptr<PbcObject>(
        std::shared_ptr<PbcContext> context,
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
        const rapidjson::Value &value,
        const boost::uuids::uuid &id)> parser_t;

class PbcContext {
 public:
  inline std::shared_ptr<PbcObject> populate(const std::string &json) {return parseJson(json);}

  void clear() {
    objects.clear();
  }

  /** enforce creation of a shared_ptr */
  static context_ptr createInstance();

  bool hasObject(const boost::uuids::uuid &id) const;
  void addObject(const std::shared_ptr<PbcObject> &obj);
  void addNativeObject(const boost::uuids::uuid &id, PbcObject *obj);

  std::shared_ptr<PbcObject> at(const boost::uuids::uuid &id);
  std::shared_ptr<PbcObject> operator[](const boost::uuids::uuid &id);
  std::shared_ptr<PbcObject> root() { return at(rootId); }

  std::shared_ptr<Element> element(const boost::uuids::uuid &id);
  std::shared_ptr<AbstractField> field  (const boost::uuids::uuid &id);
  std::shared_ptr<Pairing> pairing(const boost::uuids::uuid &id);

  std::shared_ptr<AbstractField> fromNative(const field_s *ptr, const boost::uuids::uuid &id);
  std::shared_ptr<Element> fromNative(const element_s *ptr, const boost::uuids::uuid &id);
  std::shared_ptr<Pairing> fromNative(const pairing_s *ptr, const boost::uuids::uuid &id);

  inline std::shared_ptr<AbstractField> fromNative(const field_s *ptr) {
    return fromNative(ptr, PbcObject::idOf(ptr));
  }
  inline std::shared_ptr<Element> fromNative(const element_s *ptr) {
    return fromNative(ptr, PbcObject::idOf(ptr));
  }
  inline std::shared_ptr<Pairing> fromNative(const pairing_s *ptr) {
    return fromNative(ptr, PbcObject::idOf(ptr));
  }

  inline std::shared_ptr<e2ee::GlobalParameters> global() const {
    return _global;
  }

  /*
   * implemeted in AbstractField.cpp
   */
  std::shared_ptr<AbstractField> getFieldFromJson(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const rapidjson::Value &value,
          const e2ee::JsonKey &key,
          bool requireFinal = false);

  /*
   * implemented in Element.cpp
   */
  std::shared_ptr<Element> getElementFromJson(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const rapidjson::Value &value,
          const e2ee::JsonKey &key,
          bool requireFinal = false);

  /*
   * implemented in Pairing.cpp
   */
  std::shared_ptr<Pairing> getPairingFromJson(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const rapidjson::Value &value,
          const e2ee::JsonKey &key,
          bool requireFinal = false);

  std::shared_ptr<PbcObject> parseJson(const std::string &str);

  void constructObject(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const boost::uuids::uuid &id,
          std::shared_ptr<rapidjson::Value> value);

  void finalizeObjects(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values);

  template<class T>
  void addParser() {
    static_assert(std::is_base_of<PbcObject, T>::value,
                  "invalid type argument");

    static_assert(std::is_constructible<T,
                          std::shared_ptr<PbcContext>,
    const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>&,
                          const boost::uuids::uuid &,
                          const rapidjson::Value &>::value,
                  "type is not constructible with the required arguments");

    std::string_view type = (T::SUBTYPE.empty()) ?
            (T::TYPE) : (T::SUBTYPE);

    auto fct = [](std::shared_ptr<PbcContext> context,
                  const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
                  const rapidjson::Value &value, const boost::uuids::uuid &id) {
      return std::make_shared<T>(context, values, id, value);
    };

    if (parsers.find(type) == parsers.end()) {
      parsers.insert(std::make_pair(type, fct));
    } else {
      parsers[type] = fct;
    }
  }

  PbcContext();
  PbcContext(const PbcContext &) = delete;
  PbcContext(PbcContext &&) = delete;
  PbcContext &operator=(const PbcContext &) = delete;
  PbcContext &operator=(PbcContext &&) = delete;


  boost::uuids::uuid getIdFromJson(const rapidjson::Value &value, const e2ee::JsonKey &key) const;

 public:
  std::shared_ptr<Pairing> createPairing(int32_t rBits, int32_t qBits);

  std::shared_ptr<Pairing> createPairing(const boost::uuids::uuid &id,
                                         const pairing_s* pairing, bool isFinal);

  std::shared_ptr<Element> createElement(const boost::uuids::uuid &id,
                                         element_ptr element, bool isFinal);

 private:
  std::weak_ptr<PbcContext> self;
  std::map<boost::uuids::uuid, std::shared_ptr<PbcObject>> objects;
  // this list will not be cleared on deconstruction
  std::map<boost::uuids::uuid, PbcObject*> nativeObjects;
  std::map<std::string_view, parser_t> parsers;
  boost::uuids::uuid rootId;

  std::shared_ptr<e2ee::GlobalParameters> _global;

  static const std::map<std::string_view, std::shared_ptr<FieldFactory>>
  constructors;

  std::shared_ptr<PbcObject>
  getObjectFromJson(
          const std::map<boost::uuids::uuid,
          std::shared_ptr<rapidjson::Value>>& values,
          const rapidjson::Value &value,
          const JsonKey &key,
          bool requireFinal);
};
}  // namespace e2ee

#endif  // SRC_E2EE_PBCCONTEXT_HPP_
