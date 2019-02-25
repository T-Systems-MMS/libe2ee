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

#ifndef Pairing_hpp
#define Pairing_hpp

#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/MultiplicativeSubgroup.hpp>
#include <e2ee/objects/MontFPField.hpp>
#include <pbc.h>

namespace e2ee {
  class Element;
  
  class Pairing
  : public PbcObjectImpl<struct pairing_s> {
  public:
    Pairing(int32_t rBits, int32_t qBits, const boost::uuids::uuid id = boost::uuids::nil_uuid());
    
    Pairing(pairing_ptr pairing, bool isFinal)
    : PbcObjectImpl("pairing", "", isFinal, pairing) {}
    
    virtual ~Pairing();
    
    std::shared_ptr<Element> initG1();
    std::shared_ptr<Element> initG2();
    std::shared_ptr<Element> initGT();
    std::shared_ptr<Element> initZr();
    
    std::unique_ptr<Element> apply(const std::shared_ptr<Element>& g1,
                                   const std::shared_ptr<Element>& g2) const;
    
    bool equals(const std::shared_ptr<PbcObject>& other) const override;
    
    virtual bool operator==(const PbcObject& other) const override {
      return operator==(dynamic_cast<const Pairing&>(other));
    }
    virtual bool operator==(const Pairing& other) const;
    
    struct json_object*
    toJson(struct json_object* root, bool returnIdOnly = false) const override;
    
    static
    std::shared_ptr<Pairing> construct(struct json_object* jobj, std::shared_ptr<ObjectCatalog>& catalog, const boost::uuids::uuid& id);
    
    percent_t finalize() override;

    bool isFinal() const throw() override { return PbcObject::isFinal(); }
    
    std::shared_ptr<AbstractField> getG1() const throw() { return G1; }
    std::shared_ptr<AbstractField> getG2() const throw() { return G2; }
    std::shared_ptr<MultiplicativeSubgroup> getGT() const throw() { return GT; }
    std::shared_ptr<MontFPField> getZr() const throw() { return Zr; }
    
  protected:
    void isFinal(bool f) override;
    
  private:
    mutable
    std::shared_ptr<ObjectCatalog> catalog;
    
    std::shared_ptr<AbstractField> G1;
    std::shared_ptr<AbstractField> G2;
    std::shared_ptr<MultiplicativeSubgroup> GT;
    std::shared_ptr<MontFPField> Zr;
    
    bool isGTinitialized = false;
  };
  
}

#endif /* Pairing_hpp */
