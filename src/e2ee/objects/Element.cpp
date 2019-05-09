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

#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/ObjectCatalog.hpp>
#include <e2ee/conversions.hpp>
#include <iostream>
extern "C" {
#include <pbc.h>
}
#include <memory>
#include <sstream>
#include <utility>
#include <iostream>

namespace e2ee {

  const std::string Element::typeId = "element";
  const std::string Element::subtypeId = "";
  
  Element::Element(element_ptr element, std::shared_ptr<ObjectCatalog> catalog, bool isFinal, const boost::uuids::uuid& id)
  : PbcObjectImpl(Element::getTypeId(),
                  Element::getSubtypeId(),
                  isFinal,
                  element,
                  id), field (nullptr) {
    if (isFinal) {
      field = (*catalog)[(field_ptr)&(element->field[0])];
      //field = AbstractField::constructFromNative((field_ptr)&(element->field[0]), catalog);
      
      char buf[2048];
      element_snprint(&buf[0], sizeof(buf)/sizeof(buf[0]), element);
      
      value = std::string(buf);
      assert(isValid());
    }
  }
  
  
  Element::Element(std::shared_ptr<AbstractField> field, element_ptr element, bool isFinal)
  : PbcObjectImpl(Element::getTypeId(),
                  Element::getSubtypeId(),
                  isFinal,
                  element), field (field)  {
    if (isFinal) {
      char buf[2048];
      element_snprint(&buf[0], sizeof(buf)/sizeof(buf[0]), element);

      value = std::string(buf);
    }
    assert(isValid());
  }
  
  
  Element::Element(std::shared_ptr<AbstractField> field)
  : PbcObjectImpl(Element::getTypeId(),
                  Element::getSubtypeId(),
                  true,
                  allocate_unmanaged<element_s>()), field(field) {
    element_init(get(), field->get());
    assert(isValid());
  }

  bool Element::isValid() const {
    FAIL_UNLESS(isFinal());
    FAIL_IF(get() == nullptr);
    FAIL_IF(get()->field == nullptr);
    FAIL_IF(field == nullptr);
    FAIL_IF(get()->field != field->get());
    SUCCEED();
  }
  
  json_object*
  Element::toJson(json_object* root, bool returnIdOnly) const {

    assert(isValid());

    json_object* jobj = getJsonStub(root, getId());
    if (jobj) { RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly); }
    else      {
      jobj = createJsonStub(root, getId());
      addJsonObject(jobj, KEY_TYPE, json_object_new_string("element"));
    }
    
    char buf[2048];
    
    element_snprint(&buf[0], sizeof(buf)/sizeof(buf[0]), const_cast<element_ptr>(get()));
    //std::cout << ">>> ID: " << getIdString() << std::endl;
    //std::cout << ">>> converting: " << &buf[0] << std::endl;
    /* remove all characters we do not want */
    size_t dst = 0;
    for (size_t src=0; buf[src] != '\0'; ++src) {
      if (isnumber(buf[src]) || buf[src] == ',' || buf[src] == 'O') {
        buf[dst] = buf[src];
        ++dst;
      }
    }
    
    buf[dst] = '\0';
    std::stringstream ss(buf);
    std::string s;
    std::vector<std::string > values;
    while(std::getline(ss, s, ',')) {
      values.push_back(std::move(s));
    }
    
    switch (values.size()) {
      case 0:
        break;
      case 2:
        addNumberToJson(jobj, KEY_y, values[1].cbegin(), values[1].cend());
        /* fall through */
      case 1:
        addNumberToJson(jobj, KEY_x, values[0].cbegin(), values[0].cend());
        break;
      default:
        for (size_t count = 0; count < values.size(); ++count) {
          addNumberToJson(jobj,
                          JsonKey(std::string("a") + std::to_string(count)),
                          values[count].cbegin(), values[count].cend());
        }
    }
    
    addJsonObject(jobj, KEY_FIELD, getField()->toJson(root, true));
    
    RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly);
  }
  
  void Element::addNumberToJson(json_object* jobj,
                                const JsonKey& key,
                                const std::string::const_iterator begin,
                                const std::string::const_iterator end) {
    auto m = str_to_mpz(begin, end, 10);
    auto value = mpz_to_str(m.get());
    addJsonObject(jobj, key, json_object_new_string(value->c_str()));
  }
  
  std::shared_ptr<Element>
  Element::construct(struct json_object* jobj, std::shared_ptr<ObjectCatalog>& catalog, const boost::uuids::uuid& id) {
    element_ptr element = allocate_unmanaged<struct element_s>();
    
    auto a = std::make_shared<Element>(element, catalog, false, id);
    std::list<afgh_mpz_t> values;
    try {
      values.push_back(getMpzFromJson(jobj, KEY_x));
      values.push_back(getMpzFromJson(jobj, KEY_y));
    } catch (AfghError& e) { /* ignore missing value */ }
    
    a->value = Element::formatElementComponents(values);
    return a;
  }
  
  percent_t Element::finalize() {
    assert(!isFinal());
    
    if (field == nullptr) {
      this->field = getObjectFromJson<AbstractField>(KEY_FIELD);
      assert(field != nullptr);
    }
    
    /* we require a field to initialize the element */
    if (field->get()->init == nullptr) {
      //std::cout << "required field is not initialized yet" << std::endl;
      return 50;
    }
    
    element_init(get(), field->get());
    element_set_str(get(), value.c_str(), 10);

    assert(field->get() == get()->field);
    
    assert (get()->data != NULL);
    isFinal(true);
    assert(isValid());
    return 100;
  }
  
  bool Element::equals(const std::shared_ptr<PbcObject>& other) const {
    FAIL_UNLESS(isValid());
    std::shared_ptr<Element> o = std::dynamic_pointer_cast<Element>(other);
    FAIL_UNLESS(o != nullptr);
    FAIL_UNLESS(o->isFinal());
    return (0 == element_cmp(const_cast<element_ptr>(get()), const_cast<element_ptr>(o->get())));
  }
  
  bool Element::operator==(const Element& other) const {
    assert(isValid());
    assert(other.PbcObject::isFinal());
    return (0 == element_cmp(const_cast<element_ptr>(get()), const_cast<element_ptr>(other.get())));
  }
  
  std::shared_ptr<Element> Element::initSameAs() const {
    element_ptr element = allocate_unmanaged<element_s>();
    element_init_same_as(element, const_cast<element_ptr>(get()));
    return std::make_shared<Element>(element, getObjectCatalog().lock(), true, idOf(element));
  }
  
  std::unique_ptr<Element> Element::operator ^(const Element& e) const {
    assert(isFinal());
    assert(e.isFinal());
    
    element_ptr ptr = allocate_unmanaged<element_s>();
    element_init_same_as(ptr, const_cast<element_ptr>(get()));
    element_pow_zn(ptr, (element_ptr) get(), (element_ptr)e.get());
    auto result = std::make_unique<Element>(field, ptr, true);
    assert(field->equals(result->field));
    return result;
  }
  
  std::unique_ptr<Element> Element::operator *(const Element& e) const {
    assert(isValid());
    assert(e.isValid());
    //assert(field->equals(e.field));
    
    element_ptr ptr = allocate_unmanaged<element_s>();
    element_init_same_as(ptr, const_cast<element_ptr>(get()));
    element_mul(ptr, (element_ptr) get(), (element_ptr)e.get());
    auto result = std::make_unique<Element>(field, ptr, true);
    assert(field->equals(result->field));
    return result;
  }
  
  std::unique_ptr<Element> Element::operator /(const Element& e) const {
    assert(isValid());
    assert(e.isValid());
    //assert(field->equals(e.field));
    
    element_ptr ptr = allocate_unmanaged<element_s>();
    element_init_same_as(ptr, const_cast<element_ptr>(get()));
    element_div(ptr, const_cast<element_ptr>(get()), const_cast<element_ptr>(e.get()));
    auto result = std::make_unique<Element>(field, ptr, true);
    assert(field->equals(result->field));
    return result;
    
  }
  
  std::unique_ptr<Element> Element::operator !() const {
    assert(isValid());
    
    element_ptr ptr = allocate_unmanaged<element_s>();
    element_init_same_as(ptr, const_cast<element_ptr>(get()));
    element_invert(ptr, (element_ptr) get());
    auto result = std::make_unique<Element>(field, ptr, true);
    assert(field->equals(result->field));
    return result;
  }
  
  Element& Element::randomize() {
    element_random(get());
    
    /* enforce recalculation of id */
    set(get());
    isFinal(true);
    return *this;
  }
  
  std::string
  Element::formatElementComponents(const std::list<afgh_mpz_t>& values) {
    std::string value;
    if (values.size() > 1) {
      value.append("[");
    }
    
    int n = 1;
    for (const afgh_mpz_t& v: values) {
      char* vs = mpz_get_str(NULL, 10, v.get());
      value.append(vs);
      free(vs);
      if (n < values.size()) {
        value.append(", ");
      }
      ++n;
    }
    
    if (values.size() > 1) {
      value.append("]");
    }
    return value;
  }
  
}
