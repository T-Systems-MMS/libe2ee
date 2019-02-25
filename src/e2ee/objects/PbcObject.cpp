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

#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/ObjectCatalog.hpp>
#include <e2ee/errors.hpp>
#include <e2ee/conversions.hpp>
#include <pbc.h>
#include <gmp.h>
#include <iostream>

namespace e2ee {
  
  boost::uuids::random_generator PbcObject::gen;
  
  void
  PbcObject::readValueFromJson(struct json_object * jobj, const JsonKey& key, mpz_ptr dst) {
    const char* strObj = NULL;
    readValueFromJson(jobj, key, &strObj);
    
    if (0 != mpz_set_str(dst, strObj, 10)) {
      afgh_throw_line("unable to convert '%s' to mpz_t", strObj);
    }
  }
  
  void
  PbcObject::readValueFromJson(struct json_object * jobj, const JsonKey& key, const char **dst) {
    const char* strObj = json_object_get_string(objectById(jobj, key));
    if (strObj == NULL) {
      afgh_throw_line("Json object is not a string");
    }
    *dst = strObj;
  }
  
  std::string PbcObject::exportJson() const {
    struct json_object* root = json_object_new_object();
    
    struct json_object* r = json_object_new_object();
    addJsonObject(r, KEY_ID, json_object_new_string(getIdString().c_str()));
    addJsonObject(r, KEY_TYPE, json_object_new_string(getType().c_str()));
    addJsonObject(root, KEY_ROOT, r);
    
    json_object_object_add(root, getIdString().c_str(), toJson(root));
    std::string retval = json_object_to_json_string_ext(root,
                                                        JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY);
    json_object_put(root);
    return retval;
  }
  
  struct json_object* PbcObject::createJsonId(const boost::uuids::uuid& id) {
    return json_object_new_string(boost::uuids::to_string(id).c_str());
  }
  
  struct json_object* PbcObject::getJsonStub(struct json_object* root,
                                             const boost::uuids::uuid& id) {
    struct json_object* jobj = nullptr;
    //std::cout << ">>> searching json object " << boost::uuids::to_string(id).c_str() << std::endl;
    if (json_object_object_get_ex(root, boost::uuids::to_string(id).c_str(), &jobj)) {
      //std::cout << ">>> found it !!! " << std::endl;
      return json_object_get(jobj);
    }
    return nullptr;
  }
  
  struct json_object* PbcObject::createJsonStub(struct json_object* root,
                                                const boost::uuids::uuid& id) {
    struct json_object* jobj = nullptr;
    jobj = json_object_new_object();
    json_object_object_add(root, boost::uuids::to_string(id).c_str(), jobj);
    return json_object_get(jobj);
  }
  
  json_object* PbcObject::mpz_to_json(const mpz_t number) {
    auto value = mpz_to_str(number);
    return json_object_new_string(value->c_str());
  }
  
  struct json_object* PbcObject::limbs_to_json(const mp_limb_t* limbs, mp_size_t size) {
    auto value = limbs_to_str(limbs, size);
    return json_object_new_string(value->c_str());
  }
  
  void PbcObject::setId(const std::string& idstr) {
    static boost::uuids::string_generator gen;
    this->id = gen(idstr);
  }
}
