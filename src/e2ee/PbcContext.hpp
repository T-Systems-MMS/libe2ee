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

typedef std::shared_ptr<PbcContext> context_ptr;

using boost::uuids::uuid;
typedef std::function<std::shared_ptr<PbcObject>(
        std::shared_ptr<PbcContext> context,
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
        const rapidjson::Value &value,
        const uuid &id)> parser_t;

class PbcContext : public std::enable_shared_from_this<PbcContext> {
 public:
  inline void populate(const std::string &json) {parseJson(json);}

  void clear() {
    objects.clear();
  }

  /* enforce creation of a shared_ptr */
  inline static context_ptr createInstance() {
    return context_ptr(new PbcContext());
  }

  bool hasObject(const uuid &id) const;
  void addObject(const std::shared_ptr<PbcObject> &obj);
  void addNativeObject(const uuid &id, PbcObject *obj);

  std::shared_ptr<PbcObject> at(const uuid &id);
  std::shared_ptr<PbcObject> operator[](const uuid &id);
  std::shared_ptr<PbcObject> root() { return at(rootId); }

  std::shared_ptr<Element> element(const uuid &id);
  std::shared_ptr<AbstractField> field  (const uuid &id);
  std::shared_ptr<Pairing> pairing(const uuid &id);

  std::shared_ptr<AbstractField> fromNative(const field_s *ptr, const uuid &id);
  std::shared_ptr<Element> fromNative(const element_s *ptr, const uuid &id);
  std::shared_ptr<Pairing> fromNative(const pairing_s *ptr, const uuid &id);

  inline std::shared_ptr<AbstractField> fromNative(const field_s *ptr) {
    return fromNative(ptr, PbcObject::idOf(ptr));
  }
  inline std::shared_ptr<Element> fromNative(const element_s *ptr) {
    return fromNative(ptr, PbcObject::idOf(ptr));
  }
  inline std::shared_ptr<Pairing> fromNative(const pairing_s *ptr) {
    return fromNative(ptr, PbcObject::idOf(ptr));
  }

  /*
   * implemeted in AbstractField.cpp
   */
  std::shared_ptr<AbstractField> getFieldFromJson(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const rapidjson::Value &value,
          const JsonKey &key,
          bool requireFinal = false);

  /*
   * implemented in Element.cpp
   */
  std::shared_ptr<Element> getElementFromJson(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const rapidjson::Value &value,
          const JsonKey &key,
          bool requireFinal = false);

  /*
   * implemented in Pairing.cpp
   */
  std::shared_ptr<Pairing> getPairingFromJson(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const rapidjson::Value &value,
          const JsonKey &key,
          bool requireFinal = false);

  rapidjson::Document parseJson(const std::string &str);

  void constructObject(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const uuid &id,
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
                          const uuid &,
                          const rapidjson::Value &>::value,
                  "type is not constructible with the required arguments");

    std::string_view type = (T::SUBTYPE.empty()) ?
            (T::TYPE) : (T::SUBTYPE);

    auto fct = [](std::shared_ptr<PbcContext> context,
                  const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
                  const rapidjson::Value &value, const uuid &id) {
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


  uuid getIdFromJson(const rapidjson::Value &value, const JsonKey &key) const;

  /*
   * allocators
   */
 private:
  template <class RESULT, class B, class T, class ...Args>
  RESULT createObject(Args... args) {
    static_assert(std::is_base_of<B, T>::value || std::is_same<B, T>::value);
    static_assert(std::is_base_of<std::shared_ptr<B>, RESULT>::value);
    auto obj = std::make_shared<T>(shared_from_this(), args...);
    addObject(obj);
    return obj;
  }

 public:
  template <class ...Args>
  std::shared_ptr<Pairing> createPairing(Args... args) {
    return createObject<std::shared_ptr<Pairing>, Pairing, Pairing>(args...);
  }

  template <class ...Args>
  std::shared_ptr<Element> createElement(Args... args) {
    return createObject<std::shared_ptr<Element>, Element, Element>(args...);
  }

  template <class T, class ...Args>
  std::shared_ptr<AbstractField> createField(Args... args) {
    return createObject<std::shared_ptr<AbstractField>, AbstractField, T>(args...);
  }

 private:
  std::map<uuid, std::shared_ptr<PbcObject>> objects;
  // this list will not be cleared on deconstruction
  std::map<uuid, PbcObject*> nativeObjects;
  std::map<std::string_view, parser_t> parsers;
  uuid rootId;
  static const std::map<std::string_view, std::shared_ptr<FieldFactory>>
  constructors;

  std::shared_ptr<PbcObject>
  getObjectFromJson(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const rapidjson::Value &value,
          const JsonKey &key,
          bool requireFinal);
};
}  // namespace e2ee

#endif  // SRC_E2EE_PBCCONTEXT_HPP_
