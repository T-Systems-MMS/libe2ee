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

#ifndef MultiplicativeGroup_h
#define MultiplicativeGroup_h

#include <e2ee/objects/SubField.hpp>
#include <memory>

namespace e2ee {
  
class MultiplicativeSubgroup : public SubField<MultiplicativeSubgroup> {
  public:
    static constexpr char TYPE_ID[] = "mulg";
    static constexpr char SUBTYPE_ID[] = "";
    
    MultiplicativeSubgroup() = delete;
    MultiplicativeSubgroup(const MultiplicativeSubgroup&) = delete;
    MultiplicativeSubgroup& operator=(const MultiplicativeSubgroup&) = delete;
    
    MultiplicativeSubgroup(field_ptr field,
                           std::shared_ptr<ObjectCatalog>& catalog,
                           bool isFinal,
                           const boost::uuids::uuid& id);
    
    virtual json_object* toJson(json_object* root, bool returnIdOnly = false) const override;
    std::weak_ptr<Pairing> getPairing() const { return pairing; }


  virtual void updateMembers() override;
    
  private:
    std::weak_ptr<Pairing> pairing;
  };
  
}

#endif /* MultiplicativeGroup_h */
