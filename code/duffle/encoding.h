#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#endif

typedef struct UnicodeDecode UnicodeDecode;
struct UnicodeDecode {
	U4 inc;
	U4 codepoint;
};

RO_ global U1 utf8_class[32] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5, };

FI_ void u64_to_hex(U8 val, char* buf, S4 chars) {
		static const char hex_chars[] = "0123456789ABCDEF";
		for(S1 i = chars - 1; i >= 0; --i) { buf[i] = hex_chars[val & 0xF]; val >>= 4; }
}

internal UnicodeDecode
utf8_decode(U1* str, U1 max) {
	UnicodeDecode result = {1, Max_U4};
	U1 byte = str[0];
	U1 byte_class = utf8_class[byte >> 3];
	switch(byte_class) {
	case 1: { result.codepoint = byte; } break;
	case 2: if (1 < max) {
		U1 cont_byte = str[1];
		if(utf8_class[cont_byte >> 3] == 0) {
			result.codepoint  = (byte      & Bitmask_5) << 6;
			result.codepoint |= (cont_byte & Bitmask_6);
			result.inc = 2;
		}
	} break;
	case 3: if(2 < max) {
		U1 cont_byte[2] = {str[1], str[2]};
		if ( utf8_class[cont_byte[0] >> 3] == 0
			&& utf8_class[cont_byte[1] >> 3] == 0) {
			result.codepoint  = (byte & Bitmask_4) << 12;
			result.codepoint |= ((cont_byte[0] & Bitmask_6) << 6);
			result.codepoint |=  (cont_byte[1] & Bitmask_6);
			result.inc = 3;
		}
	}break;
	case 4: if(3 < max) {
		U1 cont_byte[3] = {str[1], str[2], str[3]};
		if ( utf8_class[cont_byte[0] >> 3] == 0
			&& utf8_class[cont_byte[1] >> 3] == 0
			&& utf8_class[cont_byte[2] >> 3] == 0) {
			result.codepoint  = (byte & Bitmask_3) << 18;
			result.codepoint |= ((cont_byte[0] & Bitmask_6) << 12);
			result.codepoint |= ((cont_byte[1] & Bitmask_6) <<  6);
			result.codepoint |=  (cont_byte[2] & Bitmask_6);
			result.inc = 4;
		}
	}
	}
	return result;
}

I_ U4
utf16_encode(U2* str, U4 codepoint) {
  U4 inc = 1;
  if      (codepoint == Max_U4) { str[0] = (U2)'?'; }
  else if (codepoint < 0x10000) { str[0] = (U2)codepoint; }
  else {
    U4 v = codepoint - 0x10000;
    str[0] = Csafe_u2u4(0xD800 + (v >> 10));
    str[1] = Csafe_u2u4(0xDC00 + (v & Bitmask_10));
    inc = 2;
  }
  return inc;
}
