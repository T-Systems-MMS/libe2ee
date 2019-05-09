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
#include <e2ee/objects/CurveField.hpp>
#include <e2ee/ObjectCatalog.hpp>
#include <pbc.h>
#include <json-c/json_object.h>
#include <iostream>

namespace e2ee {
  
  CurveField::CurveField(field_ptr field, std::shared_ptr<ObjectCatalog>& catalog, bool isFinal, const boost::uuids::uuid& id)
  : AbstractField("curve", isFinal, field, catalog, id), initialized(false) {
  }
  
  void CurveField::updateElement(std::shared_ptr<Element>& dst, element_ptr e) {
    if (! dst) {
      dst = std::make_shared<Element>(e,
                                      getCatalog(),
                                      true,
                                      Element::idOf(e));
    }
  }
  
  void CurveField::updateMembers() {
    assert(isFinal());
    const curve_data* data = (curve_data*) get()->data;
    updateElement(a,             (element_ptr)&data->a[0]);
    updateElement(b,             (element_ptr)&data->b[0]);
    updateElement(gen,           (element_ptr)&data->gen[0]);
    updateElement(gen_no_cofac,  (element_ptr)&data->gen_no_cofac[0]);
  }
  
  struct json_object*
  CurveField::toJson(struct json_object* root, bool returnIdOnly) const {
    assert(isFinal());
    json_object* jobj = getJsonStub(root, getId());
    if (jobj) { RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly); }
    else      { jobj = createJsonStub(root, getId()); }
    
    fillJsonObject(jobj, const_cast<field_ptr>(get()));
    const curve_data* data = (curve_data*) get()->data;

    addJsonObject(jobj, KEY_A,          a->toJson(root, true));
    addJsonObject(jobj, KEY_B,          b->toJson(root, true));
    addJsonObject(jobj, KEY_GEN,        gen->toJson(root, true));
    addJsonObject(jobj, KEY_GENNOCOFAC, gen_no_cofac->toJson(root, true));
    
