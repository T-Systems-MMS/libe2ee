//
// Created by Jan Starke on 2019-03-01.
//

#include "PbcRepository.h"

namespace e2ee {

  void PbcRepository::put(void* obj, PbcObjectType type, void* owner) {
    if (obj == nullptr) {
      return;
    }

    if (owner != nullptr) {
      assert(objects.find(owner) != objects.end());
    }

    auto iter = objects.find(obj);
    if (iter != objects.end()) {
      assert(iter->second->type == type);
      return;
    }

    const boost::uuids::uuid id = PbcObject::idOf(obj);
    objects.insert(std::make_pair(obj, std::make_unique<PbcObjectInfo>(id, obj, type, owner)));

    /*
     * recursively add all contained objects
     */
    /*
    switch(type) {

      case PbcObjectType::ELEMENT:
        element_ptr element = reinterpret_cast<element_ptr>(obj);
        put(element->field, ELEMENT, obj);
        break;

      case PbcObjectType::FIELD:
        field_ptr field = reinterpret_cast<field_ptr>(obj);
        put(field->pairing, PAIRING, obj);
        const std::string name = field->name;
        if (name == "curve") {
          curve_data_ptr cdp = reinterpret_cast<curve_data_ptr>(field->data);
          put(cdp->field, FIELD, obj);
          put(cdp->gen_no_cofac, ELEMENT, obj);
          put(cdp->gen, ELEMENT, obj);
          put(cdp->a, ELEMENT, obj);
          put(cdp->b, ELEMENT, obj);
        }
        break;

      default:
        afgh_throw_line("invalid object type");
    }
     */
  }
}