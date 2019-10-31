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
#include <vector>
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

  std::shared_ptr<Element> emptyElement() const;
  std::shared_ptr<Element> randomElement() const;
  /*
  template <typename Iterator>
  std::shared_ptr<Element> elementFromBytes(Iterator begin, Iterator end) const;
   */

  static bool compareField(field_cptr f1, field_cptr f2, std::weak_ptr<PbcContext>);

 protected:

  static field_ptr
  parse_native(const rapidjson::Value& jobj);
};

class Element;
struct PbcSerializableField: public virtual AbstractField {
  virtual std::shared_ptr<Element> elementFromBytes(
          std::vector<std::byte>::const_iterator begin,
          std::vector<std::byte>::const_iterator end) const = 0;
};

/*
template <typename Iterator>
std::shared_ptr<Element>
        AbstractField::elementFromBytes(Iterator begin, Iterator end) const {
  std::vector<std::byte> buffer(begin, end);

  auto e = emptyElement();
  int bytes = 0;
  if (0 == std::strcmp(get()->name, "curve")) {
    bytes = element_from_bytes_x_only(e->get(),
                                      reinterpret_cast<unsigned char *>(&buffer[0]));

  } else {
    bytes = element_from_bytes(e->get(),
                               reinterpret_cast<unsigned char *>(&buffer[0]));
  }
  afgh_check(bytes == buffer.size(),
             "invalid number of bytes read. expected %d, but read %d bytes.",
             buffer.size(), bytes);

  #ifndef NDEBUG
  auto b = e->toBytes();
  auto b2 = (e->operator!())->toBytes();
  assert(std::equal(begin, end, b.begin()) ||
         std::equal(begin, end, b2.begin()));
  #endif

  return e;
}
*/
}  // namespace e2ee

#endif /* AbstractField_hpp */