    if (data->cofac != nullptr) {
      addJsonObject(jobj, KEY_COFAC, mpz_to_json(data->cofac));
    }
    if (data->quotient_cmp != NULL) {
      addJsonObject(jobj, KEY_QUOTIENTCMP, mpz_to_json(data->quotient_cmp));
    }
    RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly);
  }

  bool CurveField::isValid() const {
    FAIL_IF(a == nullptr);
    FAIL_IF(b == nullptr);
    FAIL_IF(gen == nullptr);
    FAIL_IF(gen_no_cofac == nullptr);

    FAIL_UNLESS(a->isValid());
    FAIL_UNLESS(b->isValid());
    FAIL_UNLESS(gen->isValid());
    FAIL_UNLESS(gen_no_cofac->isValid());

    SUCCEED();
  }
  
  std::shared_ptr<CurveField>
  CurveField::construct(struct json_object* jobj, std::shared_ptr<ObjectCatalog>& catalog, const boost::uuids::uuid& id) {
    field_ptr field = parse_native(jobj);
    assert(! mpz_is0(&(field->order[0])));
    auto a = std::make_shared<CurveField>(field, catalog, false, id);
    assert(! mpz_is0(&(a->get()->order[0])));
    return a;
  }
  
  percent_t
  CurveField::finalize() {
    percent_t status = 0;
    assert(!isFinal());
    if (a == nullptr) { a = getObjectFromJson<Element>(KEY_A); assert(a != nullptr); status = 10; }
    if (b == nullptr) { b = getObjectFromJson<Element>(KEY_B); assert(b != nullptr); status = 20;}
    
    if (a->get()->field == nullptr || b->get()->field == nullptr) {
      //std::cout << "a and b MUST be initialized first" << std::endl;
      return status;
    }
    status = 30;
    
    if (! initialized) {
      auto order = getMpzFromJson(getJsonObject(), KEY_ORDER);
      auto cofac = getMpzFromJson(getJsonObject(), KEY_COFAC);
      assert(! mpz_is0(order.get()));
      field_init_curve_ab(this->get(), a->get(), b->get(),
                          order.release(),
                          cofac.release());
      initialized = true;
      status = 50;
    }
    assert(initialized);

    if (gen == nullptr) {
      auto __gen = getObjectFromJson<Element>(KEY_GEN);
      if (__gen == nullptr) {
        return status;
      }
      if (! __gen->isFinal()) {
        return status;
      }
      set_gen(__gen);
      status = 70;
    }

    if (gen_no_cofac == nullptr) {
      auto __gen_no_cofac = getObjectFromJson<Element>(KEY_GENNOCOFAC);
      if (__gen_no_cofac == nullptr) {
        return status;
      }
      if (! __gen_no_cofac->isFinal()) {
        return status;
      }
      set_gen_no_cofac(__gen_no_cofac);
      status = 80;
    }

    isFinal(initialized && gen_no_cofac->isFinal() && gen->isFinal());
    if(isFinal()) {
      status = 100;
      assert(isValid());
    }
    return status;
  }
  
  void
  CurveField::set_gen_no_cofac(std::shared_ptr<Element> gen_no_cofac) {
    if (! gen_no_cofac ) {
      return;
    }
    
    assert (gen_no_cofac->isFinal());
    assert (gen_no_cofac->get()->data != nullptr);
    
    this->gen_no_cofac = gen_no_cofac;
    this->gen_no_cofac->get()->field = get();
    curve_data_ptr cdp = reinterpret_cast<curve_data_ptr>(get()->data);
    element_set(cdp->gen_no_cofac, this->gen_no_cofac->get());
  }
  
  void
  CurveField::set_gen(std::shared_ptr<Element> gen) {
    this->gen = gen;
    if (this->gen) {
      curve_data_ptr cdp = reinterpret_cast<curve_data_ptr>(get()->data);
      element_set(cdp->gen, this->gen->get());
    }
  }

  bool CurveField::equals(const std::shared_ptr<PbcObject>& other) const {
    FAIL_UNLESS(isFinal());
    std::shared_ptr<CurveField> o = std::dynamic_pointer_cast<CurveField>(other);
    FAIL_UNLESS(o != nullptr);
    FAIL_UNLESS(o->isFinal());
    
    curve_data* a_data = (curve_data*) get()->data;
    curve_data* b_data = (curve_data*) o->get()->data;
    
    if ( 0 != element_cmp(a_data->a,            b_data->a))            { return false; }
    if ( 0 != element_cmp(a_data->b,            b_data->b))            { return false; }
    if ( 0 != element_cmp(a_data->gen,          b_data->gen))          { return false; }
    if ( 0 != element_cmp(a_data->gen_no_cofac, b_data->gen_no_cofac)) { return false; }
    if ( 0 != mpz_cmp(    a_data->cofac,        b_data->cofac))        { return false; }
    if (a_data->quotient_cmp == NULL && b_data->quotient_cmp != NULL)  { return false; }
    if (a_data->quotient_cmp != NULL && b_data->quotient_cmp == NULL)  { return false; }
    if (a_data->quotient_cmp != NULL && b_data->quotient_cmp != NULL) {
      if ( 0 != mpz_cmp(a_data->quotient_cmp, b_data->quotient_cmp)) { return false; }
    }
    
    return true;
  }
  
  bool CurveField::operator==(const CurveField& other) const {
    FAIL_UNLESS(isFinal());
    FAIL_UNLESS(other.isFinal());
    if (! AbstractField::operator==(other)) {
      return false;
    }
    
    curve_data* a_data = (curve_data*) get()->data;
    curve_data* b_data = (curve_data*) other.get()->data;
    
    if ( 0 != element_cmp(a_data->a,            b_data->a))            { return false; }
    if ( 0 != element_cmp(a_data->b,            b_data->b))            { return false; }
    if ( 0 != element_cmp(a_data->gen,          b_data->gen))          { return false; }
    if ( 0 != element_cmp(a_data->gen_no_cofac, b_data->gen_no_cofac)) { return false; }
    if ( 0 != mpz_cmp(    a_data->cofac,        b_data->cofac))        { return false; }
    if (a_data->quotient_cmp == NULL && b_data->quotient_cmp != NULL)  { return false; }
    if (a_data->quotient_cmp != NULL && b_data->quotient_cmp == NULL)  { return false; }
    if (a_data->quotient_cmp != NULL && b_data->quotient_cmp != NULL) {
      if ( 0 != mpz_cmp(a_data->quotient_cmp, b_data->quotient_cmp)) { return false; }
    }
    return true;
  }
}
