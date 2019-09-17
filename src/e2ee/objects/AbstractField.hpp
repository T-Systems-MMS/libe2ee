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

#ifndef AbstractField_hpp
#define AbstractField_hpp

#include <pbc.h>
#include <string>
#include <memory>
#include <map>
#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/memory.hpp>

namespace e2ee {

typedef const field_ptr field_cptr;
class FieldFactory;
class PbcContext;

class AbstractField :
        public PbcObjectImpl<struct field_s>,
        public PbcComparable<AbstractField> {
 public:

  explicit AbstractField(const field_s* field):
    PbcObjectImpl<struct field_s>(field) {
  }

  explicit AbstractField(const rapidjson::Value &value)
          : AbstractField(parse_native(value)) {

  }

  virtual ~AbstractField() {}

  bool equals(const AbstractField &other) const override;
  virtual void updateMembers() = 0;

  void addToJson(Document& doc) const override;

  static bool compareField(field_cptr f1, field_cptr f2, std::weak_ptr<PbcContext>);

 protected:

  static field_ptr
  parse_native(const rapidjson::Value& jobj);
};

}  // namespace e2ee

#endif /* AbstractField_hpp */
