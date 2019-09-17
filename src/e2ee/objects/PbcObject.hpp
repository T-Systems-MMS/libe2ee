/*
 * Copyright 2018-2019 T-Systems Multimedia Solutions GmbH
 *
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

#ifndef PbcObject_h
#define PbcObject_h

#include <rapidjson/document.h>
#include <gmp.h>
#include <pbc.h>
#include <string>
#include <forward_list>
#include <functional>
#include <memory>
#include <map>
#include <e2ee/errors.hpp>
#include <e2ee/objects/JsonKey.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <json-c/json_object.h>

#define RETURN_JSON_OBJECT(obj,id,returnIdOnly) \
return ((returnIdOnly)?(createJsonId(id)):(obj))

#define FAIL_IF(condition) \
if (true == (condition)) { return false; }

#define FAIL_UNLESS(condition) \
if (false == (condition)) { return false; }

#define SUCCEED() do { return true; } while (0)
#define FAIL() do { return false; } while (0)

#define FAIL_UNLESS_VALID(a) \
  do { \
    if (! has_##a()) { FAIL(); } \
    if (! a()->isValid()) { FAIL(); } \
  } while (0)

#define PROPERTY(type, name) \
 public: \
       std::shared_ptr<type> name()       { return std::shared_ptr<type>(_##name); } \
 const std::shared_ptr<type> name() const {return std::shared_ptr<type>(_##name);} \
 void set_##name(const std::shared_ptr<type>& val) { _##name = val; } \
 bool has_##name() const { return static_cast<bool>(_##name.lock()); } \
 private: \
 std::weak_ptr<type> _##name

using rapidjson::Document;
using rapidjson::Value;

namespace e2ee {
  template<class T, class U>
  std::shared_ptr<T> dynamic_pointer_cast(const std::shared_ptr<U> &sp) {
    /*
     * try to cast the object to the required type.
     *
     * If this fails, std::bad_cast will be thrown. This check is necessary,
     * because std::dynamic_pointer_cast behaves undefined if you try to
     * cast a non-matching object
     */
    auto& ignore = dynamic_cast<T &>(*sp);
    return std::dynamic_pointer_cast<T>(sp);
  }
}  // namespace e2ee

namespace e2ee {
class PbcContext;

typedef int percent_t;

static constexpr const char TYPE_ELEMENT[]           = "element";
static constexpr const char TYPE_PAIRING[]           = "pairing";
static constexpr const char TYPE_FIELD[]             = "field";

static constexpr const char SUBTYPE_GENERIC[]        = "";

static constexpr const char SUBTYPE_MONTFP[]         = "montfp";
static constexpr const char SUBTYPE_CURVE[]          = "curve";
static constexpr const char SUBTYPE_QUADRATIC[]      = "fi";
static constexpr const char SUBTYPE_MULTIPLICATIVE[] = "mulg";

// https://stackoverflow.com/questions/45507041/how-to-check-if-weak-ptr-is-empty-non-assigned
template <typename T>
bool is_uninitialized(std::weak_ptr<T> const& weak) {
  using wt = std::weak_ptr<T>;
  return !weak.owner_before(wt{}) && !wt{}.owner_before(weak);
}

class PbcObject {
 public:

  PbcObject(std::shared_ptr<PbcContext> ctx,
             const boost::uuids::uuid &id,
             bool isFinal) :
          _isFinal(isFinal),
          id(id == boost::uuids::nil_uuid() ? gen() : id),
          nativeId(boost::uuids::nil_uuid()),
          context(ctx) {
  }

  std::string exportJson() const;

  inline bool documentContainsThis(const Document& doc) const {
    return doc.HasMember(getIdString().c_str());
  }
  virtual void addToJson(Document& doc) const = 0;
  void addJsonObject(Document& doc, Value& dst,
                     const JsonKey& key, const std::shared_ptr<PbcObject>& object) const;

  virtual const std::string &getType() const noexcept = 0;
  virtual const std::string &getSubtype() const noexcept = 0;

  virtual bool isFinal() const noexcept { return _isFinal; }

  virtual bool isValid() const { return true; }

  virtual percent_t finalize(
          const std::map<boost::uuids::uuid, std::shared_ptr<rapidjson::Value>>& values) = 0;

  static
  boost::uuids::uuid idOf(const void *item);

  static inline struct json_object *
  objectById(struct json_object *jobj, const JsonKey &key) {
    struct json_object *o = NULL;
    if (!json_object_object_get_ex(jobj, key.c_str(), &o)) {
      afgh_throw_line("value for key '%s' not found", key.c_str());
    }
    return o;
  }

  static Value mpz_to_json(const mpz_t number,
                           rapidjson::MemoryPoolAllocator<>& allocator);

  static Value limbs_to_json(const mp_limb_t *limbs, mp_size_t size,
                             rapidjson::MemoryPoolAllocator<>& allocator);

  void setId(const boost::uuids::uuid &id);
  void setId(const std::string &idstr);
  const boost::uuids::uuid &getId() const throw() { return id; }
  const std::string getIdString() const throw() { return boost::uuids::to_string(id); }

  void setNativeId(const boost::uuids::uuid &id) { this->nativeId = id; }
  const boost::uuids::uuid &getNativeId() const throw() { return nativeId; }

  /**
   * creates a copy of the json value object
   */
  void setJsonObject(std::shared_ptr<rapidjson::Value> v) {
    this->jobj = v;
  }

  /**
   * returns a reference to the current json value object
   */
  const std::shared_ptr<rapidjson::Value>getJsonObject() const {
    afgh_check(hasJsonObject(), "no JSON object available");
    return jobj.lock();
  }

  bool hasJsonObject() const noexcept {
    if (is_uninitialized(jobj)) {
      return false;
    }
    return (!jobj.expired());
  }

  void setContext(std::weak_ptr<PbcContext> context) {
    this->context = context;
  }
  std::weak_ptr<PbcContext> getContext() const {
    return context;
  }
  std::shared_ptr<PbcContext> lockedContext() const {
    if (auto ctx = context.lock()) {
      return ctx;
    }
    afgh_throw_line("no context available");
  }

 protected:
    /**
     * creates a new Value and stores it in doc.
     * A reference to the newly created value is returned.
     */
  Value& getJsonStub(Document& doc) const;

  static
  struct json_object *createJsonId(const boost::uuids::uuid &id);

  virtual void isFinal(bool f) { _isFinal = f; }

 private:
  static boost::uuids::random_generator gen;
  boost::uuids::uuid id;
  boost::uuids::uuid nativeId;

  bool _isFinal;
  std::weak_ptr<rapidjson::Value> jobj;
  mutable std::weak_ptr<PbcContext> context;
};

template <class T>
struct PbcComparable : public virtual PbcObject {
  bool operator==(const T& other) const {
    static_assert(std::is_base_of<PbcComparable<T>, T>::value);
    assert(isValid());
    assert(other.isValid());

    assert(isFinal());
    assert(other.isFinal());
    return equals(other);
  }

  bool equals(const std::shared_ptr<T>& other) const {
    return this->operator==(*other);
  }

 protected:
  virtual bool equals(const T& other) const = 0;
};
/*
template <class T>
bool operator==(const std::shared_ptr<T>& a, const std::shared_ptr<T>& b) {
  static_assert(std::is_base_of<PbcComparable<T>, T>::value);
  return a->operator==(b);
}

template <class T>
bool operator==(const T& a, const T& b) {
  static_assert(std::is_base_of<PbcComparable<T>, T>::value);
  return a.operator==(b);
}*/

}  // namespace e2ee

#endif /* PbcObject_h */
