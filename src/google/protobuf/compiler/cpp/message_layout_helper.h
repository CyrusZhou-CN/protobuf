// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// Author: seongkim@google.com (Seong Beom Kim)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#ifndef GOOGLE_PROTOBUF_COMPILER_CPP_MESSAGE_LAYOUT_HELPER_H__
#define GOOGLE_PROTOBUF_COMPILER_CPP_MESSAGE_LAYOUT_HELPER_H__

#include <array>
#include <cstdint>
#include <vector>

#include "absl/types/span.h"
#include "google/protobuf/compiler/cpp/options.h"
#include "google/protobuf/descriptor.h"

namespace google {
namespace protobuf {
namespace compiler {
namespace cpp {

class MessageSCCAnalyzer;

enum FieldPartition {
  kRepeated,  // Non-split repeated fields.
  kHot,
  kWarm,
  kCold,
  kSplit,
  kMax,
};

using FieldPartitionArray =
    std::array<std::vector<const FieldDescriptor*>, FieldPartition::kMax>;

class FieldGroup {
 public:
  FieldGroup() : preferred_location_(0), num_accesses_(0) {}

  // A group with a single field.
  FieldGroup(float preferred_location, const FieldDescriptor* field)
      : preferred_location_(preferred_location),
        fields_(1, field) {
  }

  const std::vector<const FieldDescriptor*>& fields() const { return fields_; }

  void SetPreferredLocation(double location) { preferred_location_ = location; }

  // Appends the fields in 'other' to this group. Access count is incremented.
  void Append(const FieldGroup& other);

  // Sorts by their preferred location.
  bool operator<(const FieldGroup& other) const;

 private:
  bool UpdatePreferredLocationAndInsertOtherFields(const FieldGroup& other);

  float preferred_location_;
  std::vector<const FieldDescriptor*> fields_;
};

// Provides an abstract interface to optimize message layout
// by rearranging the fields of a message.
class MessageLayoutHelper {
 public:
  virtual ~MessageLayoutHelper() {}

  virtual void OptimizeLayout(std::vector<const FieldDescriptor*>* fields,
                              const Options& options,
                              MessageSCCAnalyzer* scc_analyzer) = 0;

 protected:
  enum FieldFamily {
    REPEATED = 0,  // Non-split repeated fields.
    MESSAGE = 1,
    ZERO_INITIALIZABLE = 2,
    OTHER = 3,
    kMaxFamily
  };

  FieldPartitionArray PartitionFields(
      const std::vector<const FieldDescriptor*>& fields, const Options& options,
      MessageSCCAnalyzer* scc_analyzer) const;

  virtual FieldPartition FieldHotness(
      const FieldDescriptor* field, const Options& options,
      MessageSCCAnalyzer* scc_analyzer) const = 0;

  // Reorders "fields" by descending hotness grouped by field family.
  void OptimizeLayoutByFamily(std::vector<const FieldDescriptor*>* fields,
                              const Options& options,
                              absl::Span<const FieldFamily> families,
                              MessageSCCAnalyzer* scc_analyzer);

  virtual FieldGroup SingleFieldGroup(const FieldDescriptor* field) const = 0;
};

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google

#endif  // GOOGLE_PROTOBUF_COMPILER_CPP_MESSAGE_LAYOUT_HELPER_H__
