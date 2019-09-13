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

#include <gmp.h>
extern "C" {
#include <pbc.h>
}

#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <aixlog.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/PbcContext.hpp>
#include <e2ee/conversions.hpp>

namespace e2ee {

Element &Element::operator=(const element_s *src) {
  element_init_same_as(&native_element, const_cast<element_s *>(src));
  element_set(&native_element, const_cast<element_s *>(src));
  set(&native_element);
  if (getId().is_nil()) {
    setId(idOf(&native_element));
  }

  this->field = lockedContext()->fromNative(src->field);

  isFinal(true);
  return *this;
}

Element &Element::operator=(const Element &src) {
  Element::operator=(src.get());
  isFinal(src.isFinal());
  return *this;
}

Element::Element(std::shared_ptr<PbcContext> context)
        : PbcObject(context, boost::uuids::nil_uuid(), false),
          PbcObjectImpl(nullptr) {
}

Element::Element(std::shared_ptr<PbcContext> context,
                 const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
                 const boost::uuids::uuid &id,
                 const rapidjson::Value &value)
        : PbcObject(context, id, false),
          PbcObjectImpl(&native_element) {
  std::vector<afgh_mpz_t> v;
  setJsonObject(values.at(id));
  try {
    assert(value.HasMember(KEY_x.c_str()));
    v.push_back(json_to_mpz(value[KEY_x.c_str()]));

    if (value.HasMember(KEY_y.c_str())) {
      v.push_back(json_to_mpz(value[KEY_y.c_str()]));
    }
  } catch (AfghError &e) { /* ignore missing value */ }

  this->value = Element::formatElementComponents(v);
  this->field = fieldFromJson(values, KEY_FIELD, false);
}

Element::Element(std::shared_ptr<PbcContext> context,
                  const boost::uuids::uuid &id,
                  element_ptr element, bool isFinal)
        : PbcObject(context, id, isFinal),
          PbcObjectImpl(element) {
  if (isFinal) {
    // create copy of element
    element_init_same_as(&native_element, element);
    element_set(&native_element, element);
    set(&native_element);
    setId(idOf(&native_element));

    field = lockedContext()->fromNative(element->field);

    char buf[2048];
    element_snprint(&buf[0], sizeof(buf) / sizeof(buf[0]), &native_element);

    value = std::string(buf);
    assert(isValid());
  }
}

Element::Element(std::shared_ptr<PbcContext> context,
                 const std::shared_ptr<AbstractField>& _field,
                 element_ptr element,
                 bool isFinal)
        : PbcObject(context, idOf(element), isFinal),
          PbcObjectImpl(element), field(_field) {
  if (isFinal) {
    char buf[2048];
    element_snprint(&buf[0], sizeof(buf) / sizeof(buf[0]), element);

    value = std::string(buf);
  }
  assert(isValid());
}


Element::Element(std::shared_ptr<PbcContext> context,
        const std::shared_ptr<AbstractField>& _field)
        : PbcObject(context, boost::uuids::nil_uuid(), true),
          PbcObjectImpl(allocate_unmanaged<element_s>()), field(_field) {
  element_init(get(), getField()->get());
  setId(idOf(get()));
  assert(isValid());
}

bool Element::isValid() const {
  FAIL_UNLESS(isFinal());
  FAIL_IF(!field.lock());
  FAIL_IF(get() == nullptr);
  FAIL_IF(get()->field == nullptr);
  SUCCEED();
}

json_object *
Element::toJson(json_object *root, bool returnIdOnly) const {

  assert(isValid());

  json_object *jobj = getJsonStub(root, getId());
  if (jobj) { RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly); }
  else {
    jobj = createJsonStub(root, getId());
    addJsonObject(jobj, KEY_TYPE, json_object_new_string("element"));
  }

  char buf[2048];

  element_snprint(&buf[0], sizeof(buf) / sizeof(buf[0]), const_cast<element_ptr>(get()));
  //std::cout << ">>> ID: " << getIdString() << std::endl;
  //std::cout << ">>> exporting: " << &buf[0] << std::endl;
  /* remove all characters we do not want */
  size_t dst = 0;
  for (size_t src = 0; buf[src] != '\0'; ++src) {
    if (isnumber(buf[src]) || buf[src] == ',' || buf[src] == 'O') {
      buf[dst] = buf[src];
      ++dst;
    }
  }

