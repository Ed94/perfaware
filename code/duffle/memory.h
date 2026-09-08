#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#endif

#define MEM_ALIGNMENT_DEFAULT  4

#define assert_bounds(point, start, end) do{ \
	assert((start) <= (point)); \
	assert((point) <= (end));   \
} while(0)

I_ U8 align_pow2(U8 x, U8 b) {
    assert(b != 0);
    assert((b & (b - 1)) == 0);  // Check power of 2
    return ((x + b - 1) & (~(b - 1)));
}

#define align_struct(type_width) ((U8)(((type_width) + 3) & ~3))

FI_ void mem_bump_u4(U4 start, U4 cap, U4_R used, U4 amount) {
	assert(amount <= (cap - used[0])); 
	used[0] += amount; 
}
FI_ void mem_bump_u8(U8 start, U8 cap, U8_R used, U8 amount) {
	assert(amount <= (cap - used[0])); 
	used[0] += amount; 
}
#define mem_bump(start, cap, used, amount) _Generic(start, U4: mem_bump_u4, U8: mem_bump_u8)(start, cap, used, amount)

FI_ U8 mem_copy            (U8 dest, U8 src,   U8 len) { return (U8)(__builtin_memcpy ((void*)dest, (void const*)src,   len)); }
FI_ U8 mem_copy_overlapping(U8 dest, U8 src,   U8 len) { return (U8)(__builtin_memmove((void*)dest, (void const*)src,   len)); }
FI_ U8 mem_fill            (U8 dest, U8 value, U8 len) { return (U8)(__builtin_memset ((void*)dest, (int)        value, len)); }
FI_ B4 mem_zero            (U8 dest,           U8 len) { if(dest == 0){return false;} mem_fill(dest, 0, len); return true; }

FI_ U8 mem_compare(U8 a, U8 b, U8 len) { return (U8)(__builtin_memcmp((void const*)a, (void const*)b, len)); }
FI_ B4 mem_match  (U8 a, U8 b, U8 z)   { return mem_compare(a, b, z) == 0; }

#define mem_match_struct(a,b)  mem_match(C_(U8,a), C_(U8,b), S_((a)[0]))
#define mem_zero_struct(s) mem_zero(u8_(& s), S_(s))

#pragma region DAG

#define check_nil(nil, p) ((p) == 0 || (p) == nil)
#define set_nil(nil, p)   ((p) = nil)

#define sll_stack_push_n(f, n, next) do { (n)->next = (f); (f) = (n); } while(0)

#define sll_queue_push_nz(nil, f, l, n, next) \
(                           \
	check_nil(nil, f) ? (     \
		(f) = (l) = (n),        \
		set_nil(nil, (n)->next) \
	)                         \
	: (                       \
		(l)->next=(n),          \
		(l) = (n),              \
		set_nil(nil,(n)->next)  \
	)                         \
)
#define sll_queue_push_n(f, l, n, next) sll_queue_push_nz(0, f, l, n, next)

#pragma endregion DAG

#pragma region Slice

typedef unsigned char TSet_(UTF8);
typedef Struct_(Str8)         { UTF8* ptr; U8 len; }; typedef Str8 Slice_UTF8;
typedef Struct_(Slice_Str8)   { Str8* ptr; U8 len; };
#define slit8(string_literal) ((Str8){ (UTF8*) string_literal, S_(string_literal) - 1 })
#define str8(p,l) (Str8){p,l}

typedef Struct_(Slice) { U8 ptr; U8 len; }; // Untyped Slice
FI_ Slice slice_ut_(U8 ptr, U8 len) { return (Slice){ptr, len}; }

#define Slice_(type)       Struct_(tmpl(Slice,type)) { type* ptr; U8 len; }
#define slice_assert(s)    do { assert((s).ptr != 0); assert((s).len > 0); } while(0)
#define slice_end(slice)   ((slice).ptr + (slice).len)
#define S_slice(s)         ((s).len * S_((s).ptr[0]))

