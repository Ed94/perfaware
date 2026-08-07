/*
C DSL Duffle
ISA:      amd64
Sandbox:  Windows 11
Compiler: clang
Standard: c23
*/
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wuninitialized"
#pragma clang diagnostic ignored "-Wmicrosoft-enum-forward-reference"
// #pragma comment(lib, "Advapi32.lib")
// #pragma comment(lib, "gdi32.lib")
// #pragma comment(lib, "Kernel32.lib")
// #pragma comment(lib, "msvcrt.lib")
// #pragma comment(lib, "user32.lib")
// #pragma comment(lib, "ucrt.lib")
// #pragma comment(lib, "vcruntime.lib")

#pragma region Platform
#define CLANG_OPTIMIZE_DISABLE _Pragma("clang optimize off")
#define CLANG_OPTIMIZE_ENABLE  _Pragma("clang optimize on")

#define DUFFLE_x86_64  1
#define DUFFLE_WINDOWS 1

#define WinAPI __attribute((__stdcall__)) __attribute__((__force_align_arg_pointer__)) // Win32 Syscall FFI

#define os_layer // Marker for interfaced resolved by os platform layer.
#pragma endregion Platform

#define offset_of(type, member)  cast(U8,__builtin_offsetof(type,member))
#define static_assert            _Static_assert
#define typeof                   __typeof__
#define typeof_ptr(ptr)          typeof((ptr)[0])
#define typeof_same(a, b)        _Generic((a), typeof((b)): 1, default: 0)

#define m_expand(...)      __VA_ARGS__
#define glue_impl(A, B)    A ## B
#define glue(A, B)         glue_impl(A, B)
#define tmpl(prefix, type) prefix ## _ ## type

#define stringify_impl(S)        #S
#define stringify(S)             stringify_impl(S)

#define VA_Sel_1( _1, ... ) _1 // <-- Of all th args passed pick _1.
#define VA_Sel_2( _1, _2, ... ) _2 // <-- Of all the args passed pick _2.
#define VA_Sel_3( _1, _2, _3, ... ) _3 // etc..

#define global static // Mark global data
#define gknown        // Mark global data used in procedure

#define LP_      static // static data within procedure scope
#define internal static // internal

#define asm           __asm__

#define align_(value) __attribute__((aligned (value)))             // for easy alignment
#define C_(type,data) ((type)(data))                               // for enforced precedence
#define expect_(x, y) __builtin_expect(x, y)                       // so compiler knows the common path
#define cexpr_        __builtin_constant_p
#define I_            internal inline
#define FI_           inline   __attribute__((always_inline))      // inline always
#define NI_           internal __attribute__((noinline))           // inline never
#define RO_           __attribute__((section(".rodata")))          // Read only data allocation
#define T_            typeof                                       // 
#define T_same(a,b)   _Generic((a), typeof((b)): 1, default: 0)

#define R_    restrict 
#define V_    volatile 

// R_ (restrict) establishes an "Eigen" or "Proprius" mapping.
// Unlike volatile (V_), which assumes the memory can be changed by anything,
// R_ tells the compiler that this pointer holds the *sole*, private (idios) ownership of the memory slice. 
// Writes to this memory are exclusively bound to this single symbolic mapping for the duration of the scope, guaranteeing  zero aliasing.

#pragma region Fictional //, used for intiution

#define EUB_  restrict // Execute Unit Bound:    Data is siloed in the ALU Register File. The Load/Store Unit is bypassed. (Route to Execution Unit.  Keep in registers)
#define ISO_  restrict // Isolated Provenance:   Alternative to Exu_. Guarantees electrical memory isolation, 
											 //                        unlocking the compiler’s ability to safely pack data across multiple parallel SIMD lanes (vectorization).
#define LSU_  volatile // Load/Store Unit Bound: The compiler is forbidden from caching in registers. Forces physical L1 Cache matrix sampling.
#define LIVE_ volatile // Live External Data:    Alternative to Lsu_ emphasizing the memory is tapped by an external electrical actor.

