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

#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/MontFPField.hpp>
#include <e2ee/PbcContext.hpp>
extern "C" {
#include <pbc.h>
#include <pbc_fp.h>
}

namespace e2ee {
MontFPField::MontFPField(std::shared_ptr<PbcContext> context,
                         const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
                         const boost::uuids::uuid &id,
                         const rapidjson::Value &value)
        : PbcObject(context, id, false), AbstractField(value) {
  // pbc_tweak_use_fp((char*)"mont");
  // montfp_data *data = allocate_unmanaged<montfp_data>();
  afgh_mpz_t primelimbs = json_to_mpz(value[KEY_MODULUS.c_str()]);
  field_init_mont_fp(get(), primelimbs.release());

  // auto ptr = json_to_limbs(value[KEY_NEGPINV.c_str()]);
  // data->negpinv = (*ptr);
  // data->R = json_to_limbs(value[KEY_R.c_str()]).release();
  // data->R3 = json_to_limbs(value[KEY_R3.c_str()]).release();
  // get()->data = data;
}

void MontFPField::addToJson(Document& doc) const {
  assert(isFinal());
  if (documentContainsThis(doc)) {
    return;
  }
  AbstractField::addToJson(doc);

  auto& self = getJsonStub(doc);
  const montfp_data *data = (montfp_data *) get()->data;

  self.AddMember(KEY_MODULUS,
          limbs_to_json(data->primelimbs, data->limbs, doc.GetAllocator()).Move(),
          doc.GetAllocator());
  self.AddMember(KEY_NEGPINV,
          limbs_to_json(&(data->negpinv), 1, doc.GetAllocator()).Move(),
          doc.GetAllocator());
  self.AddMember(KEY_R,
          limbs_to_json(data->R, data->limbs, doc.GetAllocator()).Move(),
          doc.GetAllocator());
  self.AddMember(KEY_R3,
          limbs_to_json(data->R3, data->limbs, doc.GetAllocator()).Move(),
          doc.GetAllocator());
}

bool MontFPField::equals(const MontFPField &other) const {
  if (!AbstractField::equals(other)) {
    return false;
  }
  montfp_data *a_data = (montfp_data *) get()->data;
  montfp_data *b_data = (montfp_data *) other.get()->data;
  if (0 != mpn_cmp(&(a_data->negpinv), &(b_data->negpinv), 1)) { return false; }
  if (0 != mpn_cmp(a_data->primelimbs, b_data->primelimbs, a_data->limbs)) { return false; }
  if (0 != mpn_cmp(a_data->R, b_data->R, a_data->limbs)) { return false; }
  if (0 != mpn_cmp(a_data->R3, b_data->R3, a_data->limbs)) { return false; }
  return true;
}

std::shared_ptr<Element>
MontFPField::elementFromBytes(
        std::vector<std::byte>::const_iterator begin,
        std::vector<std::byte>::const_iterator end) const {
  std::vector<std::byte> buffer(begin, end);

  auto e = emptyElement();
  int bytes = 0;
  bytes = element_from_bytes(e->get(),
                             reinterpret_cast<unsigned char *>(&buffer[0]));
  e->updateStringValue();
  afgh_check(bytes == buffer.size(),
             "invalid number of bytes read. expected %d, but read %d bytes.",
             buffer.size(), bytes);

  #ifndef NDEBUG
  auto b = e->toBytes();
  auto b2 = (e->operator!())->toBytes();
  assert(std::equal(begin, end, b.begin()) ||
         std::equal(begin, end, b2.begin()));
  #endif

  return e;
}

}  // namespace e2ee
