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

#include <e2ee/objects/SubField.hpp>
#include <memory>

namespace e2ee {
  
  class QuadraticField : public SubField<QuadraticField> {
  public:
    static constexpr char TYPE_ID[] = "field";
    static constexpr char SUBTYPE_ID[] = "fi";
    
    QuadraticField() = delete;
    QuadraticField(const QuadraticField&) = delete;
    QuadraticField& operator=(const QuadraticField&) = delete;
    
    QuadraticField(field_ptr field, std::shared_ptr<ObjectCatalog>& catalog, bool isFinal, const boost::uuids::uuid& id)
    : SubField(field, catalog, isFinal, id) {}
    
  protected:
    percent_t initField() override;
  };
  
}

#endif /* QuadraticField_hpp */
