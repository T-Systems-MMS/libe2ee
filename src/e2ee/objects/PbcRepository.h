//
// Created by Jan Starke on 2019-03-01.
//

#ifndef LIBE2EE_PBCREPOSITORY_H
#define LIBE2EE_PBCREPOSITORY_H

#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/Element.hpp>
#include <e2ee/objects/Pairing.hpp>
#include <boost/uuid/uuid.hpp>
#include <map>
#include <memory>

namespace e2ee {

   enum PbcObjectType {
     PAIRING,
     FIELD,
     ELEMENT
   };

  struct PbcObjectInfo {
    PbcObjectInfo(const boost::uuids::uuid& id, void* object, PbcObjectType type, void* owner)
    : id(id), object(object), type(type), owner(owner) {}

    boost::uuids::uuid id;
    void* object;
    void* owner;
    PbcObjectType type;
  };

  class PbcRepository {
  public:
    PbcRepository() {}
    //~PbcRepository();

    void put(element_ptr obj) {put(obj, ELEMENT);}
    void put(field_ptr obj)   {put(obj, FIELD);}
    void put(pairing_ptr obj) {put(obj, PAIRING);}
/*
    std::shared_ptr<AbstractField> getFieldId(field_ptr obj);
    std::shared_ptr<Element> getElement(element_ptr obj);
    std::shared_ptr<Pairing> getPairing(pairing_ptr obj);
*/
  private:
    void put(void* obj, PbcObjectType type, void* owner = nullptr);

    std::map<void*, std::unique_ptr<PbcObjectInfo> > objects;
  };

}

#endif //LIBE2EE_PBCREPOSITORY_H
