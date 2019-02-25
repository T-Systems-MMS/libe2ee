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
#include <e2ee/objects/MontFPField.hpp>
#include <e2ee/objects/QuadraticField.hpp>
#include <e2ee/objects/MultiplicativeSubgroup.hpp>
#include <iostream>
#include <memory>

namespace e2ee {
  
  const std::string AbstractField::typeId = "field";

  std::shared_ptr<AbstractField> AbstractField::constructFromNative(field_ptr ptr, std::shared_ptr<ObjectCatalog>& catalog) {
    static const std::map <std::string,
              std::function<std::shared_ptr<AbstractField>(field_ptr, std::shared_ptr<ObjectCatalog>&)> >
    constructors = {
      { "curve",  [](field_ptr p, std::shared_ptr<ObjectCatalog>& c) {return std::make_shared<CurveField>(p, c, true, idOf(p)); } },
      { "montfp", [](field_ptr p, std::shared_ptr<ObjectCatalog>& c) {return std::make_shared<MontFPField>(p, c, true, idOf(p)); } },
      { "fi",     [](field_ptr p, std::shared_ptr<ObjectCatalog>& c) {return std::make_shared<QuadraticField>(p, c, true, idOf(p)); } },
      { "mulg",   [](field_ptr p, std::shared_ptr<ObjectCatalog>& c) {return std::make_shared<MultiplicativeSubgroup>(p, c, true, idOf(p)); } }
    };
    
    if (catalog->hasObject(idOf(ptr))) {
      return std::dynamic_pointer_cast<AbstractField>(catalog->at(idOf(ptr)));
    }
    
    auto c = constructors.find(ptr->name);
    if (c == constructors.end()) {
      return nullptr;
    }
    auto f = c->second(ptr, catalog);
    assert (f->getId() == idOf(ptr));
    catalog->addObject(f);
    f->updateMembers();
    return f;
  }
  
  void
  AbstractField::fillJsonObject(json_object* jobj, field_cptr field) {
    assert(std::string(field->name).length() > 0);
    addJsonObject(jobj, KEY_TYPE, json_object_new_string("field"));
    addJsonObject(jobj, KEY_SUBTYPE, json_object_new_string(field->name));
    addJsonObject(jobj, KEY_ORDER, mpz_to_json(field->order));
  }
  
  field_ptr
  AbstractField::parse_native(struct json_object* jobj) {
    const char* type = NULL;
    const char* subtype = NULL;
    field_ptr field = allocate_unmanaged<struct field_s>();
    
    /* read type and subtype*/
    PbcObject::readValueFromJson(jobj, KEY_TYPE, &type);
    PbcObject::readValueFromJson(jobj, KEY_SUBTYPE, &subtype);
    
    field_init(field);
    auto order = getMpzFromJson(jobj, KEY_ORDER);
    mpz_set(field->order, order.get());
    assert(! mpz_is0(&(field->order[0])));
    field->name = (char*)malloc(strlen(subtype)+1);
    strcpy(field->name, subtype);
    
    return field;
  }
  
  
  bool AbstractField::operator==(const AbstractField& other) const {
    assert(isFinal());
    assert(other.PbcObject::isFinal());
    
    if (0 != strcmp(get()->name, other.get()->name)) { return false; }
    return (0 == mpz_cmp(get()->order, other.get()->order));
  }
  
  bool
  AbstractField::compareField(field_cptr a, field_cptr b, std::shared_ptr<ObjectCatalog>& catalog) {
    int res;
    
    field_ptr ma = const_cast<field_ptr>(a);
    field_ptr mb = const_cast<field_ptr>(b);
    
    if (a == NULL || b == NULL) { return -1; }
    
    res = strcmp(a->name, b->name);
    if (res != 0) { return res; }
    if (strcmp (a->name, "curve") == 0) {
      CurveField f1(ma, catalog, true, idOf(ma));
      CurveField f2(mb, catalog, true, idOf(mb));
      return f1 == f2;
    }
    if (strcmp (a->name, "montfp") == 0) {
      MontFPField f1(ma, catalog, true, idOf(ma));
      MontFPField f2(mb, catalog, true, idOf(mb));
      return f1 == f2;
    }
    if (strcmp (a->name, "fi") == 0) {
      QuadraticField f1(ma, catalog, true, idOf(ma));
      QuadraticField f2(mb, catalog, true, idOf(mb));
      return f1 == f2;
    }
    if (strcmp (a->name, "mulg") == 0) {
      MultiplicativeSubgroup f1(ma, catalog, true, idOf(ma));
      MultiplicativeSubgroup f2(mb, catalog, true, idOf(mb));
      return f1 == f2;
    }
    return false;
  }
}