#define slice_ut(ptr,len)  slice_ut_(u8_(ptr),     u8_(len))
#define slice_ut_arr(a)    slice_ut_(u8_(a),       S_(a))
#define slice_to_ut(s)     slice_ut_(u8_((s).ptr), S_slice(s))
#define slice_iter(container, iter)     (T_((container).ptr) iter = (container).ptr; iter != slice_end(container); ++ iter)
#define slice_arg_from_array(type, ...) & (tmpl(Slice,type)) { .ptr = Array_decl(type,__VA_ARGS__), .len = Array_len( Array_decl(type,__VA_ARGS__)) }
#define slice_from_array(type, array)     (tmpl(Slice,type)) { .ptr = array, .len = S_(array) }

FI_ void slice_zero_(Slice s) { slice_assert(s); mem_zero(s.ptr, s.len); }
#define  slice_zero(s)        slice_zero_(slice_to_ut(s))

FI_ void slice_copy_(Slice dest, Slice src) {
	assert(dest.len >= src.len);
	slice_assert(dest);
	slice_assert(src);
	mem_copy(dest.ptr, src.ptr, src.len);
}
#define slice_copy(dest, src) do {  \
	static_assert(T_same(dest, src), "slices are not the same type"); \
	slice_copy_(slice_to_ut(dest), slice_to_ut(src)); \
} while(0)

typedef Slice_(B1);
typedef Slice_(U1);
typedef Slice_(U2);
typedef Slice_(U4);
typedef Slice_(U8); 

#pragma endregion Slice

I_ Slice mem_push_aligned_typed(U8 start, U8 capacity, U8_R used, U8 amount, U4 alignment, U4 type_width) {
	if (amount == 0) { return (Slice){}; }
	U8 desired   = amount * (type_width == 0 ? 1 : type_width);
	U8 to_commit = align_pow2(desired, alignment ?  alignment : MEM_ALIGNMENT_DEFAULT);
	U8 ptr       = start + used[0];
	mem_bump_u8(start, capacity, used, to_commit);
	return (Slice){ ptr, to_commit };
}

#pragma region FArena

typedef Opt_(farena)    { U8 alignment, type_width; };
typedef Struct_(FArena) { U8 start, capacity, used; };
FI_ void farena_init(FArena_R arena, Slice mem) {  assert(arena != nullptr);
	arena->start    = mem.ptr;
	arena->capacity = mem.len;
	arena->used     = 0;
}
FI_ FArena farena_make(Slice mem) { FArena a; farena_init(& a, mem); return a; }
FI_ Slice  farena_push(FArena_R arena, U8 amount, Opt_farena o) {
	return mem_push_aligned_typed(arena->start, arena->capacity, & arena->used, amount, o.alignment, o.type_width);
}
FI_ void farena_reset (FArena_R arena) { arena->used = 0; }
FI_ void farena_rewind(FArena_R arena, U8 save_point) {
	assert(save_point <= arena->used);
	arena->used = save_point;
}
FI_ U8 farena_save(FArena arena) { return arena.used; }
#define farena_push_(arena, amount, ...)                                         farena_push((arena),(amount),opt_(farena,__VA_ARGS__))
#define farena_push_type(arena, type, ...)                              C_(type*,farena_push((arena),1,       opt_(farena,.type_width=S_(type),__VA_ARGS__)).ptr)
#define farena_push_array(arena, type, amount, ...) (tmpl(Slice,type)){ C_(type*,farena_push((arena),(amount),opt_(farena,.type_width=S_(type),__VA_ARGS__)).ptr),(amount) }

#pragma endregion FArena

#pragma region FStack
#define FStack_(name, type, width) Struct_(name) { U8 top; type arr[width]; }

FI_ Slice fstack_push(Slice mem, U8_R top, U8 amount, Opt_farena o) { 
	return mem_push_aligned_typed(mem.ptr, mem.len, top, amount, o.alignment, o.type_width);
};

// This is here more for annotation than anything else.
#define fstack_save(stack)       stack.top
#define fstack_rewind(stack, sp) do{stack.top = sp;}while(0)
#define fstack_reset(stack)      do{stack.top = 0; }while(0)

#define fstack_slice(stack) slice_ut_arr((stack).arr)
#define fstack_push_(stk, amount, ...)                                         fstack_push(fstack_slice(stk),&(stk).top,(amount),opt_(farena,__VA_ARGS__))
#define fstack_push_array(stk, type, amount, ...) (tmpl(Slice,type)){ C_(type*,fstack_push(fstack_slice(stk),&(stk).top,(amount),opt_(farena,.type_width=S_(type),__VA_ARGS__)).ptr),(amount) }
#pragma endregion FStack
