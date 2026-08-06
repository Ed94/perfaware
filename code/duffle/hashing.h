#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#	include "memory.h"
#endif

#pragma region Hashing
FI_ void hash64_fnv1a(U8_R hash, Slice data, U8 seed) {
	LP_ U8 const default_seed = 0xcbf29ce484222325; 
	if (seed == 0) seed = default_seed;
	hash[0] = seed; for (U8 elem = data.ptr; elem != slice_end(data); elem += 1) {
		hash[0] ^= u1_r(elem)[0];
		hash[0] *= 0x100000001b3;
	}
}
FI_ U8 hash64_fnv1a_ret(Slice data, U8 seed) { U8 h = 0; hash64_fnv1a(& h, data, seed); return h; }
#pragma endregion Hashing
