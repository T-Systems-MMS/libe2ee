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

#ifndef afgh_pbc_wrapper_hpp
#define afgh_pbc_wrapper_hpp

#include <e2ee/errors.cpp>
#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/PbcContext.hpp>
#include <pbc.h>
#include <string>
#include <memory>
#include <map>
#include <cstdio>
#include <array>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace e2ee {
class PbcContext;

template<class T>
class PbcObjectImpl : public virtual PbcObject {
 public:
  explicit PbcObjectImpl(const T *wrappedObject) :
          wrappedObject(wrappedObject) {
    const auto id = idOf(wrappedObject);
    if (getId().is_nil()) {
      setId((id.is_nil()) ? (idOf(wrappedObject)) : (id));
    }

    setNativeId(idOf(wrappedObject));
  }

  virtual ~PbcObjectImpl() {}

  T *get() throw() { return const_cast<T *>(wrappedObject); }

  const T *get() const throw() { return wrappedObject; }

  template<class E>
  std::shared_ptr<E> getObject(const boost::uuids::uuid &id, bool requireFinal = false);

  inline std::shared_ptr<AbstractField> fieldFromJson(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const JsonKey &key, bool requireFinal = false) {
    auto jobj = getJsonObject();
    return lockedContext()->getFieldFromJson(values, *jobj, key, requireFinal);
  }
  inline std::shared_ptr<Element> elementFromJson(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const JsonKey &key, bool requireFinal = false) {
    auto jobj = getJsonObject();
    return lockedContext()->getElementFromJson(values, *jobj, key, requireFinal);
  }
  inline std::shared_ptr<Pairing> pairingFromJson(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const JsonKey &key, bool requireFinal = false) {
    auto jobj = getJsonObject();
    return lockedContext()->getPairingFromJson(values, *jobj, key, requireFinal);
  }

 protected:
  void set(T *w) throw() {
    wrappedObject = w;
    const auto id = idOf(wrappedObject);
    if (getId().is_nil()) {
      setId(id);
    }

    setNativeId(id);
  }

 private:
  const T *wrappedObject;
};

template<class T>
template<class E>
std::shared_ptr<E> PbcObjectImpl<T>::getObject(const boost::uuids::uuid &id,
                                               bool requireFinal) {
  if (getContext() == nullptr) {
    return nullptr;
  } else {
    return std::dynamic_pointer_cast<E>(getContext()->getObject(id, requireFinal));
  }
}
/*
template<class T>
template<class E>
object_ref<E> PbcObjectImpl<T>::getObjectFromJson(const JsonKey &key,
                                                       bool requireFinal) {
  afgh_check(hasJsonObject(), "error in internal data types");
  return std::dynamic_pointer_cast<E>(
          lockedContext()->getObjectFromJson(getJsonObject(), key, requireFinal));

  afgh_throw_line("no object context available");
}
 */
}  // namespace e2ee
#endif /* afgh_pbc_wrapper_hpp */
