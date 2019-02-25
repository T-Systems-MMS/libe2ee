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

#ifndef ObjectCatalog_hpp
#define ObjectCatalog_hpp
#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/memory.hpp>
#include <e2ee/conversions.hpp>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <boost/uuid/uuid.hpp>

namespace e2ee {
  
  int
  createObjectStub(json_object *jso,
                   int flags,
                   json_object *parent_jso,
                   const char *jso_key,
                   size_t *jso_index,
                   void *userarg);
  
  class ObjectCatalog : public std::enable_shared_from_this<ObjectCatalog> {
  public:
    
    void populate (const std::string& json);
    
    /* enforce creation of a shared_ptr */
    static std::shared_ptr<ObjectCatalog>
    getInstance() {
      return std::shared_ptr<ObjectCatalog>(new ObjectCatalog());
    }
    
    bool
    hasObject(const boost::uuids::uuid& id) const {
      return (objectList.find(id) != objectList.end());
    }
    
    void addObject(std::shared_ptr<PbcObject> obj) {
      assert(! hasObject(obj->getId()));
      objectList[obj->getId()] = obj;
    }
    
    void extracted(const std::shared_ptr<PbcObject> &obj);
    
    void extracted();
    
    json_object* getJsonObject(const std::string& id);
    
    std::shared_ptr<PbcObject>& at(const boost::uuids::uuid& id) { return objectList.at(id); }
    std::shared_ptr<PbcObject>& operator[](const boost::uuids::uuid& id) { return objectList[id]; }
    std::shared_ptr<PbcObject>& root() { return at(rootId); }
    
    std::shared_ptr<PbcObject> getObjectFromJson(json_object* jobj, const JsonKey& key, bool requireFinal = false);
    
    json_object*
    parseJson(const std::string& str);
    void finalizeObjects();
    
    
    typedef std::function<std::shared_ptr<PbcObject>
      (struct json_object*, std::shared_ptr<ObjectCatalog>&, const boost::uuids::uuid& id) > parser_t;
    
    std::map<boost::uuids::uuid, std::shared_ptr<PbcObject> > objectList;
    std::map<std::string, parser_t > parsers;
    
    template <class T>
    void addParser(const std::string& typeId) {
      static_assert(std::is_base_of<PbcObject, T>::value, "invalid type argument");
      parsers[typeId] = T::construct;
    }
    
    static std::string
    get_string_value_by_id(json_object* jobj, const JsonKey& key);
    
  private:
    /*
     * create empty object catalog
     */
    ObjectCatalog()  :
    base(nullptr), /*root(nullptr), */rootId(boost::uuids::nil_uuid()){}
    
    json_object* base;
    //json_object* root;
    boost::uuids::uuid rootId;
  };
  
  bool isValidUuid(const std::string& maybe_uuid,  boost::uuids::uuid& result);
  /*
  template <class T>
  T getNativeObject(json_object* jobj, const JsonKey& key);
  
  template <>
  mpz_ptr
  getNativeObject(json_object* jobj, const JsonKey& key);
  
  template <>
  __mpz_struct
  getNativeObject(json_object* jobj, const JsonKey& key);
  
  template <>
  mp_limb_t*
  getNativeObject(json_object* jobj, const JsonKey& key);
  */
  afgh_mpz_t
  getMpzFromJson(json_object* jobj, const JsonKey& key);
  
  std::unique_ptr<mp_limb_t>
  getLimbsFromJson(json_object* jobj, const JsonKey& key);
  
}

#endif /* ObjectCatalog_hpp */
