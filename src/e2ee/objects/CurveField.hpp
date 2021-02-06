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

#ifndef SRC_E2EE_OBJECTS_CURVEFIELD_HPP_
#define SRC_E2EE_OBJECTS_CURVEFIELD_HPP_

#include <pbc.h>
#include <memory>
#include <vector>
#include <e2ee/objects/CurveField.hpp>
#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/Element.hpp>

namespace e2ee {

class CurveField :
        public virtual AbstractField,
        public virtual PbcComparable<CurveField>,
        public virtual PbcSerializableField {
 public:
  using AbstractField::isFinal;

  CurveField() = delete;
  CurveField(CurveField &&) = delete;
  CurveField(const CurveField &) = delete;
  CurveField &operator=(CurveField &&) = delete;
  CurveField &operator=(const CurveField &) = delete;

  CurveField(std::shared_ptr<PbcContext> context, const boost::uuids::uuid &id,
             const field_s* field, bool isFinal);

  CurveField(std::shared_ptr<PbcContext> context,
             const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
             const boost::uuids::uuid &id,
             const rapidjson::Value &value);

  const std::string &getSubtype() const noexcept override {
    static std::string value = SUBTYPE_CURVE;
    return value;
  }

  bool equals(const CurveField &other) const final;

  void addToJson(Document& doc) const override;

  percent_t finalize(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values) override;

  bool isValid() const override;

  void updateMembers() override;
  std::shared_ptr<Element> elementFromBytes(
          std::vector<std::byte>::const_iterator begin,
          std::vector<std::byte>::const_iterator end) const override;
 private:
  PROPERTY(Element, a);
  PROPERTY(Element, b);
  bool initialized;
};

}  // namespace e2ee

#endif  // SRC_E2EE_OBJECTS_CURVEFIELD_HPP_
