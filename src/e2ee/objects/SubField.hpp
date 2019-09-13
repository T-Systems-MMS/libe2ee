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

#ifndef SubField_hpp
#define SubField_hpp

#include <e2ee/objects/AbstractField.hpp>
#include <string>
#include <cstring>
#include <memory>

namespace e2ee {
/* field that is defined by another field */
template<class F>
class SubField :
        public AbstractField,
        public PbcComparable<SubField<F>> {
 public:
  using AbstractField::isFinal;
  using PbcComparable<SubField<F>>::operator==;

  SubField() = delete;

  SubField(const SubField &) = delete;

  SubField &operator=(const SubField &) = delete;

  SubField(std::shared_ptr<PbcContext>& context, const field_s *field)
          : AbstractField(field) {
    if (isFinal()) {
      auto superfield = reinterpret_cast<field_ptr>(field->data);
      assert(superfield != nullptr);
      set_superField(context->fromNative(superfield));
      assert(has_superField());
    }
  }

  SubField(std::weak_ptr<PbcContext> context, const rapidjson::Value &value)
          : AbstractField(value) {
    if (auto ctx = context.lock()) {   // do not use lockedContext() in ctor !!!!
      auto superfield = reinterpret_cast<field_ptr>(get()->data);
      if (superfield != nullptr) {
        set_superField(ctx->fromNative(superfield));
        assert(has_superField());
      }
    }
  }

  bool equals(const SubField<F> &other) const final;
  void updateMembers() override {}

  percent_t finalize(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>> &values) override;

  virtual json_object *toJson(json_object *root, bool returnIdOnly = false) const override;

  virtual void isFinal(bool f) override { AbstractField::isFinal(f); }

  virtual bool isFinal() const noexcept override { return AbstractField::isFinal(); }

 protected:
  virtual percent_t initField() {
    field_init(get());
    return 100;
  }

 private:
  PROPERTY(AbstractField, superField);
};

template<class F>
percent_t SubField<F>::finalize(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>> &values) {
  assert(!isFinal());
  try {
    /*
     * do not require finalization here
     */
    set_superField(fieldFromJson(values, KEY_BASE, false));
  } catch(AfghError& e) {
    isFinal(false);
    return 50;
  }
  get()->data = superField()->get();

  percent_t status = initField();
  isFinal(status == 100);
  return status;
}

template<class F>
struct json_object *
SubField<F>::toJson(struct json_object *root, bool returnIdOnly) const {
  json_object *jobj = getJsonStub(root, getId());
  if (jobj) { RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly); }
  else { jobj = createJsonStub(root, getId()); }

  fillJsonObject(root, jobj, const_cast<field_ptr>(get()));

  assert(has_superField());
  addJsonObject(jobj, KEY_BASE, superField()->toJson(root, true));
  RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly);
}


template<class F>
bool SubField<F>::equals(const SubField<F> &other) const {
  if (AbstractField::equals(other)) {
    return (superField()->PbcComparable<AbstractField>::equals(other.superField()));
  } else {
    return false;
  }
}

}  // nmaespace e2ee

#endif /* SubField_hpp */
