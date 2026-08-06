#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#endif

#pragma region Encoding
FI_ void u64_to_hex(U8 val, char* buf, S4 chars) {
		static const char hex_chars[] = "0123456789ABCDEF";
		for(S1 i = chars - 1; i >= 0; --i) { buf[i] = hex_chars[val & 0xF]; val >>= 4; }
}
#pragma endregion Encoding
