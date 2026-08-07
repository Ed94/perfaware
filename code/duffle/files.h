#ifdef INTELLISENSE_DIRECTIVES
#	pragma once
#	include "dsl.h"
#	include "memory.h"
#	include "math.h"
#	include "text.h"
#
#	if DUFFLE_WINDOWS
#		include "win32.h"
#	endif
#endif

CLANG_OPTIMIZE_DISABLE

// Most of this referenced from the RAD Debugger codebase.
typedef Enum_(U4,AccessFlags) {
  Bit_(AccessFlag_Read,       0),
  Bit_(AccessFlag_Write,      1),
  Bit_(AccessFlag_Execute,    2),
  Bit_(AccessFlag_Append,     3),
  Bit_(AccessFlag_ShareRead,  4),
  Bit_(AccessFlag_ShareWrite, 5),
  Bit_(AccessFlag_Inherited,  6),
};

typedef U4 FilePropertyFlags;
enum {
  Bit_(FilePropertyFlag_IsFolder, 0),
};

typedef Struct_(FileProperties) {
  U8                size;
  DenseTime         modified;
  DenseTime         created;
  FilePropertyFlags flags;
};

typedef Struct_(File) { U8 ptr[1]; };

FI_ File file_zero() { File f = {0}; return f; }

typedef Struct_(Scope_FileInfo) { File f; B4 once; };

FI_ B4 file_match(File a, File b) { return mem_match_struct(& a, & b); }

os_layer void file_close(File file);
os_layer File file_open(FArena* scratch, AccessFlags flags, Str8 path);
#define file_scope(scratch, flags, path) scope_info(Scope_FileInfo, {.f = file_open(scratch, flags, path)}, file_close(info.f))

os_layer FileProperties properties_from_file(File file);
os_layer U8             file_read(File file, R1_U8 rng, U1* out_data);

internal Slice_U1
data_from_file_range(FArena* arena, File file, R1_U8 range) {
  U8       pre_pos = farena_save(arena[0]);
  U8       len     = span_r1u8(range);
  Slice_U1 result  = farena_push_array(arena, U1, len);
  U8 actual_read_size = file_read(file, range, result.ptr); if (actual_read_size < result.len) {
		farena_rewind(arena, pre_pos + actual_read_size); result.len = actual_read_size;
  }
  return result;
}

I_ Slice_U1
data_from_file_path(FArena* arena, Str8 path, FArena* scratch) { file_scope(scratch, AccessFlag_Read | AccessFlag_ShareRead, path) {
	FileProperties props = properties_from_file(info.f);
	Slice_U1       data  = data_from_file_range(arena, info.f, r1u8(0, props.size)); return data;
} unreachable(); }

CLANG_OPTIMIZE_ENABLE
