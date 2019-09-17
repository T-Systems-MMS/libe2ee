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

#ifndef MontFPField_hpp
#define MontFPField_hpp

#include <e2ee/objects/AbstractField.hpp>
#include <memory>

namespace e2ee {
  
  class MontFPField:
          public PbcObjectTypeIdentifier<TYPE_FIELD, SUBTYPE_MONTFP>,
          public AbstractField, PbcComparable<MontFPField> {
  public:

    MontFPField() = delete;
    MontFPField(const MontFPField&) = delete;
    MontFPField& operator=(const MontFPField&) = delete;
    
    MontFPField(
            std::shared_ptr<PbcContext> context,
            const boost::uuids::uuid& id,
            const field_s* field,
            bool isFinal):
            PbcObject(context, id, isFinal),
            AbstractField (field) {}

    MontFPField(std::shared_ptr<PbcContext> context,
                const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values,
                const boost::uuids::uuid &id,
                const rapidjson::Value &value);

    using PbcObjectTypeIdentifier::getType;
    using PbcObjectTypeIdentifier::getSubtype;

    bool equals(const MontFPField& other) const final;

    void addToJson(Document& doc) const override;

    void updateMembers() override {}
    percent_t finalize(
            const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values) override { isFinal(true); return 100;}
  };
  
}

#endif /* MontFPField_hpp */
