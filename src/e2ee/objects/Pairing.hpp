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

#include <memory>
#include <e2ee/objects/PbcObjectImpl.hpp>
#include <e2ee/objects/AbstractField.hpp>
#include <e2ee/objects/MultiplicativeSubgroup.hpp>
#include <e2ee/objects/MontFPField.hpp>
#include <pbc.h>

namespace e2ee {
class Element;

class Pairing :
        public PbcObjectImpl<struct pairing_s>,
                public PbcComparable<Pairing> {
 public:
  Pairing(std::shared_ptr<PbcContext> context,
          const boost::uuids::uuid &id, const pairing_s *pairing, bool isFinal);

  Pairing(std::shared_ptr<PbcContext> context, const pairing_s *pairing, bool isFinal) :
          Pairing(context, boost::uuids::nil_uuid(), pairing, isFinal) {}

  Pairing(std::shared_ptr<PbcContext> context,
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
          const boost::uuids::uuid &id,
          const rapidjson::Value &value);

  Pairing(std::shared_ptr<PbcContext> context, int32_t rBits, int32_t qBits,
          const boost::uuids::uuid &id = boost::uuids::nil_uuid());

 public:
  virtual ~Pairing();

  const std::string &getType() const noexcept override {
    static std::string value = TYPE_PAIRING;
    return value;
  }

  const std::string &getSubtype() const noexcept override {
    static std::string value = SUBTYPE_GENERIC;
    return value;
  }

  std::shared_ptr<Element> initG1();
  std::shared_ptr<Element> initG2();
  std::shared_ptr<Element> initGT();
  std::shared_ptr<Element> initZr();
  std::shared_ptr<Element> apply( const std::shared_ptr<Element> &g1,
                                  const std::shared_ptr<Element> &g2) const;

  bool equals(const Pairing &other) const final;

  void addToJson(Document& doc) const override;

  percent_t finalize(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values) override;

  bool isFinal() const throw() override { return PbcObject::isFinal(); }

 protected:
  void isFinal(bool f) override;

 private:
  int initMethod = 0;
  PROPERTY(AbstractField, G1);
  PROPERTY(AbstractField, G2);
  PROPERTY(AbstractField, GT);
  PROPERTY(AbstractField, Zr);

  PROPERTY(AbstractField, Eq);
  PROPERTY(AbstractField, Fq);
  PROPERTY(AbstractField, Fq2);

  bool isGTinitialized = false;

  void initFields(const std::shared_ptr<PbcContext> &ctx, const pairing_s *ptr);
};

}

#endif /* Pairing_hpp */
