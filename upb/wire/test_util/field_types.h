#ifndef GOOGLE_UPB_UPB_WIRE_TEST_WIRE_TYPES_H__
#define GOOGLE_UPB_UPB_WIRE_TEST_WIRE_TYPES_H__

#include <cstdint>
#include <string>

#include <gtest/gtest.h>
#include "absl/base/casts.h"
#include "absl/strings/string_view.h"
#include "upb/base/descriptor_constants.h"
#include "upb/wire/decode_fast/combinations.h"
#include "upb/wire/test_util/wire_message.h"

namespace upb {
namespace test {
namespace field_types {

// This set of field types is similar to upb_FieldType, but it also includes
// some extra distinctions like closed vs. open enum and validated vs.
// unvalidated UTF-8.

struct Fixed32 {
  using Value = uint32_t;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_Fixed32;
  inline static constexpr absl::string_view kName = "Fixed32";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_Fixed32;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Fixed32(value);
  }
};

struct Fixed64 {
  using Value = uint64_t;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_Fixed64;
  inline static constexpr absl::string_view kName = "Fixed64";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_Fixed64;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Fixed64(value);
  }
};

struct SFixed32 {
  using Value = int32_t;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_SFixed32;
  inline static constexpr absl::string_view kName = "SFixed32";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_Fixed32;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Fixed32(static_cast<uint32_t>(value));
  }
};

struct SFixed64 {
  using Value = int64_t;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_SFixed64;
  inline static constexpr absl::string_view kName = "SFixed64";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_Fixed64;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Fixed64(static_cast<uint64_t>(value));
  }
};

struct Float {
  using Value = float;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_Float;
  inline static constexpr absl::string_view kName = "Float";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_Fixed32;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Fixed32(absl::bit_cast<uint32_t>(value));
  }
};

struct Double {
  using Value = double;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_Double;
  inline static constexpr absl::string_view kName = "Double";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_Fixed64;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Fixed64(absl::bit_cast<uint64_t>(value));
  }
};

struct Int32 {
  using Value = int32_t;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_Int32;
  inline static constexpr absl::string_view kName = "Int32";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_Varint32;

  static wire_types::WireValue WireValue(Value value) {
    // Need to sign-extend to 64-bit varint.
    return wire_types::Varint(static_cast<int64_t>(value));
  }
};

struct Int64 {
  using Value = int64_t;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_Int64;
  inline static constexpr absl::string_view kName = "Int64";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_Varint64;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Varint(value);
  }
};

struct UInt32 {
  using Value = uint32_t;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_UInt32;
  inline static constexpr absl::string_view kName = "UInt32";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_Varint32;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Varint(value);
  }
};

struct UInt64 {
  using Value = uint64_t;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_UInt64;
  inline static constexpr absl::string_view kName = "UInt64";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_Varint64;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Varint(value);
  }
};

struct SInt32 {
  using Value = int32_t;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_SInt32;
  inline static constexpr absl::string_view kName = "SInt32";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_ZigZag32;

  static wire_types::WireValue WireValue(Value value) {
    uint32_t uvalue = ((uint32_t)value << 1) ^ (value >> 31);
    return wire_types::Varint(uvalue);
  }
};

struct SInt64 {
  using Value = int64_t;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_SInt64;
  inline static constexpr absl::string_view kName = "SInt64";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_ZigZag64;

  static wire_types::WireValue WireValue(Value value) {
    uint64_t uvalue = ((uint64_t)value << 1) ^ (value >> 63);
    return wire_types::Varint(uvalue);
  }
};

struct Bool {
  using Value = bool;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_Bool;
  inline static constexpr absl::string_view kName = "Bool";
  inline static constexpr upb_DecodeFast_Type kFastType = kUpb_DecodeFast_Bool;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Varint(value);
  }
};

struct String {
  using Value = std::string;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_String;
  inline static constexpr absl::string_view kName = "String";
  inline static constexpr upb_DecodeFast_Type kFastType =
      kUpb_DecodeFast_String;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Delimited(value);
  }
};

struct Bytes {
  using Value = std::string;
  inline static constexpr upb_FieldType kFieldType = kUpb_FieldType_Bytes;
  inline static constexpr absl::string_view kName = "Bytes";
  inline static constexpr upb_DecodeFast_Type kFastType = kUpb_DecodeFast_Bytes;

  static wire_types::WireValue WireValue(Value value) {
    return wire_types::Delimited(value);
  }
};

// TODO: Message, Group, ClosedEnum

}  // namespace field_types

using FieldTypes =
    testing::Types<field_types::Fixed32, field_types::Fixed64,
                   field_types::SFixed32, field_types::SFixed64,
                   field_types::Float, field_types::Double, field_types::Int32,
                   field_types::Int64, field_types::UInt32, field_types::UInt64,
                   field_types::SInt32, field_types::SInt64, field_types::Bool,
                   field_types::String>;

template <typename Func, typename... Ts>
void ForEachTypeImpl(Func&& func, testing::Types<Ts...>) {
  (std::forward<Func>(func)(Ts{}), ...);
}

template <typename Func>
void ForEachType(Func&& func) {
  ForEachTypeImpl(std::forward<Func>(func), FieldTypes{});
}

}  // namespace test
}  // namespace upb

#endif  // GOOGLE_UPB_UPB_WIRE_TEST_WIRE_TYPES_H__
