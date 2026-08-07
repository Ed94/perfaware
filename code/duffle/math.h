#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#	include "analysis.h"
#endif

#pragma region Math
#define u8_max 0xffffffffffffffffull

#define min(A,B)       (((A) < (B)) ? (A) : (B))
#define max(A,B)       (((A) > (B)) ? (A) : (B))
#define clamp_bot(X,B) max(X, B) // Clamp "X" by "B"

#define clamp_decrement(X) (((X) > 0) ? ((X) - 1) : 0)

typedef Struct_(R1_U1){ U1 p0; U1 p1; };
typedef Struct_(R1_U2){ U2 p0; U2 p1; };
typedef Struct_(R1_U4){ U4 p0; U2 p1; };
typedef Struct_(R1_U8){ U8 p0; U8 p1; };

#define r1u4(p0,p1) (R1_U4){p0,p1}
#define r1u8(p0,p1) (R1_U8){p0,p1}

typedef Struct_(V2_U1){ U1 x; U1 y;};

FI_ B8 add_of  (U8 a, U8 b, U8*R_ res) { return __builtin_uaddll_overflow(a, b, res); }
FI_ B8 sub_of  (U8 a, U8 b, U8*R_ res) { return __builtin_usubll_overflow(a, b, res); }
FI_ B8 mul_of  (U8 a, U8 b, U8*R_ res) { return __builtin_umulll_overflow(a, b, res); }
FI_ B8 add_s_of(S8 a, S8 b, S8*R_ res) { return __builtin_saddll_overflow(a, b, res); }
FI_ B8 sub_s_of(S8 a, S8 b, S8*R_ res) { return __builtin_ssubll_overflow(a, b, res); }
FI_ B8 mul_s_of(S8 a, S8 b, S8*R_ res) { return __builtin_smulll_overflow(a, b, res); }

FI_ U8 span_r1u8(R1_U8 r) {U8 c = ((r.p1 > r.p0) ? (r.p1 - r.p0) : 0); return c;}
#pragma endregion Math

#pragma region Numerics

enum {
	Max_U4 = 0xffffffff
};

FI_ U2 Csafe_u2u4(U4 x) { assert_always(x <= Max_U4); return C_(U2, x); }
#pragma endregion Numerics
