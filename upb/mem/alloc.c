// Protocol Buffers - Google's data interchange format
// Copyright 2023 Google LLC.  All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "upb/mem/alloc.h"

#include <stdlib.h>

// Must be last.
#include "upb/port/def.inc"

static void* upb_global_allocfunc(upb_alloc* alloc, void* ptr, size_t oldsize,
                                  size_t size, size_t* actual_size) {
  UPB_UNUSED(alloc);
  UPB_UNUSED(oldsize);
  if (size == 0) {
    free(ptr);
    if (actual_size != NULL) {
      *actual_size = 0;
    }
    return NULL;
  }
  void* newptr = realloc(ptr, size);
  if (actual_size != NULL) {
    *actual_size = newptr != NULL ? size : 0;
  }
  return newptr;
}

upb_alloc upb_alloc_global = {&upb_global_allocfunc};
