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

#ifndef PbcObject_h
#define PbcObject_h

#include <e2ee/errors.hpp>
#include <e2ee/objects/JsonKey.hpp>
#include <string>
#include <forward_list>
#include <json-c/json_object.h>
#include <functional>
#include <gmp.h>
#include <pbc.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#define RETURN_JSON_OBJECT(obj,id,returnIdOnly) \
return ((returnIdOnly)?(createJsonId(id)):(obj))

#define FAIL_IF(condition) \
if (true == (condition)) { return false; }

#define FAIL_UNLESS(condition) \
if (false == (condition)) { return false; }

#define SUCCEED() do { return true; } while (0)

namespace e2ee {
  class ObjectCatalog;
  
  typedef int percent_t;
  
  class PbcObject {
  public:
    
    
    PbcObject(const boost::uuids::uuid& id,
              const std::string& type,
              const std::string& subtype,
              bool isFinal) :
    _isFinal(isFinal),
    id (id == boost::uuids::nil_uuid() ? gen() : id),
    jobj(nullptr), catalog(nullptr) {
    }
    
    virtual ~PbcObject() {
      if (jobj != nullptr) {
        json_object_put(jobj);
      }
    }
    
    virtual bool equals(const std::shared_ptr<PbcObject>& other) const = 0;
    virtual bool operator==(const PbcObject& other) const = 0;
    
    std::string exportJson() const;
    virtual json_object* toJson(json_object* root, bool returnIdOnly = false) const = 0;
    
    const boost::uuids::uuid& getId() const throw() { return id; }
    const std::string getIdString() const throw() { return boost::uuids::to_string(id); }
    const std::string& getType() const throw() { return type; }
    const std::string& getSubtype() const throw() { return subtype; }
    
    virtual bool isFinal() const noexcept {return _isFinal;}
    virtual bool isValid() const { return true; }
    virtual percent_t finalize() = 0;

    static
    boost::uuids::uuid idOf(const void* item);
    virtual const boost::uuids::uuid& nativeId() const noexcept = 0;

    static inline struct json_object *
    objectById(struct json_object * jobj, const JsonKey& key) {
      struct json_object * o = NULL;
      if (! json_object_object_get_ex(jobj, key.c_str(), &o)) {
        afgh_throw_line("value for key '%s' not found", key.c_str());
      }
      return o;
    }
    
    static void
    readValueFromJson(struct json_object * jobj, const JsonKey& id, mpz_ptr dst);
    
    static void
    readValueFromJson(struct json_object * jobj, const JsonKey& id, const char **dst);
    
    static inline int
    addJsonObject(json_object* parent, const JsonKey& key, json_object* jobj) {
      return json_object_object_add(parent, key.c_str(), jobj);
    }
    
    static json_object* mpz_to_json(const mpz_t number);
    static json_object* limbs_to_json(const mp_limb_t* limbs, mp_size_t size);
    
    void setId(const boost::uuids::uuid& id) { this->id = id; }
    void setId(const std::string& idstr);
    
    void setJsonObject(json_object* jobj) {
      this->jobj = json_object_get(jobj);
    }
    
    json_object* getJsonObject() {
      return jobj;
    }
    
    void setObjectCatalog(std::shared_ptr<ObjectCatalog> catalog) {
      this->catalog = catalog;
    }
    
    std::weak_ptr<ObjectCatalog> getObjectCatalog() const {
      return catalog;
    }
    
  protected:

    std::shared_ptr<ObjectCatalog> getCatalog() const {
      assert(catalog != nullptr);
      return catalog;
    }
    
    static
    struct json_object* createJsonStub(struct json_object* root,
                                       const boost::uuids::uuid& id);
    
    static
    struct json_object* getJsonStub(struct json_object* root,
                                    const boost::uuids::uuid& id);
    
    static
    struct json_object* createJsonId(const boost::uuids::uuid& id);
    
    virtual void isFinal(bool f) { _isFinal = f; }
    
  private:
    static boost::uuids::random_generator gen;
    boost::uuids::uuid id;
    std::string type;
    std::string subtype;
    
    bool _isFinal;
    json_object* jobj;
    mutable std::shared_ptr<ObjectCatalog> catalog;
  };
  
}

#endif /* PbcObject_h */
