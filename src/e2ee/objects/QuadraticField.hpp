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

#ifndef QuadraticField_hpp
#define QuadraticField_hpp

#include <memory>
#include <e2ee/objects/SubField.hpp>

namespace e2ee {

class QuadraticField :
        public PbcObjectTypeIdentifier<TYPE_FIELD, SUBTYPE_QUADRATIC>,
        public SubField<QuadraticField> {
 public:

  QuadraticField() = delete;
  QuadraticField(const QuadraticField &) = delete;
  QuadraticField &operator=(const QuadraticField &) = delete;

  QuadraticField(
          std::shared_ptr<PbcContext> context,
          const boost::uuids::uuid &id,
          const field_s *field,
          bool isFinal) :
          PbcObject(context, id, isFinal),
          SubField(context, field) {}

  QuadraticField(
          std::shared_ptr<PbcContext> context,
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const boost::uuids::uuid &id,
          const rapidjson::Value &value)
          : PbcObject(context, id, false), SubField(context, value) {}

 protected:
  percent_t initField() override;
};

}

#endif /* QuadraticField_hpp */
