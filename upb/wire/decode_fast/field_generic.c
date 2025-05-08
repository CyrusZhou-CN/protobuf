
#include <stdint.h>

#include "upb/message/message.h"
#include "upb/wire/decode_fast/combinations.h"
#include "upb/wire/decode_fast/dispatch.h"
#include "upb/wire/internal/decoder.h"

const char* _upb_FastDecoder_DecodeGeneric(struct upb_Decoder* d,
                                           const char* ptr, upb_Message* msg,
                                           intptr_t table, uint64_t hasbits,
                                           uint64_t data) {
  (void)data;
  upb_DecodeFast_SetHasbits(msg, hasbits);
  return _upb_Decoder_DecodeMessage(d, ptr, msg, decode_totablep(table));
}

// Not supported yet, so we just fall back to the generic decoder.
#define ALIAS_GENERIC(type, card, tagsize)                                   \
  const char* UPB_DECODEFAST_FUNCNAME(type, card, tagsize)(UPB_PARSE_PARAMS) \
      __attribute__((alias("_upb_FastDecoder_DecodeGeneric")));

UPB_DECODEFAST_CARDINALITIES(UPB_DECODEFAST_TAGSIZES, ALIAS_GENERIC, ClosedEnum)

#undef ALIAS_GENERIC
