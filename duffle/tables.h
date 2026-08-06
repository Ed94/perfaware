#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#	include "memory.h"
#	include "hashing.h"
#endif

#pragma region Key Table Linear (KTL)

enum { KT_SLot_value = S_(U8), };
#define KTL_Slot_(type) Struct_(tmpl(KTL_Slot,type)) { \
	U8   key;   \
	type value; \
}
#define KTL_(type) Slice_(tmpl(KTL_Slot,type)); \
	typedef tmpl(Slice_KTL_Slot,type) tmpl(KTL,type)
typedef Slice KTL_Byte;
typedef Struct_(KTL_Meta) {
	U8   slot_size;
	U8   type_width;
};

typedef Array_(Str8, 2);
typedef Slice_(A2_Str8);
typedef KTL_Slot_(Str8);
typedef KTL_(Str8);
FI_ void ktl_populate_slice_a2_str8(KTL_Str8* kt, Slice_A2_Str8 values) {
	assert(kt != null); slice_assert(* kt);
	if (values.len == 0) return;
	assert(kt->len == values.len);
	for index_iter(U4, id, 0, <, values.len) { 
		hash64_fnv1a(& kt->ptr[id].key, slice_to_ut(values.ptr[id][0]), 0);
		mem_copy(u8_(& kt->ptr[id].value), u8_(& values.ptr[id][1]), S_(Str8));
	}
}
#define ktl_str8_key(str)      hash64_fnv1a_ret(slice_to_ut(str8(str)), 0)
#define ktl_str8_from_arr(arr) (KTL_Str8){arr, array_len(arr)}

#pragma endregion KTL
