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

#ifndef CurveField_hpp
#define CurveField_hpp

#include <e2ee/objects/CurveField.hpp>
#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/Element.hpp>
#include <pbc.h>
#include <memory>

namespace e2ee {
  
  class CurveField : public AbstractField, public std::enable_shared_from_this<CurveField> {
  public:
    CurveField() = delete;
    CurveField(const CurveField&) = delete;
    CurveField& operator=(const CurveField&) = delete;
    
    CurveField(field_ptr field, std::shared_ptr<ObjectCatalog>& catalog, bool isFinal, const boost::uuids::uuid& id);
    
    bool equals(const std::shared_ptr<PbcObject>& other) const override;
    
    virtual bool operator==(const PbcObject& other) const override {
      return operator==(dynamic_cast<const CurveField&>(other));
    }
    bool operator==(const CurveField& other) const;
    
    json_object* toJson(json_object* root, bool returnIdOnly) const override;
    
    static
    std::shared_ptr<CurveField> construct(struct json_object* jobj, std::shared_ptr<ObjectCatalog>& catalog, const boost::uuids::uuid& id);
    
    percent_t finalize() override;
    
    void updateMembers() override;
    
    void
    set_gen_no_cofac(std::shared_ptr<Element> gen_no_cofac);
    
    void
    set_gen(std::shared_ptr<Element> gen);
    
  private:
    void updateElement(std::shared_ptr<Element>& dst, element_ptr e);
    
    std::shared_ptr<Element> a;
    std::shared_ptr<Element> b;
    std::shared_ptr<__mpz_struct> cofac;
    std::shared_ptr<Element> gen_no_cofac;
    std::shared_ptr<Element> gen;
    bool initialized;
    
  };
  
}

#endif /* CurveField_hpp */
