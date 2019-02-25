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
  template <class F>
  class SubField : public AbstractField {
  public:
    SubField() = delete;
    SubField(const SubField&) = delete;
    SubField& operator=(const SubField&) = delete;
    
    SubField(field_ptr field, std::shared_ptr<ObjectCatalog>& catalog, bool isFinal, const boost::uuids::uuid& id)
    : AbstractField(F::SUBTYPE_ID, isFinal, field, catalog, id) {
      if (isFinal) {
        field_ptr superfield = reinterpret_cast<field_ptr>(field->data);
        assert(superfield != nullptr);
        setSuperField(constructFromNative(superfield, catalog));
        assert(hasSuperField());
      }
    }
    
    virtual bool equals(const std::shared_ptr<PbcObject>& other) const override;
    
    static std::shared_ptr<F>
    construct(struct json_object* jobj, std::shared_ptr<ObjectCatalog>& catalog, const boost::uuids::uuid& id) {
      return std::make_shared<F>(parse_native(jobj), catalog, false, id);
    }
    
    virtual percent_t finalize() override;
    
    bool hasSuperField() const throw() { return superField != nullptr; }
    std::shared_ptr<AbstractField> getSuperField() throw() { return superField; }
    const std::shared_ptr<AbstractField> getSuperField() const throw() { return superField; }
    void setSuperField(std::shared_ptr<AbstractField> sf) {superField = sf;}
    
    virtual json_object* toJson(json_object* root, bool returnIdOnly = false) const override;
    
    virtual void isFinal(bool f) override { AbstractField::isFinal(f); }
    virtual bool isFinal() const noexcept override { return AbstractField::isFinal(); }
  protected:
    virtual percent_t initField() { field_init(get()); return 100; }
    
  private:
    std::shared_ptr<AbstractField> superField;
  };
  
  template <class F>
  percent_t SubField<F>::finalize() {
    assert(!AbstractField::isFinal());
    superField = getObjectFromJson<AbstractField>(KEY_BASE);
    get()->data = superField.get();
    
    percent_t status = initField();
    isFinal(status == 100);
    return status;
  }
  
  template <class F>
  struct json_object*
  SubField<F>::toJson(struct json_object* root, bool returnIdOnly) const {
    json_object* jobj = getJsonStub(root, getId());
    if (jobj) { RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly); }
    else      { jobj = createJsonStub(root, getId()); }
    
    fillJsonObject(jobj, get());
    
    assert(hasSuperField());
    addJsonObject(jobj, KEY_BASE, getSuperField()->toJson(root, true));
    RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly);
  }
  
  template <class F>
  bool SubField<F>::equals(const std::shared_ptr<PbcObject>& other) const {
    FAIL_UNLESS(isFinal());
    std::shared_ptr<SubField<F> > o = std::dynamic_pointer_cast<SubField<F> >(other);
    FAIL_UNLESS(o != nullptr);
    FAIL_UNLESS(o->isFinal());
    FAIL_IF(std::strcmp(get()->name, o->get()->name));
    return (getSuperField()->equals(o->getSuperField()));
  }
}

#endif /* SubField_hpp */
