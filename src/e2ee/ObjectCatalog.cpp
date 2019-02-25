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

#include <e2ee/ObjectCatalog.hpp>
#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/conversions.hpp>
#include <e2ee/errors.hpp>
#include <e2ee/objects/MontFPField.hpp>
#include <e2ee/objects/CurveField.hpp>
#include <e2ee/objects/MultiplicativeSubgroup.hpp>
#include <e2ee/objects/QuadraticField.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/Pairing.hpp>
extern "C" {
#include <json-c/json_object.h>
#include <json-c/json_visit.h>
#include <json-c/json_tokener.h>
}
#include <memory>
#include <iostream>
#include <vector>
#include <boost/uuid/uuid.hpp>

namespace e2ee {
  
  
  
  void ObjectCatalog::populate(const std::string& json)
  {
    addParser<MontFPField>("montfp");
    addParser<CurveField>("curve");
    addParser<QuadraticField>("fi");
    addParser<MultiplicativeSubgroup>("mulg");
    addParser<Pairing>("pairing");
    addParser<Element>(Element::getTypeId());
    
    parseJson(json);
    assert(base != NULL);
    //assert(root != NULL);
    assert(rootId != boost::uuids::nil_uuid());
    auto self = shared_from_this();
    json_c_visit(base, 0, createObjectStub, &self);
    
    assert(objectList.size() > 0);
    assert(hasObject(rootId));
    
    finalizeObjects();
  }
  
  int
  createObjectStub(json_object *jso, int flags, json_object *parent_jso,
                   const char *jso_key, size_t *jso_index, void* userdata) {
    const char* type = NULL;
    boost::uuids::uuid id = boost::uuids::nil_uuid();
    // copy shared_ptr
    auto catalog = *(reinterpret_cast<std::shared_ptr<ObjectCatalog>*>(userdata));
    
    if (jso_key == NULL)             { return JSON_C_VISIT_RETURN_CONTINUE; }
    
    if (! isValidUuid(jso_key, id))  { return JSON_C_VISIT_RETURN_SKIP; }
    assert(id != boost::uuids::nil_uuid());
    if (catalog->hasObject(id))      { return JSON_C_VISIT_RETURN_SKIP; }
    
    /* looks like a UUID; continue */
    PbcObject::readValueFromJson(jso, KEY_TYPE, &type);
    if (KEY_FIELD == type) {
      PbcObject::readValueFromJson(jso, KEY_SUBTYPE, &type);
    }
    
    std::map<std::string, ObjectCatalog::parser_t >::iterator iter = catalog->parsers.find(type);
    if (iter == catalog->parsers.end()) {
      afgh_throw_line("no parser found for type '%s'", type);
    }
    
    assert(catalog->objectList.find(id) == catalog->objectList.end());
    auto newObject = iter->second(jso, catalog, id);
    newObject->setObjectCatalog(catalog);
    newObject->setJsonObject(jso);
    newObject->setId(id);
    catalog->objectList[id] = newObject;
    assert(catalog->objectList.find(id) != catalog->objectList.end());
    assert(catalog->objectList.find(id)->second->getId() == id);
    
    return JSON_C_VISIT_RETURN_SKIP;
  }
  
  json_object*
  ObjectCatalog::getJsonObject(const std::string& id) {
    struct json_object* obj = NULL;
    if (! json_object_object_get_ex(base, id.c_str(), &obj)) {
      return NULL;
    }
    return obj;
  }
  
  json_object*
  ObjectCatalog::parseJson(const std::string& str) {
    base = json_tokener_parse(str.c_str());
    struct json_object* metainfo, *type, *idobj;
    
    if (! json_object_object_get_ex(base, "root", &metainfo)) {
      json_object_put(base);
      afgh_throw_line("missing 'root' key");
    }
    
    if (! json_object_object_get_ex(metainfo, "type", &type)) {
      json_object_put(base);
      afgh_throw_line("missing 'type' key");
    }
    
    if (! json_object_object_get_ex(metainfo, "id", &idobj)) {
      json_object_put(base);
      afgh_throw_line("missing 'id' key");
    }
    
    if (! isValidUuid(json_object_get_string(idobj), rootId)) {
      afgh_throw_line("invalid uuid syntax");
    }
    
    json_object* root = nullptr;
    if (! json_object_object_get_ex(base, boost::uuids::to_string(rootId).c_str(), &root)) {
      json_object_put(base);
      afgh_throw_line("missing root element");
    }
    return root;
  }
  
  void ObjectCatalog::finalizeObjects() {
    bool isFinal;
    percent_t status, old_status = 0;
    
    do {
      isFinal = true;
      status = 0;
      for(auto i: objectList) {
        if (i.second->isFinal()) {
          status += 100;
        } else {
          const percent_t new_status = i.second->finalize();
          assert(i.second->isFinal() == (new_status == 100));
          status += new_status;
          isFinal &= i.second->isFinal();
        }
      }
      std::cout << status << "% " << std::endl;
      
      assert(isFinal || status > old_status);
      old_status = status;
    } while (! isFinal);
    assert (status == 100 * objectList.size());
  }
  
  std::shared_ptr<PbcObject>
  ObjectCatalog::getObjectFromJson(json_object* jobj, const JsonKey& key, bool requireFinal) {
    static boost::uuids::string_generator gen;
    const boost::uuids::uuid id = gen(get_string_value_by_id(jobj, key));
    
    decltype(objectList)::iterator iter = objectList.find(id);
    if (iter == objectList.end()) {
      return nullptr;
    }
    
    if (requireFinal) {
      if (! iter->second->isFinal()) {
        finalizeObjects();
      }
    }
    
    return iter->second;
  }
  /*
  template <>
  __mpz_struct
  getNativeObject(json_object* jobj, const JsonKey& key) {
    mpz_ptr ptr = getNativeObject<mpz_ptr>(jobj, key);
    const __mpz_struct m = *ptr;
    free(ptr);
    return m;
  }*/
  
  afgh_mpz_t
  getMpzFromJson(json_object* jobj, const JsonKey& key) {
    const std::string strObj = ObjectCatalog::get_string_value_by_id(jobj, key);
    return str_to_mpz(strObj.cbegin(), strObj.cend());
  }
  
  std::unique_ptr<mp_limb_t>
  getLimbsFromJson(json_object* jobj, const JsonKey& key) {
    const std::string strObj = ObjectCatalog::get_string_value_by_id(jobj, key);
    size_t limbs = 0;
    mp_limb_t *ptr = str_to_limbs(strObj.cbegin(), strObj.cend(), &limbs);
    return std::unique_ptr<mp_limb_t>(ptr);
  }
  
  
  std::string
  ObjectCatalog::get_string_value_by_id(json_object* jobj, const JsonKey& key) {
    json_object* o = NULL;
    if (! json_object_object_get_ex(jobj, key.c_str(), &o)) {
      afgh_throw_line("no value found for key '%s'", key.c_str());
    }
    
    return json_object_get_string(o);
  }
  
  bool isValidUuid(const std::string& maybe_uuid,  boost::uuids::uuid& result) {
    using namespace boost::uuids;
    static string_generator gen;
    
    try {
      result = gen(maybe_uuid);
      return result.version() != uuid::version_unknown;
    } catch(...) {
      return false;
    }
  }
}
