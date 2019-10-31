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

#include <e2ee/objects/CurveField.hpp>
#include <map>
#include <vector>
#include <pbc.h>
#include <json-c/json_object.h>
#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/PbcContext.hpp>
#include <e2ee/json.hpp>
#include <aixlog.hpp>

namespace e2ee {

CurveField::CurveField(std::shared_ptr<PbcContext> context, const boost::uuids::uuid &id,
                       const field_s* field, bool isFinal)
        : PbcObject(context, id, isFinal), AbstractField(field),
          initialized(false) {
}


CurveField::CurveField(std::shared_ptr<PbcContext> context,
                       const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
                       const boost::uuids::uuid &id,
                       const rapidjson::Value &value)
                       : PbcObject(context, id, false), AbstractField(value),
                       initialized(false) {
  assert(!mpz_is0(&(this->get()->order[0])));
}

void CurveField::updateMembers() {
  assert(isFinal());
  const curve_data *data = reinterpret_cast<curve_data *>(get()->data);
  if (auto ctx = lockedContext()) {
    set_a(ctx->fromNative(&data->a[0]));
    set_b(ctx->fromNative(&data->b[0]));
    //set_gen(ctx->fromNative(&data->gen[0]));
    //set_gen_no_cofac(ctx->fromNative(&data->gen_no_cofac[0]));
  }
}

void CurveField::addToJson(Document& doc) const {
  assert(isFinal());
  if (documentContainsThis(doc)) {
    return;
  }
  AbstractField::addToJson(doc);

  auto& self = getJsonStub(doc);
  const curve_data *data = (curve_data *) get()->data;

  addJsonObject(doc, self, KEY_A, a());
  addJsonObject(doc, self, KEY_B, b());

  if (data->cofac != nullptr) {
    self.AddMember(KEY_COFAC,
            mpz_to_json(data->cofac, doc.GetAllocator()).Move(),
            doc.GetAllocator());
  }
  if (data->quotient_cmp != NULL) {
    self.AddMember(KEY_QUOTIENTCMP,
                   mpz_to_json(data->quotient_cmp, doc.GetAllocator()).Move(),
                   doc.GetAllocator());
  }
}

bool CurveField::isValid() const {
  FAIL_UNLESS_VALID(a);
  FAIL_UNLESS_VALID(b);
  FAIL_UNLESS(get()->init != NULL);
  return true;
}

percent_t
CurveField::finalize(
        const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values) {
  percent_t status = 0;
  assert(!isFinal());
  if (!has_a()) {
    set_a(elementFromJson(values, KEY_A));
    assert(has_a());
    status = 10;
  }
  if (!has_b()) {
    set_b(elementFromJson(values, KEY_B));
    assert(has_b());
    status = 20;
  }
  if (!has_a() || !has_b()) {
    return 30;
  }

  if (! a()->isFinal()) { return 40; }
  if (! b()->isFinal()) { return 50; }

  if (!initialized) {
    auto jobj = getJsonObject();
    auto order = str_to_mpz(JSON_GET_STRING(*jobj, KEY_ORDER));
    auto cofac = str_to_mpz(JSON_GET_STRING(*jobj, KEY_COFAC));
    assert(!mpz_is0(order.get()));
    auto order_ptr = order.release();
    auto cofac_ptr = cofac.release();
    field_init_curve_ab(this->get(), a()->get(), b()->get(),
                        order_ptr,
                        cofac_ptr);
    initialized = true;
  }
  assert(initialized);
  status = 60;

  isFinal(initialized );
  if (isFinal()) {
    status = 100;
    assert(isValid());
  }
  return status;
}

bool CurveField::equals(const CurveField &other) const {
  FAIL_UNLESS(isFinal());
  FAIL_UNLESS(other.isFinal());
  if (!AbstractField::equals(other)) {
    return false;
  }

  curve_data *a_data = (curve_data *) get()->data;
  curve_data *b_data = (curve_data *) other.get()->data;

  if (0 != element_cmp(a_data->a, b_data->a)) { return false; }
  if (0 != element_cmp(a_data->b, b_data->b)) { return false; }
  if (0 != element_cmp(a_data->gen, b_data->gen)) { return false; }
  if (0 != element_cmp(a_data->gen_no_cofac, b_data->gen_no_cofac)) { return false; }
  if (0 != mpz_cmp(a_data->cofac, b_data->cofac)) { return false; }
  if (a_data->quotient_cmp == NULL && b_data->quotient_cmp != NULL) { return false; }
  if (a_data->quotient_cmp != NULL && b_data->quotient_cmp == NULL) { return false; }
  if (a_data->quotient_cmp != NULL && b_data->quotient_cmp != NULL) {
    if (0 != mpz_cmp(a_data->quotient_cmp, b_data->quotient_cmp)) { return false; }
  }
  return true;
}

std::shared_ptr<Element>
CurveField::elementFromBytes(
        std::vector<std::byte>::const_iterator begin,
        std::vector<std::byte>::const_iterator end) const {
  std::vector<std::byte> buffer(begin, end);

  auto e = emptyElement();
  int bytes = 0;
  bytes = element_from_bytes_x_only(e->get(),
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
}
