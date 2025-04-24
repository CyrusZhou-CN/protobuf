// Protocol Buffers - Google's data interchange format
// Copyright 2023 Google LLC.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef UPB_UPB_GENERATOR_PLUGIN_H_
#define UPB_UPB_GENERATOR_PLUGIN_H_

#include <stdio.h>

#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "absl/container/flat_hash_set.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/compiler/code_generator_lite.h"
#include "google/protobuf/descriptor.h"
#include "upb/mem/arena.h"
#include "upb/mem/arena.hpp"
#include "upb/reflection/descriptor_bootstrap.h"
#include "upb_generator/file_layout.h"

// Must be last.
#include "upb/port/def.inc"

namespace google::protobuf::compiler {
class CodeGeneratorRequest;
}  // namespace protobuf
}  // namespace google::compiler

namespace upb {
namespace generator {

inline std::vector<std::pair<std::string, std::string>> ParseGeneratorParameter(
    const absl::string_view text) {
  std::vector<std::pair<std::string, std::string>> ret;
  google::protobuf::compiler::ParseGeneratorParameter(text, &ret);
  return ret;
}

// Recursively populates the DefPoolPair with the given FileDescriptor.
void PopulateDefPool(const google::protobuf::FileDescriptor* file, upb::Arena* arena,
                     DefPoolPair* pools,
                     absl::flat_hash_set<std::string>* files_seen);

// Returns a new DefPoolPair with a google::protobuf::compiler::CodeGeneratorRequest.
// NewDefPool only uses the proto_file field of the request. The message
// CodeGeneratorResponse is used as the input because we can rely on the
// topological order of the files in the request as described in
// http://google3/third_party/protobuf/compiler/plugin.proto;l=54-56;rcl=586756816.
std::unique_ptr<DefPoolPair> NewDefPool(
    const google::protobuf::compiler::CodeGeneratorRequest& request, upb::Arena* arena);

}  // namespace generator
}  // namespace upb

#include "upb/port/undef.inc"

#endif  // UPB_UPB_GENERATOR_PLUGIN_H_
