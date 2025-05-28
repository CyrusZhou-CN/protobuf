# Copyright (c) 2025, Google LLC
# All rights reserved.
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

package(default_applicable_licenses = ["//:license"])

cc_library(
    name = "field_types",
    testonly = True,
    hdrs = ["field_types.h"],
    visibility = ["//upb:__subpackages__"],
    deps = [
        ":wire_message",
        "//upb:base",
        "//upb/wire/decode_fast:combinations",
        "@abseil-cpp//absl/base",
        "@abseil-cpp//absl/strings:string_view",
        "@googletest//:gtest",
    ],
)

cc_library(
    name = "wire_message",
    testonly = True,
    srcs = ["wire_message.cc"],
    hdrs = ["wire_message.h"],
    visibility = [
        "//upb:__pkg__",
        "//upb:__subpackages__",
    ],
    deps = [
        "//upb:wire_reader",
        "//upb/base:internal",
        "@abseil-cpp//absl/log:absl_check",
    ],
)
