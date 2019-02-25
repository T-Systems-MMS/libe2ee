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

#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/memory.hpp>
#include <pbc.h>
#include <string>
#include <memory>

namespace e2ee {

  typedef const field_ptr field_cptr;
  
  class AbstractField : public PbcObjectImpl<struct field_s>{
  public:
    typedef PbcObjectImpl<struct field_s> baseclass_t;
    
    AbstractField(const std::string& subtype,
                  bool isFinal,
                  field_ptr field,
                  std::shared_ptr<ObjectCatalog>& catalog,
                  const boost::uuids::uuid& id)
    : PbcObjectImpl<struct field_s>("field", subtype, isFinal, field, id)
    {setObjectCatalog(catalog);}
    
    static
    std::shared_ptr<AbstractField> constructFromNative(field_ptr ptr, std::shared_ptr<ObjectCatalog>& catalog);
    
    virtual ~AbstractField() {}
    
    virtual bool operator==(const PbcObject& other) const override {
      return operator==(static_cast<const AbstractField&>(other));
    }
    virtual bool operator==(const AbstractField& other) const;
    
    static void
    fillJsonObject(json_object* jobj, field_cptr field);
    
    static bool compareField(field_cptr f1, field_cptr f2, std::shared_ptr<ObjectCatalog>&);
    virtual void updateMembers() {}
    
    
    const __mpz_struct * getOrder () const { return &get()->order[0]; }
  protected:
    
    static field_ptr
    parse_native(struct json_object* jobj);
    
  private:
    static const std::string typeId;
  };
  
}

#endif /* AbstractField_hpp */
