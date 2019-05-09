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

#include <e2ee/objects/MultiplicativeSubgroup.hpp>
#include <e2ee/objects/SubField.hpp>
#include <e2ee/objects/Pairing.hpp>
#include <e2ee/objects/JsonKey.hpp>
#include <e2ee/ObjectCatalog.hpp>

namespace e2ee {
  
  MultiplicativeSubgroup::MultiplicativeSubgroup(field_ptr field, std::shared_ptr<ObjectCatalog>& catalog, bool isFinal, const boost::uuids::uuid& id)
  : SubField(field, catalog, isFinal, id) {
  }

  void MultiplicativeSubgroup::updateMembers() {
    AbstractField::updateMembers();
    if (get()->pairing) {
      pairing = (*getCatalog())[get()->pairing];
    }
  }
  
  struct json_object*
  MultiplicativeSubgroup::toJson(struct json_object* root, bool returnIdOnly) const {
    json_object* jobj = SubField<MultiplicativeSubgroup>::toJson(root, false);
    if (auto p = getPairing().lock()) {
      addJsonObject(jobj, KEY_PAIRING, p->toJson(root, true));
    } else {
      afgh_throw_line("incomplete mulg field, I won't export this sh*t");
    }
    RETURN_JSON_OBJECT(jobj, getId(), returnIdOnly);
  }
}
