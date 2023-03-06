#if __clang__
#	pragma clang diagnostic ignored "-Wunused-const-variable"
#	pragma clang diagnostic ignored "-Wswitch"
#	pragma clang diagnostic ignored "-Wunused-variable"
#endif


#pragma region 							ZPL INCLUDE

#if __clang__
#	pragma clang diagnostic push 
#	pragma clang diagnostic ignored "-Wmissing-braces"
#	pragma clang diagnostic ignored "-Wbraced-scalar-init"
#endif

#define ZPL_IMPLEMENTATION
// #   define ZPL_HEAP_ANALYSIS
#	define ZPL_NO_MATH_H
#	define ZPL_CUSTOM_MODULES
#		define ZPL_MODULE_ESSENTIALS
#		define ZPL_MODULE_CORE
#		define ZPL_MODULE_TIMER
// #	define ZPL_MODULE_HASHING
// #	define ZPL_MODULE_REGEX
// #	define ZPL_MODULE_EVENT
// #	define ZPL_MODULE_DLL
// #	define ZPL_MODULE_OPTS
// #	define ZPL_MODULE_PROCESS
// #	define ZPL_MODULE_MATH
// #	define ZPL_MODULE_THREADING
// #	define ZPL_MODULE_JOBS
// #	define ZPL_MODULE_PARSER
// extern "C" {
#include "zpl.h"
// }

#if __clang__
#	pragma clang diagnostic pop
#endif


#pragma endregion 						ZPL INCLUDE



zpl_arena Global_Arena {};
#define g_allocator zpl_arena_allocator( & Global_Arena)

void setup()
{
	zpl_arena_init_from_allocator( & Global_Arena, zpl_heap(), zpl_megabytes(10) );

	if ( Global_Arena.total_size == 0 )
	{
		zpl_assert_crash( "Failed to reserve memory for Tests:: Global_Arena" );
	}
}

void cleanup()
{
	zpl_arena_free( & Global_Arena);
}



#pragma region ZPL Expose
zpl_string string_make(zpl_isize capacity) 
{
	zpl_isize header_size = zpl_size_of(zpl_string_header);
	void *ptr = zpl_alloc(g_allocator, header_size + capacity + 1);

	zpl_string str;
	zpl_string_header *header;

	if (ptr == NULL) return NULL;
	zpl_zero_size(ptr, header_size + capacity + 1);

	str = cast(char *) ptr + header_size;
	header = ZPL_STRING_HEADER(str);
	header->allocator = g_allocator;
	header->length = 0;
	header->capacity = capacity;
	str[capacity] = '\0';

	return str;
}

inline
zpl_string string_make(const char *str) 
{
	zpl_isize len = str ? zpl_strlen(str) : 0;
	return zpl_string_make_length(g_allocator, str, len);
}

inline 
zpl_string string_append(zpl_string str, zpl_string const other) 
{
	return zpl_string_append_length(str, other, zpl_string_length(other));
}

zpl_string string_format( char const* fmt, ... )
{
	zpl_local_persist zpl_thread_local char buf[ZPL_PRINTF_MAXLEN] = { 0 };
	va_list va;
	va_start(va, fmt);
	zpl_snprintf_va(buf, ZPL_PRINTF_MAXLEN, fmt, va);
	va_end(va);

	return zpl_string_make(g_allocator, buf);
}

zpl_string string_format( char* buffer, zpl_isize size, char const* fmt, ... )
{
	va_list va;
	va_start(va, fmt);
	zpl_snprintf_va(buffer, size, fmt, va);
	va_end(va);

	return zpl_string_make(g_allocator, buffer);
}
#pragma endregion ZPL Expose



#define bit( Value_ )                       ( 1 << Value_ )
#define bitfield_is_equal( Field_, Mask_ ) ( ( Mask_ & Field_ ) == Mask_ )
#undef  cast
#define cast( Type_ , Value_ )             ( ( Type_ )( Value_ ) )
#define ct                                 constexpr
#define gen( ... )                         template< __VA_ARGS__ >
#define forceinline                        ZPL_ALWAYS_INLINE
#define print_nl( _)                       zpl_printf("\n")
#define Msg_Invalid_Value                  "INVALID VALUE PROVIDED"
#define scast                              static_cast
#define rcast                              reinterpret_cast
#define pcast( Type_, Value_ )             ( * (Type_*)( & Value_ ) )



using s8  = zpl_i8;
using u8  = zpl_u8;
using u16 = zpl_u16;
using u32 = zpl_u32;
using f64 = zpl_f64;
using uw  = zpl_usize;
using sw  = zpl_isize;

struct u16_Split
{
	u8 Low;
	u8 High;

	operator u16()
	{
		return * cast( u16*, this );
	}
};



ct inline
char char_binary( u8 value, u8 pos )
{
	u8 mask = 1 << pos ;

	return ( (1 << pos) & value) == mask ? '1' : '0';
}

inline
void str_binary( char* result, u8 value )
{
#if 0
	result[0] = char_binary( value, 0);
	result[1] = char_binary( value, 1);
	result[2] = char_binary( value, 2);
	result[3] = char_binary( value, 3);
	result[4] = char_binary( value, 4);
	result[5] = char_binary( value, 5);
	result[6] = char_binary( value, 6);
	result[7] = char_binary( value, 7);
#else
	result[0] = char_binary( value, 7);
	result[1] = char_binary( value, 6);
	result[2] = char_binary( value, 5);
	result[3] = char_binary( value, 4);
	result[4] = char_binary( value, 3);
	result[5] = char_binary( value, 2);
	result[6] = char_binary( value, 1);
	result[7] = char_binary( value, 0);
#endif
}

inline
void print_as_binary( u8* data, uw size, char const* byte_term )
{
	char
	binary_string[9];
	binary_string[8] = '\0';

	while ( size-- )
	{
		str_binary( binary_string, data[0]);

		printf( "%s%s", binary_string, byte_term );

		data += 1;
	}
}
