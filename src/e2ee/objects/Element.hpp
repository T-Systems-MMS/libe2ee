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

#ifndef SRC_E2EE_OBJECTS_ELEMENT_HPP_
#define SRC_E2EE_OBJECTS_ELEMENT_HPP_

#include <pbc.h>
#include <memory>
#include <list>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/objects/PbcObjectTypeIdentifier.hpp>

namespace e2ee {

class Pairing;
class AbstractField;

class Element :
        public PbcObjectTypeIdentifier<TYPE_ELEMENT, SUBTYPE_GENERIC>,
        public PbcObjectImpl<struct element_s>,
                public PbcComparable<Element> {
 public:
  Element() = delete;
  Element(Element&&) = delete;
  Element(const Element&) = delete;
  Element& operator=(Element&&) = delete;

  /**
   * constructs an empty element
   * @param context
   */
  explicit Element(std::shared_ptr<PbcContext> context);

  /**
   * constructs an empty element that is member of a given field
   * @param context
   * @param fieldId
   */
  Element(std::shared_ptr<PbcContext> context, const std::shared_ptr<AbstractField>& _field);

  Element(std::shared_ptr<PbcContext> catalog,
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const boost::uuids::uuid &id,
          const rapidjson::Value &value);

  Element(std::shared_ptr<PbcContext> catalog, const boost::uuids::uuid &id,
          element_ptr element, bool isFinal);

  Element(std::shared_ptr<PbcContext> context,
          const std::shared_ptr<AbstractField>& _field,
          element_ptr element,
          bool isFinal);

  // casting assignment operator
  Element &operator=(const element_s *src);

  // normal assignment operator
  Element &operator=(const Element &);

  std::shared_ptr<Element> initSameAs() const;

  Element &randomize();

  bool isValid() const override;

  /*
   * arithmetical operations
   */
  std::shared_ptr<Element> operator^(const Element &e) const;
  std::shared_ptr<Element> operator^(const std::shared_ptr<Element> &e) const {
    return this->operator^(*(e));
  }

  std::shared_ptr<Element> operator*(const Element &e) const;
  std::shared_ptr<Element> operator*(const std::shared_ptr<Element> &e) const {
    return this->operator*(*(e));
  }

  std::shared_ptr<Element> operator/(const Element &e) const;

  std::shared_ptr<Element> operator/(const std::shared_ptr<Element> &e) const {
    return this->operator/(*(e));
  }

  std::shared_ptr<Element> operator!() const;

  void addToJson(Document& doc) const override;

  e2ee::percent_t finalize(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values) override;

  static std::string
  formatElementComponents(const std::vector<afgh_mpz_t> &values);

  void updateStringValue();
  std::vector<std::byte> toBytes() const;

 protected:
  bool equals(const Element &other) const final;

 private:
  PROPERTY(AbstractField, field);
  std::string value;

  element_s native_element;

  static
  Value numberToJson(const std::string& s,
          rapidjson::MemoryPoolAllocator<>& allocator);
};

}  // namespace e2ee
#endif  // SRC_E2EE_OBJECTS_ELEMENT_HPP_