#define latch_store  /* ~: atomic_store*/         // Blasts voltages from the Store Buffer into the L1 SRAM, physically flipping the cross-coupled inverters to lock the state.
#define pulse_rfo    /* ~: atomic_xchg*/          // Broadcasts an electrical RFO (Request For Ownership) pulse across the CPU mesh network to invalidate other L1 caches.
#define tact_acquire /* ~: memory_order_acquire*/ // Clamp. Sends a voltage signal to the instruction decoder to halt the Out-of-Order engine until the load resolves.
#define tact_release /* ~: memory_order_release*/ // Drain. Forces the Store Buffer flip-flops to completely empty into the L1 cache before proceeding.

// -----------------------------------------------------------------------------
// Out-of-Order (OoO) Pipeline Modifiers
// -----------------------------------------------------------------------------
#define ooo_drift_  __ATOMIC_RELAXED // OoO engine allowed to drift
#define ooo_anchor_ __ATOMIC_ACQUIRE // Anchor the Load Queue (halt spec lookahead)
#define ooo_drain_  __ATOMIC_RELEASE // Drain the Store Buffer (force writeback)
#define ooo_weld_   __ATOMIC_SEQ_CST // Weld pipeline (total order bus lock)

// Latch operations with physical queue modifiers
#define latch_load_anchor(ptr)       //__atomic_load_n(ptr, ooo_anchor_)
#define latch_store_drain(ptr, val)  //__atomic_store_n(ptr, val, ooo_drain_)
#define pulse_xchg_weld(ptr, val)    //__atomic_exchange_n(ptr, val, ooo_weld_)

#pragma endregion Fictional

#define r_(ptr)        C_(T_(ptr[0])*R_, ptr) // Constrain pointer to restrict
#define v_(ptr)        C_(T_(ptr[0])V_*, ptr) // 
#define tr_(type, ptr) C_(type *R_, ptr)
#define tv_(type, ptr) C_(type V_*, ptr)

#define TypeR_(type)   type *R_ type ## _R  // type *restrict type_R
#define TypeV_(type)   type V_* type ## _V  // type volatile* type_V
#define PtrSet_(type)  TypeR_(type); typedef TypeV_(type)
#define TSet_(type)    type; typedef PtrSet_(type)

#define Array_len(a)                   (U4)(sizeof(a) / sizeof(typeof((a)[0])))
#define Array_decl(type, ...)          (type[]){__VA_ARGS__}
#define Array_sym(type,len)            A ## len ## _ ## type
#define Array_expand(type,len)         type Array_sym(type, len)[len]; typedef PtrSet_(Array_sym(type, len))
#define Array_(type,len)               Array_expand(type,len)
#define Bit_(id,b)                     id = (1 << b), tmpl(id,pos) = b
#define Enum_(underlying_type, symbol) underlying_type TSet_(symbol); enum symbol 
#define Proc_(symbol)                  symbol
#define Relative_(symbol)              // Does nothing but annotate that a symbol is associated with another.
#define Struct_(symbol)                struct symbol   TSet_(symbol); struct symbol
#define Union_(symbol)                 union  symbol   TSet_(symbol); union  symbol

#define Opt_(proc)                     Struct_(tmpl(Opt,proc))
#define opt_(symbol, ...)              (tmpl(Opt,symbol)){__VA_ARGS__}
#define Ret_(proc)                     Struct_(tmpl(Ret,proc))
#define ret_(proc)                     tmpl(Ret,proc) proc

// Using Byte-Width convention for the fundamental types.
typedef __UINT8_TYPE__  TSet_(U1); 
typedef __UINT16_TYPE__ TSet_(U2);
typedef __UINT32_TYPE__ TSet_(U4);
typedef __UINT64_TYPE__ TSet_(U8);
typedef __INT8_TYPE__   TSet_(S1); 
typedef __INT16_TYPE__  TSet_(S2); 
typedef __INT32_TYPE__  TSet_(S4);
typedef __INT64_TYPE__  TSet_(S8);
typedef unsigned char   TSet_(B1); 
typedef __UINT16_TYPE__ TSet_(B2); 
typedef __UINT32_TYPE__ TSet_(B4);
typedef __UINT64_TYPE__ TSet_(B8);
typedef float F4_2 __attribute__((vector_size(16)));

