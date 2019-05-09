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

#ifndef afgh_pbc_wrapper_hpp
#define afgh_pbc_wrapper_hpp

#include <e2ee/errors.cpp>
#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/ObjectCatalog.hpp>
#include <pbc.h>
#include <json-c/json_object.h>
#include <string>
#include <memory>
#include <map>
#include <cstdio>
#include <array>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace e2ee {
  
  template <class T>
  class PbcObjectImpl : public PbcObject {
  public:
    PbcObjectImpl(const std::string& type,
                  const std::string& subtype,
                  bool isFinal,
                  T* wrappedObject)
    : PbcObject(idOf(wrappedObject), type, subtype, isFinal),
      wrappedObject(wrappedObject), _nativeId(idOf(wrappedObject))
      { }
    
    PbcObjectImpl(const std::string& type,
                  const std::string& subtype,
                  bool isFinal,
                  T* wrappedObject,
                  const boost::uuids::uuid& id)
    : PbcObject(((id == boost::uuids::nil_uuid()) ? (idOf(wrappedObject)) : (id)), type, subtype, isFinal),
    wrappedObject(wrappedObject), _nativeId(idOf(wrappedObject))
    { }
    
    virtual ~PbcObjectImpl() { }

    T* get() throw() { return wrappedObject; }
    const T* get() const throw() { return wrappedObject; }
    
    template <class E>
    std::shared_ptr<E> getObject(const boost::uuids::uuid& id, bool requireFinal = false);
    
    template <class E>
    std::shared_ptr<E> getObjectFromJson(const JsonKey& key, bool requireFinal = false);

    const boost::uuids::uuid& nativeId() const noexcept override { return _nativeId; }
  protected:
    void set(T* w) throw() { wrappedObject = w; _nativeId = idOf(w); }
    
  private:
    mutable
    T* wrappedObject;

    boost::uuids::uuid _nativeId;
  };
  
  template <class T>
  template <class E>
  std::shared_ptr<E> PbcObjectImpl<T>::getObject(const boost::uuids::uuid& id,
                               bool requireFinal) {
    if (getObjectCatalog() == nullptr) {
      return nullptr;
    } else {
      return std::dynamic_pointer_cast<E>(getObjectCatalog()->getObject(id, requireFinal));
    }
  }
  
  template <class T>
  template <class E>
  std::shared_ptr<E> PbcObjectImpl<T>::getObjectFromJson(const JsonKey& key,
                                       bool requireFinal) {
    if (getJsonObject() != nullptr) {
      if (auto catalog = getObjectCatalog().lock()) {
        return std::dynamic_pointer_cast<E>(
                catalog->getObjectFromJson(getJsonObject(), key, requireFinal));
      }
    }
    afgh_throw_line("error in internal data types");
  }
}
#endif /* afgh_pbc_wrapper_hpp */
