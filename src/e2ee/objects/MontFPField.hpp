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

#ifndef MontFPField_hpp
#define MontFPField_hpp

#include <e2ee/objects/AbstractField.hpp>
#include <memory>

namespace e2ee {
  
  class MontFPField
  : public  AbstractField {
  public:
    static constexpr char TYPE_ID[] = "field";
    static constexpr char SUBTYPE_ID[] = "montfp";
    
    MontFPField() = delete;
    MontFPField(const MontFPField&) = delete;
    MontFPField& operator=(const MontFPField&) = delete;
    
    MontFPField(field_ptr field, std::shared_ptr<ObjectCatalog>& catalog, bool isFinal, const boost::uuids::uuid& id)
    : AbstractField (SUBTYPE_ID, isFinal, field, catalog, id) {}
    
    bool equals(const std::shared_ptr<PbcObject>& other) const override;
    
    bool operator==(const PbcObject& other) const override {
      return operator==(dynamic_cast<const MontFPField&>(other));
    }
    bool operator==(const MontFPField& other) const;
    
    json_object* toJson(json_object* root, bool returnIdOnly = false) const override;
    
    static
    std::shared_ptr<MontFPField> construct(struct json_object* jobj, std::shared_ptr<ObjectCatalog>& catalog, const boost::uuids::uuid& id);
    
    percent_t finalize() override { isFinal(true); return 100;}
  };
  
}

#endif /* MontFPField_hpp */