#define u1_(value)  C_(U1, value)
#define u2_(value)  C_(U2, value)
#define u4_(value)  C_(U4, value)
#define u8_(value)  C_(U8, value)
#define s1_(value)  C_(S1, value)
#define s2_(value)  C_(S2, value)
#define s4_(value)  C_(S4, value)
#define s8_(value)  C_(S8, value)

#define u1_r(value) C_(U1 *R_, value)
#define u2_r(value) C_(U2 *R_, value)
#define u4_r(value) C_(U4 *R_, value)
#define u8_r(value) C_(U8 *R_, value)
#define u1_v(value) C_(U1 V_*, value)
#define u2_v(value) C_(U2 V_*, value)
#define u4_v(value) C_(U4 V_*, value)
#define u8_v(value) C_(U8 V_*, value)
enum { false = 0, true  = 1, true_overflow, };

#define u4_lo(value) ((value) & 0xFFFFU)
#define u4_hi(value) ((value) >> 12)

typedef void Proc_(VoidFn) (void);

#define kilo(n)                (n << 10)
#define mega(n)                (n << 20)
#define giga(n)                (n << 30)
#define tera(n)                (n << 40)

#define null                    C_(U8,    0)
#define nullptr                 C_(void*, 0)
#define O_(type, field)         C_(U8, & C_(type*,0)->field)
#define OA_(type, member, idx)  C_(U8, & C_(type*,0)->member[idx])
#define OT_(field)              O_(typeof_ptr(& field), filed))
#define S_(data)                C_(U8, sizeof(data))

#define sop_1(op,a,b) C_(U1, s1_(a) op s1_(b))
#define sop_2(op,a,b) C_(U2, s2_(a) op s2_(b))
#define sop_4(op,a,b) C_(U4, s4_(a) op s4_(b))
#define sop_8(op,a,b) C_(U8, s8_(a) op s8_(b))

#undef def_signed_op
#define def_signed_op(id,op,width) FI_ U ## width id ## _s ## width(U ## width a, U ## width b) {return sop_ ## width(op, a, b); }
#define def_signed_ops(id,op)      def_signed_op(id, op, 1) def_signed_op(id, op, 2) def_signed_op(id, op, 4) def_signed_op(id, op, 8)
def_signed_ops(add, +)
def_signed_ops(sub, -)
def_signed_ops(mut, *)
def_signed_ops(div, /)
def_signed_ops(gt,  >)
def_signed_ops(lt,  <) 
def_signed_ops(ge, >=)
def_signed_ops(le, <=)
#undef def_signed_ops
#undef def_signed_op

#define def_generic_sop(op, a, ...) _Generic((a), U1:  op ## _s1, U2: op ## _s2, U4: op ## _s4, U8: op ## _s8) (a, __VA_ARGS__)
#define add_s(a,b) def_generic_sop(add,a,b)
#define sub_s(a,b) def_generic_sop(sub,a,b)
#define mut_s(a,b) def_generic_sop(mut,a,b)
#define gt_s(a,b)  def_generic_sop(gt, a,b)
#define lt_s(a,b)  def_generic_sop(lt, a,b)
#define ge_s(a,b)  def_generic_sop(ge, a,b)
#define le_s(a,b)  def_generic_sop(le, a,b)
#undef def_generic_sop

#define alignas               _Alignas
#define alignof               _Alignof
#define byte_pad(amount, ...) B1 glue(_PAD_, __VA_ARGS__) [amount]
#define pcast(type, data)     (C_(type*, & (data)) [0])

#define dbg_args(...) __VA_ARGS__

#pragma region Control Flow & Iteration
#define unreachable() __builtin_unreachable()

#define each_iter(type, iter, end)             (type iter = 0; iter < end; ++ iter)
#define index_iter(type, iter, begin, op, end) (type iter = begin; iter op end; (begin < end ? ++ iter : -- iter))
#define range_iter(iter,op,range)              (T_((range).p0) iter = (range).p0; iter op (range).p1; ((range).p0 < (range).p1 ? ++ iter : -- iter))

