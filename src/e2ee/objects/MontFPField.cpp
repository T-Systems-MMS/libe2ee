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

#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/MontFPField.hpp>
#include <e2ee/ObjectCatalog.hpp>
extern "C" {
#include <pbc.h>
#include <pbc_fp.h>
}

namespace e2ee {
  struct json_object*
  MontFPField::toJson(struct json_object* root, bool returnIdOnly) const {
    json_object* jobj = getJsonStub(root, getId());
    if (jobj) { RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly); }
    else      { jobj = createJsonStub(root, getId()); }
    
    fillJsonObject(jobj, const_cast<field_ptr>(get()));
    const montfp_data* data = (montfp_data*) get()->data;
    
    addJsonObject(jobj, KEY_MODULUS, limbs_to_json(data->primelimbs, data->limbs));
    addJsonObject(jobj, KEY_NEGPINV, limbs_to_json(&(data->negpinv), 1));
    addJsonObject(jobj, KEY_R, limbs_to_json(data->R, data->limbs));
    addJsonObject(jobj, KEY_R3, limbs_to_json(data->R3, data->limbs));
    RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly);
  }
  
  
  std::shared_ptr<MontFPField>
  MontFPField::construct(struct json_object* jobj, std::shared_ptr<ObjectCatalog>& catalog, const boost::uuids::uuid& id) {
    field_ptr field = parse_native(jobj);
    //pbc_tweak_use_fp((char*)"mont");
    montfp_data* data = allocate_unmanaged<montfp_data>();
    field->data = data;
    afgh_mpz_t primelimbs = getMpzFromJson(jobj, KEY_MODULUS);
    field_init_mont_fp(field, primelimbs.release());
    
    auto ptr = getLimbsFromJson(jobj, KEY_NEGPINV);
    data->negpinv = (*ptr);
    data->R = getLimbsFromJson(jobj, KEY_R).release();
    data->R3 = getLimbsFromJson(jobj, KEY_R3).release();
    auto a = std::make_shared<MontFPField>(field, catalog, false, id);
    return a;
  }
  
  bool MontFPField::equals(const std::shared_ptr<PbcObject>& other) const {
    FAIL_UNLESS(isFinal());
    std::shared_ptr<MontFPField> o = std::dynamic_pointer_cast<MontFPField>(other);
    FAIL_UNLESS(o != nullptr);
    FAIL_UNLESS(o->isFinal());
    
    montfp_data* a_data = (montfp_data*) get()->data;
    montfp_data* b_data = (montfp_data*) o->get()->data;
    if ( 0 != mpn_cmp(&(a_data->negpinv), &(b_data->negpinv), 1))            { return false; }
    if ( 0 != mpn_cmp(a_data->primelimbs, b_data->primelimbs, a_data->limbs)){ return false; }
    if ( 0 != mpn_cmp(a_data->R,          b_data->R, a_data->limbs))         { return false; }
    if ( 0 != mpn_cmp(a_data->R3,         b_data->R3, a_data->limbs))        { return false; }
    return true;
  }
  
  bool MontFPField::operator==(const MontFPField& other) const {
    montfp_data* a_data = (montfp_data*) get()->data;
    montfp_data* b_data = (montfp_data*) other.get()->data;
    if ( 0 != mpn_cmp(&(a_data->negpinv), &(b_data->negpinv), 1))            { return false; }
    if ( 0 != mpn_cmp(a_data->primelimbs, b_data->primelimbs, a_data->limbs)){ return false; }
    if ( 0 != mpn_cmp(a_data->R,          b_data->R, a_data->limbs))         { return false; }
    if ( 0 != mpn_cmp(a_data->R3,         b_data->R3, a_data->limbs))        { return false; }
    return true;
  }
}
