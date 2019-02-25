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

#ifndef Element_hpp
#define Element_hpp

#include <e2ee/objects/PbcObject.hpp>
#include <e2ee/objects/PbcObjectImpl.hpp>
#include <pbc.h>
#include <memory>
#include <list>

namespace e2ee {
  
  class Pairing;
  class AbstractField;
  
  class Element : public PbcObjectImpl<struct element_s> {
  public:
    
    Element(element_ptr element, std::shared_ptr<ObjectCatalog>& catalog, bool isFinal, const boost::uuids::uuid& id);
    Element(std::shared_ptr<AbstractField> field, element_ptr element, bool isFinal=false);
    Element(std::shared_ptr<AbstractField> field);
    
    std::shared_ptr<Element> initSameAs() const;
    Element& randomize();
    
    bool equals(const std::shared_ptr<PbcObject>& other) const override;
    
    virtual bool operator==(const PbcObject& other) const override {
      return operator==(dynamic_cast<const Element&>(other));
    }
    virtual bool operator==(const Element& other) const;
    
    std::unique_ptr<Element> operator ^(const Element& e) const;
    std::unique_ptr<Element> operator ^(const std::shared_ptr<Element>& e) const {
      return this->operator^(*e);
    }
    std::unique_ptr<Element> operator *(const Element& e) const;
    std::unique_ptr<Element> operator *(const std::shared_ptr<Element>& e) const {
      return this->operator*(*e);
    }
    
    std::unique_ptr<Element> operator /(const Element& e) const;
    std::unique_ptr<Element> operator /(const std::shared_ptr<Element>& e) const {
      return this->operator/(*e);
    }
    
    std::unique_ptr<Element> operator !() const;

    struct json_object* toJson(struct json_object* root, bool returnIdOnly = false) const override;
    
    static
    std::shared_ptr<Element> construct(struct json_object* jobj, std::shared_ptr<ObjectCatalog>& catalog, const boost::uuids::uuid& id);
    
    percent_t finalize() override;
    std::shared_ptr<AbstractField> getField() const { return field; }
    
    static std::string
    formatElementComponents(const std::list<afgh_mpz_t>& values);
    
    static const std::string& getTypeId() throw() { return typeId; }
    static const std::string& getSubtypeId() throw() { return subtypeId; }
    
  private:
    static const std::string typeId;
    static const std::string subtypeId;
    std::shared_ptr<AbstractField> field;
    std::string value;
    
    static
    void addNumberToJson(json_object* jobj,
                         const JsonKey& key,
                         const std::string::const_iterator begin,
                         const std::string::const_iterator end);
  };
  
}
#endif /* Element_hpp */