#define defer(expr)                for(U4         once= 1;                 once!=1;++     once,(expr))    // Basic do something after body
#define defer_rewind(cursor)       for(T_(cursor) sp=cursor,once=0;        once!=1;++     once,cursor=sp) // Used with arenas/stacks
#define defer_info(type,expr, ...) for(type       info={__VA_ARGS__}; info.once!=1;++info.once,(expr))    // Defer with tracked state
#define scope(begin,end)           for(U4         once=(1,(begin));        once!=1;++     once,(end ))    // Do things before or after a scope
#define scope_info(type,begin,end) for(type       info=begin;         info.once!=1;++info.once,(end ))

#define do_while(cond) for (U4 once=0; once!=1 || (cond); ++once)

#define span_iter(type, iter, m_begin, op, m_end) ( \
	tmpl(Iter_Span,type) iter = {     \
		.r      = {(m_begin), (m_end)}, \
		.cursor = (m_begin) };          \
	iter.cursor op iter.r.end;        \
	++ iter.cursor                    \
)
#define Span_(type)                                                \
					Struct_(tmpl(     Span,type)) { type begin; type end; }; \
	typedef Struct_(tmpl(Iter_Span,type)) { tmpl(Span,type) r; type cursor; }
#pragma endregion Control Flow & Iteration

typedef Span_(S4);
typedef Span_(U4);
typedef Span_(U8);
#pragma region Thread Coherence

FI_ void barrier_compiler(void){asm volatile("::""memory");} // Compiler Barrier
FI_ void barrier_memory  (void){__builtin_ia32_mfence();}    // Memory   Barrier
FI_ void barrier_read    (void){__builtin_ia32_lfence();}    // Read     Barrier
FI_ void barrier_write   (void){__builtin_ia32_sfence();}    // Write    Barrier

// x86-64
FI_ U4 atm_add_u4 (U4_R addr, U4 value){asm volatile("lock xaddl %0,%1":"=r"(value),"=m"(addr[0]):"0"(value),"m"(addr[0]):"memory","cc");return value;}
FI_ U8 atm_add_u8 (U8_R addr, U8 value){asm volatile("lock xaddq %0,%1":"=r"(value),"=m"(addr[0]):"0"(value),"m"(addr[0]):"memory","cc");return value;}
FI_ U4 atm_swap_u4(U4_R addr, U4 value){asm volatile("lock xchgl %0,%1":"=r"(value),"=m"(addr[0]):"0"(value),"m"(addr[0]):"memory","cc");return value;}
FI_ U8 atm_swap_u8(U8_R addr, U8 value){asm volatile("lock xchgq %0,%1":"=r"(value),"=m"(addr[0]):"0"(value),"m"(addr[0]):"memory","cc");return value;}
#pragma endregion Thread Coherence

#pragma region Misc
enum {
	Bitmask_3  = 0x00000007,
	Bitmask_4  = 0x0000000f,
	Bitmask_5  = 0x0000001f,
	Bitmask_6  = 0x0000003f,
	Bitmask_10 = 0x000003ff,
};

typedef Enum_(U4, WeekDay) {
	WeekDay_Sun,
	WeekDay_Mon,
	WeekDay_Tue,
	WeekDay_Wed,
	WeekDay_Thu,
	WeekDay_Fri,
	WeekDay_Sat,
	WeekDay_Num,
};

typedef Enum_(U4, Month) {
	Month_Jan,
	Month_Feb,
	Month_Mar,
	Month_Apr,
	Month_May,
	Month_Jun,
	Month_Jul,
	Month_Aug,
	Month_Sep,
	Month_Oct,
	Month_Nov,
	Month_Dec,
	Month_Num,
};

typedef U8 DenseTime;

typedef Struct_(DateTime) {
	U4      micro_sec; // [0,999]
	U4      msec;      // [0,999]
	U4      sec;       // [0,60]
	U4      min;       // [0,59]
	U4      hour;      // [0,24]
	U4      day;       // [0,30]
	WeekDay week_day;
	Month   month;
	U4      year; // 1 = 1 CE, 0 = 1 BC
};

I_ DenseTime
dense_time_from_date_time(DateTime date_time) {
  DenseTime result = 0;
  result += date_time.year;  result *= 12;
  result += date_time.month; result *= 31;
  result += date_time.day;   result *= 24;
  result += date_time.hour;  result *= 60;
  result += date_time.min;   result *= 61;
  result += date_time.sec;   result *= 1000;
  result += date_time.msec;  
  return(result);
}
#pragma endregion Misc
