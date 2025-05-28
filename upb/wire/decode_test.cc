// Protocol Buffers - Google's data interchange format
// Copyright 2025 Google LLC.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "upb/wire/decode.h"

#include <cstddef>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include "absl/container/flat_hash_set.h"
#include "absl/log/absl_check.h"
#include "upb/base/status.hpp"
#include "upb/mem/alloc.h"
#include "upb/mem/arena.h"
#include "upb/mem/arena.hpp"
#include "upb/message/accessors.h"
#include "upb/message/accessors.hpp"
#include "upb/message/message.h"
#include "upb/mini_descriptor/decode.h"
#include "upb/mini_descriptor/internal/encode.hpp"
#include "upb/mini_descriptor/internal/modifiers.h"
#include "upb/mini_table/field.h"
#include "upb/mini_table/message.h"
#include "upb/wire/decode_fast/combinations.h"
#include "upb/wire/test_util/field_types.h"
#include "upb/wire/test_util/wire_message.h"

// Must be last.
#include "upb/port/def.inc"

namespace upb {
namespace test {

namespace {

int FieldModifiers(upb_DecodeFast_Type type,
                   upb_DecodeFast_Cardinality cardinality) {
  int modifiers = 0;
  switch (type) {
    case kUpb_DecodeFast_String:
      modifiers |= kUpb_FieldModifier_ValidateUtf8;
      break;
    default:
      break;
  }
  switch (cardinality) {
    case kUpb_DecodeFast_Repeated:
      modifiers |= kUpb_FieldModifier_IsRepeated;
      break;
    case kUpb_DecodeFast_Packed:
      modifiers |= kUpb_FieldModifier_IsRepeated | kUpb_FieldModifier_IsPacked;
      break;
    default:
      return 0;
  }
  return modifiers;
}

class MiniTable {
 public:
  template <typename Field>
  static std::pair<const upb_MiniTable*, const upb_MiniTableField*>
  MakeSingleFieldTable(int field_number, upb_DecodeFast_Cardinality cardinality,
                       upb_Arena* arena) {
    MtDataEncoder encoder;
    encoder.StartMessage(0);
    encoder.PutField(Field::kFieldType, 1,
                     FieldModifiers(Field::kFastType, cardinality));
    if (cardinality == kUpb_DecodeFast_Oneof) {
      encoder.StartOneof();
      encoder.PutOneofField(field_number);
    }
    const std::string& data = encoder.data();
    upb::Status status;
    const upb_MiniTable* table =
        upb_MiniTable_Build(data.data(), data.size(), arena, status.ptr());
    ABSL_CHECK(status.ok()) << status.error_message();
    const upb_MiniTableField* field = upb_MiniTable_GetFieldByIndex(table, 0);
    ABSL_CHECK(field != nullptr);
#if UPB_FASTTABLE
    if (field_number < (1 << 11)) {
      ABSL_CHECK_EQ(HasFastTableEntry(table, field),
                    UPB_DECODEFAST_ISENABLED(Field::kFastType, cardinality,
                                             kUpb_DecodeFast_Tag1Byte))
          << Field::kName;
    }
#endif
    return std::make_pair(table, upb_MiniTable_GetFieldByIndex(table, 0));
  }
};

template <typename T>
std::optional<T> GetOptionalField(upb_Message* msg, const upb_MiniTable* mt,
                                  int field_number) {
  const upb_MiniTableField* field =
      upb_MiniTable_FindFieldByNumber(mt, field_number);
  ABSL_CHECK(field != nullptr);
  if (!upb_Message_HasBaseField(msg, field)) {
    return std::nullopt;
  }
  return GetMessageBaseField<T>(msg, field, T{});
}

template <typename T>
class FieldTypeTest : public testing::Test {};

TYPED_TEST_SUITE(FieldTypeTest, FieldTypes);

TYPED_TEST(FieldTypeTest, DecodeOptionalMaxValue) {
  using Value = typename TypeParam::Value;
  Value value;
  if constexpr (std::is_same_v<Value, std::string>) {
    for (int i = 0; i < 1000; ++i) {
      value.append("hello world! ");
    }
  } else {
    value = std::numeric_limits<Value>::max();
  }
  upb::Arena arena;
  auto [mt, field] = MiniTable::MakeSingleFieldTable<TypeParam>(
      1, kUpb_DecodeFast_Scalar, arena.ptr());
  upb_Message* msg = upb_Message_New(mt, arena.ptr());
  std::string payload = ToBinaryPayload(
      wire_types::WireMessage{{1, TypeParam::WireValue(value)}});
  upb_DecodeStatus result = upb_Decode(payload.data(), payload.size(), msg, mt,
                                       nullptr, 0, arena.ptr());
  ASSERT_EQ(result, kUpb_DecodeStatus_Ok) << upb_DecodeStatus_String(result);
  EXPECT_EQ(GetOptionalField<Value>(msg, mt, 1), value);
}

TYPED_TEST(FieldTypeTest, DecodeOptionalMinValue) {
  using Value = typename TypeParam::Value;
  Value value;
  if constexpr (!std::is_same_v<Value, std::string>) {
    value = std::numeric_limits<Value>::max();
  }
  upb::Arena arena;
  auto [mt, field] = MiniTable::MakeSingleFieldTable<TypeParam>(
      1, kUpb_DecodeFast_Scalar, arena.ptr());
  upb_Message* msg = upb_Message_New(mt, arena.ptr());
  std::string payload = ToBinaryPayload(
      wire_types::WireMessage{{1, TypeParam::WireValue(value)}});
  upb_DecodeStatus result = upb_Decode(payload.data(), payload.size(), msg, mt,
                                       nullptr, 0, arena.ptr());
  ASSERT_EQ(result, kUpb_DecodeStatus_Ok) << upb_DecodeStatus_String(result);
  EXPECT_EQ(GetOptionalField<Value>(msg, mt, 1), value);
}

TYPED_TEST(FieldTypeTest, DecodeOneofMaxValue) {
  using Value = typename TypeParam::Value;
  Value value = std::numeric_limits<Value>::max();
  upb::Arena arena;
  auto [mt, field] = MiniTable::MakeSingleFieldTable<TypeParam>(
      1, kUpb_DecodeFast_Oneof, arena.ptr());
  upb_Message* msg = upb_Message_New(mt, arena.ptr());
  std::string payload = ToBinaryPayload(
      wire_types::WireMessage{{1, TypeParam::WireValue(value)}});
  upb_DecodeStatus result = upb_Decode(payload.data(), payload.size(), msg, mt,
                                       nullptr, 0, arena.ptr());
  ASSERT_EQ(result, kUpb_DecodeStatus_Ok) << upb_DecodeStatus_String(result);
  EXPECT_EQ(GetOptionalField<Value>(msg, mt, 1), value);
}

void BM_Decode(benchmark::State& state, const upb_MiniTable* mt,
               std::string payload, bool alias, bool initial_block) {
  char mem[4096];
  int decode_options = 0;
  for (auto s : state) {
    upb_Arena* arena = initial_block
                           ? upb_Arena_Init(mem, 4096, &upb_alloc_global)
                           : upb_Arena_New();
    upb_Message* msg = upb_Message_New(mt, arena);
    upb_DecodeStatus result = upb_Decode(payload.data(), payload.size(), msg,
                                         mt, nullptr, decode_options, arena);
    ASSERT_EQ(result, kUpb_DecodeStatus_Ok) << upb_DecodeStatus_String(result);
    upb_Arena_Free(arena);
  }
  state.SetBytesProcessed(state.iterations() * payload.size());
}

[[maybe_unused]] const bool benchmark_registration = [] {
  static upb_Arena* arena = upb_Arena_New();
  std::vector<size_t> sizes{8, 64, 512};
  for (size_t size : sizes) {
    absl::flat_hash_set<upb_DecodeFast_Type> fast_types;
    ForEachType([&](auto&& type) {
      using Type = std::remove_reference_t<decltype(type)>;
      // We only need to benchmark each wire type once, since they are all
      // treated the same by the decoder.
      if (!fast_types.insert(Type::kFastType).second) return;
      std::vector<bool> initial_block{true, false};
      for (bool init : initial_block) {
        auto [mt, field] = MiniTable::MakeSingleFieldTable<Type>(
            1, kUpb_DecodeFast_Scalar, arena);
        std::string payload;
        while (payload.size() < size) {
          payload.append(ToBinaryPayload(wire_types::WireMessage{
              {1, Type::WireValue(typename Type::Value{})}}));
        }
        ::benchmark::RegisterBenchmark(
            absl::StrFormat("BM_Decode/%s/%zu/%s", Type::kName, size,
                            init ? "InitialBlock" : "NoInitialBlock"),
            BM_Decode, mt, payload, false, init);
      }
    });
  }
  return false;
}();

}  // namespace

}  // namespace test
}  // namespace upb