  buf[dst] = '\0';
  std::stringstream ss(buf);
  std::string s;
  std::vector<std::string> values;
  while (std::getline(ss, s, ',')) {
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

void Element::addNumberToJson(json_object *jobj,
                              const JsonKey &key,
                              const std::string::const_iterator begin,
                              const std::string::const_iterator end) {
  auto m = str_to_mpz(begin, end, 10);
  auto value = mpz_to_str(m.get());
  addJsonObject(jobj, key, json_object_new_string(value->c_str()));
}

percent_t Element::finalize(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values) {
  assert(!isFinal());

  /* we require a field to initialize the element */
  auto fptr = field.lock();
  assert(fptr != nullptr);
  if (fptr->get()->init == NULL) {
    LOG(DEBUG) << COLOR(yellow) << "required field is not initialized yet" << std::endl;
    LOG(DEBUG) << "field type is    " << fptr->getSubtype() << std::endl;
    LOG(DEBUG) << "field id is      " << fptr->getId() << std::endl;
    LOG(DEBUG) << "field address is " << static_cast<PbcObject*>(fptr.get()) << std::endl << COLOR(none);
    return 50;
  }

  element_init(get(), fptr->get());
  element_set_str(get(), value.c_str(), 10);

  #ifndef NDEBUG
  char buf[2048];
  element_snprint(&buf[0],
          sizeof(buf) / sizeof(buf[0]),
          const_cast<element_ptr>(get()));
  assert(value == buf);
  #endif  // NDEBUG

  assert(fptr->get() == get()->field);

  assert(get()->data != NULL);
  isFinal(true);
  assert(isValid());
  return 100;
}

bool Element::equals(const Element &other) const {
  return (0 == element_cmp(const_cast<element_ptr>(get()),
          const_cast<element_ptr>(other.get())));
}

std::shared_ptr<Element> Element::initSameAs() const {
  element_ptr element = allocate_unmanaged<element_s>();
  element_init_same_as(element, const_cast<element_ptr>(get()));
  return lockedContext()->fromNative(element);
}

std::shared_ptr<Element> Element::operator^(const Element &e) const {
  assert(isFinal());
  assert(e.isFinal());

  element_ptr ptr = allocate_unmanaged<element_s>();
  element_init_same_as(ptr, const_cast<element_ptr>(get()));
  element_pow_zn(ptr, (element_ptr) get(), (element_ptr) e.get());
  return lockedContext()->fromNative(ptr);
}

std::shared_ptr<Element> Element::operator*(const Element &e) const {
  assert(isValid());
  assert(e.isValid());

  element_ptr ptr = allocate_unmanaged<element_s>();
  element_init_same_as(ptr, const_cast<element_ptr>(get()));
  element_mul(ptr, (element_ptr) get(), (element_ptr) e.get());
  return lockedContext()->fromNative(ptr);
}

std::shared_ptr<Element> Element::operator/(const Element &e) const {
  assert(isValid());
  assert(e.isValid());

  element_ptr ptr = allocate_unmanaged<element_s>();
  element_init_same_as(ptr, const_cast<element_ptr>(get()));
  element_div(ptr, const_cast<element_ptr>(get()),
          const_cast<element_ptr>(e.get()));
  return lockedContext()->fromNative(ptr);
}

std::shared_ptr<Element> Element::operator!() const {
  assert(isValid());

  element_ptr ptr = allocate_unmanaged<element_s>();
  element_init_same_as(ptr, const_cast<element_ptr>(get()));
  element_invert(ptr, (element_ptr) get());
  return lockedContext()->fromNative(ptr);
}

Element &Element::randomize() {
  element_random(get());

  /* enforce recalculation of id */
  set(get());
  isFinal(true);
  return *this;
}

std::string
Element::formatElementComponents(const std::vector<afgh_mpz_t> &values) {
  std::string value;
  assert(values.size() > 0);
  const auto& first_value = values[0];
  const bool require_brackets = (values.size() > 1);

  if (require_brackets) {
    value.append("[");
  }

  int n = 1;
  for (const auto &v : values) {
    char *vs = mpz_get_str(NULL, 10, v.get());
    value.append(vs);
    free(vs);
    if (n < values.size()) {
      value.append(", ");
    }
    ++n;
  }

  if (require_brackets) {
    value.append("]");
  }
  return value;
}

std::shared_ptr<Element> PbcContext::getElementFromJson(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
        const rapidjson::Value &value,
        const JsonKey &key, bool requireFinal) {
  return e2ee::dynamic_pointer_cast<Element>
          (getObjectFromJson(values, value, key, requireFinal));
}

}  // namespace e2ee
