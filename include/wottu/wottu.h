#ifndef WOTTU_H
#define WOTTU_H

#include <stdbool.h>
#include <stddef.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <errno.h>
#include <ctype.h>
#include <string.h>

// Typedefs
typedef int wtu_err_t;  // Used for error handling

// Macros
#define WTU_IMPL_UNUSED0()
#define WTU_IMPL_UNUSED1(a)             (void)(a)
#define WTU_IMPL_UNUSED2(a, b)          (void)(a), WTU_IMPL_UNUSED1(b)
#define WTU_IMPL_UNUSED3(a, b, c)       (void)(a), WTU_IMPL_UNUSED2(b, c)
#define WTU_IMPL_UNUSED4(a, b, c, d)    (void)(a), WTU_IMPL_UNUSED3(b, c, d)
#define WTU_IMPL_UNUSED5(a, b, c, d, e) (void)(a), WTU_IMPL_UNUSED4(b, c, d, e)

#define WTU_VA_NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, N, ...) N
#define WTU_VA_NUM_ARGS(...)                                 WTU_VA_NUM_ARGS_IMPL(100, ##__VA_ARGS__, 5, 4, 3, 2, 1, 0)

#define WTU_UNUSED_IMPL_(nargs) WTU_IMPL_UNUSED##nargs
#define WTU_UNUSED_IMPL(nargs)  WTU_UNUSED_IMPL_(nargs)
#define WTU_UNUSED(...)         WTU_UNUSED_IMPL(WTU_VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#define WTU_STR_IMPL_(x) #x
#define WTU_STR(x)       WTU_STR_IMPL_(x)

#define WTU_CAT_IMPL_(x, y) x##y
#define WTU_CAT(x, y)       WTU_CAT_IMPL_(x, y)

#define WTU_VAR(x) WTU_CAT(var_, WTU_CAT(x, WTU_CAT(__LINE__, _)))

#define WTU_LINEINFO "[" __FILE__ ":" WTU_STR(__LINE__) "]"

#define WTU_MAX(a, b) ((a > b) ? a : b)
#define WTU_MIN(a, b) ((a < b) ? a : b)

// ANSI Colours
#define WTU_RESET "\x1b[0m"
#define WTU_BOLD  "\x1b[1m"

#define WTU_FG_BLACK   "\x1b[30m"
#define WTU_FG_RED     "\x1b[31m"
#define WTU_FG_GREEN   "\x1b[32m"
#define WTU_FG_YELLOW  "\x1b[33m"
#define WTU_FG_BLUE    "\x1b[34m"
#define WTU_FG_MAGENTA "\x1b[35m"
#define WTU_FG_CYAN    "\x1b[36m"
#define WTU_FG_WHITE   "\x1b[37m"

#define WTU_FG_BLACK_BRIGHT   "\x1b[90m"
#define WTU_FG_RED_BRIGHT     "\x1b[91m"
#define WTU_FG_GREEN_BRIGHT   "\x1b[92m"
#define WTU_FG_YELLOW_BRIGHT  "\x1b[93m"
#define WTU_FG_BLUE_BRIGHT    "\x1b[94m"
#define WTU_FG_MAGENTA_BRIGHT "\x1b[95m"
#define WTU_FG_CYAN_BRIGHT    "\x1b[96m"
#define WTU_FG_WHITE_BRIGHT   "\x1b[97m"

#define WTU_BG_BLACK   "\x1b[40m"
#define WTU_BG_RED     "\x1b[41m"
#define WTU_BG_GREEN   "\x1b[42m"
#define WTU_BG_YELLOW  "\x1b[43m"
#define WTU_BG_BLUE    "\x1b[44m"
#define WTU_BG_MAGENTA "\x1b[45m"
#define WTU_BG_CYAN    "\x1b[46m"
#define WTU_BG_WHITE   "\x1b[47m"

#define WTU_BG_BLACK_BRIGHT   "\x1b[100m"
#define WTU_BG_RED_BRIGHT     "\x1b[101m"
#define WTU_BG_GREEN_BRIGHT   "\x1b[102m"
#define WTU_BG_YELLOW_BRIGHT  "\x1b[103m"
#define WTU_BG_BLUE_BRIGHT    "\x1b[104m"
#define WTU_BG_MAGENTA_BRIGHT "\x1b[105m"
#define WTU_BG_CYAN_BRIGHT    "\x1b[106m"
#define WTU_BG_WHITE_BRIGHT   "\x1b[107m"

#define LOGLEVELS \
	X(WTU_INFO, "[.]", "info", WTU_FG_CYAN_BRIGHT) \
	X(WTU_WARN, "[*]", "warn", WTU_FG_BLUE) \
	X(WTU_FAIL, "[!]", "fail", WTU_FG_RED) \
	X(WTU_OKAY, "[^]", "okay", WTU_FG_GREEN)

#define X(x, y, z, w) x,

typedef enum {
	LOGLEVELS
} wtu_loglevel_t;

#undef X

#define X(x, y, z, w) [x] = y,
const char* WTU_LOGLEVEL_TO_STR[] = {LOGLEVELS};
#undef X

#define X(x, y, z, w) [x] = z,
const char* WTU_LOGLEVEL_HUMAN_TO_STR[] = {LOGLEVELS};
#undef X

#define X(x, y, z, w) [x] = w,
const char* WTU_LOGLEVEL_COLOUR[] = {LOGLEVELS};
#undef X

#undef LOGLEVELS

typedef struct {
	const char* name;  // Name of logger for filtering by pass or stage
	FILE* dest;        // Destination to log to (usually stderr)
	wtu_loglevel_t level;
	size_t indent;
} wtu_logger_t;

static wtu_logger_t wtu_logger_create(const char* name) {
	return (wtu_logger_t){
		.name = name,
		.dest = NULL,
		.level = WTU_INFO,
		.indent = 0,
	};
}

static void wtu_log_info_v(wtu_logger_t* log,
	wtu_loglevel_t lvl,
	const char* filename,
	const char* line,
	const char* func,
	const char* fmt,
	va_list args) {
	if (lvl < log->level) {
		return;
	}

	if (log->dest == NULL) {
		log->dest = stderr;  // Default location for logging.
	}

	const char* lvl_s = WTU_LOGLEVEL_TO_STR[lvl];
	const char* lvl_hs = WTU_LOGLEVEL_HUMAN_TO_STR[lvl];
	const char* lvl_col = WTU_LOGLEVEL_COLOUR[lvl];

	fprintf(log->dest,
		WTU_BOLD "%s%s" WTU_RESET
				 " "
				 "%s%s" WTU_RESET,
		lvl_col,
		lvl_s,
		lvl_col,
		lvl_hs);

	// TODO: Check if these cases actually work.
	if (filename != NULL && line != NULL) {
		fprintf(log->dest, " [%s:%s]", filename, line);
	}

	else if (filename != NULL && line == NULL) {
		fprintf(log->dest, " [%s]", filename);
	}

	else if (filename == NULL && line != NULL) {
		fprintf(log->dest, " [%s]", line);
	}

	if (func != NULL) {
		fprintf(log->dest, " `%s`", func);
	}

	if (fmt != NULL) {
		fprintf(log->dest, ": ");
		vfprintf(log->dest, fmt, args);
	}

	fputc('\n', log->dest);
}

static void wtu_log_info(wtu_logger_t* log,
	wtu_loglevel_t lvl,
	const char* filename,
	const char* line,
	const char* func,
	const char* fmt,
	...) {
	va_list args;
	va_start(args, fmt);

	wtu_log_info_v(log, lvl, filename, line, func, fmt, args);

	va_end(args);
}

static void wtu_log(wtu_logger_t* log, wtu_loglevel_t lvl, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	wtu_log_info_v(log, lvl, NULL, NULL, NULL, fmt, args);

	va_end(args);
}

#define WTU_INFO(log, ...) wtu_log_info(log, WTU_INFO, __FILE__, WTU_STR(__LINE__), __func__, __VA_ARGS__)
#define WTU_WARN(log, ...) wtu_log_info(log, WTU_WARN, __FILE__, WTU_STR(__LINE__), __func__, __VA_ARGS__)
#define WTU_FAIL(log, ...) wtu_log_info(log, WTU_FAIL, __FILE__, WTU_STR(__LINE__), __func__, __VA_ARGS__)
#define WTU_OKAY(log, ...) wtu_log_info(log, WTU_OKAY, __FILE__, WTU_STR(__LINE__), __func__, __VA_ARGS__)

// Find out where you are with a rainbow.
#define WTU_WHEREAMI(log) \
	wtu_log_info(log, \
		WTU_INFO, \
		__FILE__, \
		WTU_STR(__LINE__), \
		__func__, \
		WTU_FG_RED "Y" WTU_FG_RED_BRIGHT "O" WTU_FG_YELLOW "U" WTU_RESET " " WTU_FG_GREEN "A" WTU_FG_BLUE \
				   "R" WTU_FG_MAGENTA "E" WTU_RESET " " WTU_FG_MAGENTA_BRIGHT "H" WTU_FG_RED "E" WTU_FG_RED_BRIGHT \
				   "R" WTU_FG_YELLOW "E" WTU_RESET)

#define WTU_FUNCTION_ENTER(log) wtu_log_info(log, WTU_INFO, __FILE__, WTU_STR(__LINE__), __func__, NULL)

// TODO: Implement "unimplemented" macro that will unconditionally abort
// TODO: Implement "unreachable" macro
// TODO: Implement wtu_die function that reports using WTU_ERROR and then calls
// abort
// TODO: Implement assert macro
// TODO: Debug macro (might not be reasonable)

// Global logger instance
// static wtu_logger_t WTU_LOGGER = (wtu_logger_t){
// 	.dest = NULL,
// 	.level = WTU_DEBUG,
// 	.indent = 0,
// };

// Return absolute difference between 2 pointers regardless of order.
static size_t wtu_ptrdiff(const void* a, const void* b) {
	return b > a ? b - a : a - b;
}

// Compare strings after first comparing length.
static bool wtu_strncmp(const char* ptr, const char* end, const char* str) {
	size_t length = wtu_ptrdiff(ptr, end);

	// TODO: Write our own strncmp implementation here to avoid
	// iterating string twice due to strlen.
	if (length != strlen(str)) {
		return false;
	}

	return strncmp(ptr, str, length) == 0;
}

// Read bytes from stdin to a growing buffer until EOF.
static wtu_err_t wtu_read_stdin(char** buffer, size_t* length) {
	// TODO: Just increase allocations by fixed amount with IO.
	char* buf = realloc(NULL, 256);

	size_t capacity = 256;
	size_t index = 0;

	int c;
	while ((c = fgetc(stdin)) != EOF) {
		// if (index >= capacity) {
		// 	capacity *= 2;
		// 	*buffer = realloc(*buffer, capacity);

		// 	if (!*buffer) {
		// 		return errno;
		// 	}
		// }

		buf[index++] = c;
	}

	if (ferror(stdin)) {
		return errno;
	}

	*length = index;

	return 0;
}

static wtu_err_t wtu_read_file(const char* path, char** buf_out, size_t* len_out) {
	FILE* file = fopen(path, "r");
	if (!file) {
		return errno;
	}

	// get file size
	if (fseek(file, 0, SEEK_END) == -1) {
		return errno;
	}

	int len = ftell(file);
	if (len == -1) {
		return errno;
	}

	rewind(file);

	char* buf = malloc(len);
	if (!buf) {
		return errno;
	}

	// read file contents into buf
	int nread = fread(buf, 1, len, file);
	if (nread != len || fclose(file) == EOF) {
		wtu_err_t code = errno;
		free(buf);
		return code;
	}

	*buf_out = buf;
	*len_out = len;

	return 0;
}

// wtu_err_t wtu_basename(const char* path, char* out, size_t size) {
// 	memset(out, 0, size);

// 	size_t len = strlen(path);

// 	char* last = strrchr(path, '/');
// 	if (!last) {
// 		if (len > size) {
// 			return ENAMETOOLONG;
// 		}

// 		memcpy(out, path, len);
// 		return 0;
// 	}

// 	len = strlen(last + 1);
// 	if (len > size) {
// 		return ENAMETOOLONG;
// 	}

// 	// memcpy(out, last + 1, len);
// 	strncpy(out, last + 1, size - len);

// 	return 0;
// }

// Get the name of the binary from argv[0].
// Basically `basename` but without allocating or trimming trailing slashes.
static const char* wtu_exe(const char* exe) {
	size_t slash = 0;
	size_t i = 0;

	for (; exe[i] != '\0'; ++i) {
		if (exe[i] == '/') {
			slash = i + 1;
		}
	}

	return exe + WTU_MIN(slash, i);
}

// Tokens/Instructions
#define INSTRUCTIONS \
	X(WTU_NONE, "none") \
	X(WTU_ENDFILE, "endfile") \
\
	X(WTU_WHITESPACE, "whitespace") \
	X(WTU_COMMENT, "comment") \
\
	/* Atoms */ \
	X(WTU_NUMBER, "number") \
	X(WTU_STRING, "string") \
	X(WTU_IDENT, "ident") \
	X(WTU_SYMBOL, "symbol") \
\
	/* Types */ \
	X(WTU_TYPE_NUMBER, "number type") \
	X(WTU_TYPE_STRING, "string type") \
	X(WTU_TYPE_ANY, "any type") \
	X(WTU_TYPE_FN, "function type") \
\
	/* Keywords */ \
	X(WTU_DEFINE, "define") \
	X(WTU_LET, "let") \
\
	/* Operators */ \
	X(WTU_OR, "or") \
	X(WTU_AND, "and") \
	X(WTU_NOT, "not") \
\
	X(WTU_ADD, "add") \
	X(WTU_SUB, "sub") \
	X(WTU_MUL, "mul") \
	X(WTU_DIV, "div") \
\
	X(WTU_COND, "cond") \
	X(WTU_APPLY, "apply") \
	X(WTU_EQUAL, "equal") \
\
	X(WTU_ARROW, "arrow") \
	X(WTU_TYPE, "type") \
\
	/* Grouping */ \
	X(WTU_LPAREN, "lparen") \
	X(WTU_RPAREN, "rparen") \
\
	X(WTU_LBRACKET, "lbracket") \
	X(WTU_RBRACKET, "rbracket")

#define X(x, y) x,

typedef enum {
	INSTRUCTIONS
} wtu_instr_kind_t;

#undef X

#define X(x, y) [x] = #x,
const char* WTU_INSTR_KIND_TO_STR[] = {INSTRUCTIONS};
#undef X

#define X(x, y) [x] = y,
const char* WTU_INSTR_TO_STR[] = {INSTRUCTIONS};
#undef X

#undef INSTRUCTIONS

// Token/Instruction
typedef bool (*wtu_lexer_pred_t)(char);  // Used for lexer predicates

typedef struct wtu_instr_t wtu_instr_t;

struct wtu_instr_t {
	wtu_instr_kind_t kind;

	wtu_instr_t* next;
	wtu_instr_t* prev;

	union {
		struct {
			const char* ptr;
			const char* end;
		} str;

		size_t num;
	};
};

static const char* wtu_instr_str(wtu_instr_t instr) {
	return instr.str.ptr;
}

static size_t wtu_instr_strlen(wtu_instr_t instr) {
	return wtu_ptrdiff(instr.str.ptr, instr.str.end);
}

static wtu_instr_t wtu_instr_create(wtu_instr_kind_t kind, const char* ptr, const char* end) {
	return (wtu_instr_t){
		.kind = kind,

		.next = NULL,
		.prev = NULL,

		.str.ptr = ptr,
		.str.end = end,
	};
}

static wtu_instr_t WTU_INSTR_NONE = (wtu_instr_t){
	.kind = WTU_NONE,

	.next = NULL,
	.prev = NULL,

	.str.ptr = NULL,
	.str.end = NULL,
};

// Lexer
typedef struct {
	const char* const src;

	const char* ptr;
	const char* end;

	wtu_instr_t peek;
} wtu_lexer_t;

static bool wtu_lexer_take(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr);

static wtu_lexer_t wtu_lexer_create(wtu_logger_t* log, const char* ptr, const char* end) {
	wtu_lexer_t lx = (wtu_lexer_t){
		.src = ptr,
		.ptr = ptr,
		.end = end,
		.peek = wtu_instr_create(WTU_NONE, ptr, ptr),
	};

	wtu_lexer_take(log, &lx, NULL);

	return lx;
}

// Debugging/printing
// TODO: Make these functions returned a formatted buffer rather than
// calling printf within.
// TODO: Print position info for tokens?
// static void wtu_instr_print(wtu_lexer_t* lx, wtu_instr_t instr) {
// 	printf(
// 		"kind = %s, ptr = %p, end = %p, size = %lu\n",
// 		WTU_INSTR_TO_STR[instr.kind],
// 		(void*) instr.str.ptr,
// 		(void*) instr.str.end,
// 		wtu_ptrdiff(instr.str.ptr, instr.str.end));
// }

// static void wtu_lexer_print(wtu_lexer_t* lx, wtu_instr_t instr) {
// 	// TODO: Print lexer state in some readable fashion for debugging.
// 	// Some ideas:
// 	// - Print all tokens and then reset lexer state (set `ptr` to `src`)
// 	// - Print pointers, peek token
// 	// - Total number of tokens
// 	// - Line count
// 	// - Percentage of file that has been lexed
// }

// Basic stream interaction
static char wtu_peek(wtu_lexer_t* lx) {
	if (lx->ptr >= lx->end) {
		return '\0';
	}

	return *lx->ptr;
}

static char wtu_take(wtu_lexer_t* lx) {
	if (lx->ptr >= lx->end) {
		return '\0';
	}

	return *lx->ptr++;
}

// Conditional consumers
static bool wtu_take_if(wtu_lexer_t* lx, wtu_lexer_pred_t cond) {
	char c = wtu_peek(lx);

	if (!c || !cond(c)) {
		return false;
	}

	wtu_take(lx);
	return true;
}

// Same as take_if but just takes a character directly
// for common usecases.
static bool wtu_take_ifc(wtu_lexer_t* lx, char c) {
	if (c != wtu_peek(lx)) {
		return false;
	}

	wtu_take(lx);
	return true;
}

static bool wtu_take_str(wtu_lexer_t* lx, const char* str) {
	size_t length = strlen(str);

	if (lx->ptr + length >= lx->end) {
		return false;
	}

	// TODO: Use custom strncmp to avoid iterating strings twice.
	if (strncmp(lx->ptr, str, length) != 0) {
		return false;
	}

	lx->ptr += length;
	return true;
}

static bool wtu_take_while(wtu_lexer_t* lx, wtu_lexer_pred_t cond) {
	bool taken = false;

	while (wtu_take_if(lx, cond)) {
		taken = true;
	}

	return taken;
}

// Lexer predicates
static bool wtu_is_whitespace(char c) {
	return isspace(c);
}

static bool wtu_is_comment(char c) {
	return c != '\n';
}

static bool wtu_is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool wtu_is_digit(char c) {
	return c >= '0' && c <= '9';
}

static bool wtu_is_alphanum(char c) {
	return wtu_is_alpha(c) || wtu_is_digit(c);
}

static bool wtu_is_ident(char c) {
	return wtu_is_alphanum(c) || c == '_';
}

// Token producers
static bool wtu_produce_if(
	wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr, wtu_instr_kind_t kind, wtu_lexer_pred_t cond) {
	wtu_instr_t next_instr = wtu_instr_create(WTU_NONE, lx->ptr, lx->ptr);

	if (!wtu_take_if(lx, cond)) {
		return false;
	}

	next_instr.str.end = lx->ptr;
	next_instr.kind = kind;

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

static bool wtu_produce_while(
	wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr, wtu_instr_kind_t kind, wtu_lexer_pred_t cond) {
	wtu_instr_t next_instr = wtu_instr_create(WTU_NONE, lx->ptr, lx->ptr);

	if (!wtu_take_while(lx, cond)) {
		return false;
	}

	next_instr.str.end = lx->ptr;
	next_instr.kind = kind;

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

static bool wtu_produce_str(
	wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr, wtu_instr_kind_t kind, const char* str) {
	wtu_instr_t next_instr = wtu_instr_create(WTU_NONE, lx->ptr, lx->ptr);

	if (!wtu_take_str(lx, str)) {
		return false;
	}

	next_instr.str.end = lx->ptr;
	next_instr.kind = kind;

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

static bool wtu_produce_ident(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr) {
	wtu_instr_t next_instr = wtu_instr_create(WTU_NONE, lx->ptr, lx->ptr);

	if (!wtu_take_if(lx, wtu_is_alpha)) {
		return false;
	}

	wtu_take_while(lx, wtu_is_ident);

	next_instr.str.end = lx->ptr;

	// We could have used `wtu_produce_str` here to handle these cases
	// but we want "maximal munch" meaning that we lex the entire
	// identifier before trying to classify it. Why? because if we
	// didn't, an identifier like "letfoo" would actually be lexed
	// as 2 seperate tokens because it sees `let` and stops there.

	// Keywords
	if (wtu_strncmp(next_instr.str.ptr, next_instr.str.end, "let")) {
		next_instr.kind = WTU_LET;
	}

	else if (wtu_strncmp(next_instr.str.ptr, next_instr.str.end, "def")) {
		next_instr.kind = WTU_DEFINE;
	}

	// Operators
	else if (wtu_strncmp(next_instr.str.ptr, next_instr.str.end, "or")) {
		next_instr.kind = WTU_OR;
	}

	else if (wtu_strncmp(next_instr.str.ptr, next_instr.str.end, "and")) {
		next_instr.kind = WTU_AND;
	}

	else if (wtu_strncmp(next_instr.str.ptr, next_instr.str.end, "not")) {
		next_instr.kind = WTU_NOT;
	}

	// Types
	else if (wtu_strncmp(next_instr.str.ptr, next_instr.str.end, "int")) {
		next_instr.kind = WTU_TYPE_NUMBER;
	}

	else if (wtu_strncmp(next_instr.str.ptr, next_instr.str.end, "string")) {
		next_instr.kind = WTU_TYPE_STRING;
	}

	else if (wtu_strncmp(next_instr.str.ptr, next_instr.str.end, "any")) {
		next_instr.kind = WTU_TYPE_ANY;
	}

	// User identifier
	else {
		next_instr.kind = WTU_IDENT;
	}

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

static bool wtu_produce_symbol(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr) {
	wtu_instr_t next_instr = wtu_instr_create(WTU_NONE, lx->ptr, lx->ptr);

	if (!wtu_take_ifc(lx, '#')) {
		return false;
	}

	wtu_take_while(lx, wtu_is_ident);

	next_instr.str.end = lx->ptr;
	next_instr.kind = WTU_SYMBOL;

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

static bool wtu_produce_number(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr) {
	return wtu_produce_while(log, lx, instr, WTU_NUMBER, wtu_is_digit);
}

static bool wtu_produce_sigil(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr) {
#define WTU_PRODUCE_SIGIL(s, k) wtu_produce_str(log, lx, instr, s, k)
	// clang-format off

	return WTU_PRODUCE_SIGIL(WTU_ARROW,    "->") ||
	       WTU_PRODUCE_SIGIL(WTU_ADD,      "+")  ||
		   WTU_PRODUCE_SIGIL(WTU_SUB,      "-")  ||
		   WTU_PRODUCE_SIGIL(WTU_MUL,      "*")  ||
		   WTU_PRODUCE_SIGIL(WTU_DIV,      "/")  ||
		   WTU_PRODUCE_SIGIL(WTU_APPLY,    ".")  ||
		   WTU_PRODUCE_SIGIL(WTU_EQUAL,    "=")  ||
		   WTU_PRODUCE_SIGIL(WTU_LPAREN,   "(")  ||
		   WTU_PRODUCE_SIGIL(WTU_RPAREN,   ")")  ||
		   WTU_PRODUCE_SIGIL(WTU_LBRACKET, "[")  ||
		   WTU_PRODUCE_SIGIL(WTU_RBRACKET, "]")  ||
		   WTU_PRODUCE_SIGIL(WTU_COND,     "?")  ||
		   WTU_PRODUCE_SIGIL(WTU_TYPE,     "$");

	// clang-format on
#undef WTU_PRODUCE_SIGIL
}

static bool wtu_produce_whitespace(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr) {
	return wtu_produce_while(log, lx, instr, WTU_WHITESPACE, wtu_is_whitespace);
}

static bool wtu_produce_comment(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr) {
	wtu_instr_t next_instr = wtu_instr_create(WTU_NONE, lx->ptr, lx->ptr);

	if (!wtu_take_str(lx, "#!")) {
		return false;
	}

	if (!wtu_take_while(lx, wtu_is_comment)) {
		return false;
	}

	next_instr.str.end = lx->ptr;
	next_instr.kind = WTU_COMMENT;

	if (instr != NULL) {
		*instr = next_instr;
	}

	return true;
}

// Core lexer interface
// TODO: Reconsider implementation. Is it safe to always return true?
static bool wtu_lexer_peek(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr) {
	if (instr != NULL) {
		*instr = lx->peek;
	}

	return true;
}

// TODO: Make lexer_next produce all tokens and then wrap it in
// another function which skips whitespace and comments.
static bool wtu_lexer_take(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_t* instr) {
	wtu_instr_t next_instr = wtu_instr_create(WTU_NONE, lx->ptr, lx->ptr);

	while (wtu_produce_whitespace(log, lx, NULL) || wtu_produce_comment(log, lx, NULL)) {}

	// Handle EOF
	if (lx->ptr >= lx->end) {
		next_instr.kind = WTU_ENDFILE;
	}

	// Handle normal tokens
	else if (!(wtu_produce_ident(log, lx, &next_instr) || wtu_produce_symbol(log, lx, &next_instr) ||
				 wtu_produce_number(log, lx, &next_instr) || wtu_produce_sigil(log, lx, &next_instr))) {
		wtu_log(log, WTU_FAIL, "unknown character");
		return false;
	}

	// Return previously peeked token and then store newly
	// lexed token to be used on the next call to peek.
	if (instr != NULL) {
		wtu_lexer_peek(log, lx, instr);
	}

	lx->peek = next_instr;

	return true;
}

typedef struct {
	// Function definitions
	// Bindings
	// Typestack
} wtu_context_t;

static wtu_context_t wtu_context_create() {
	return (wtu_context_t){};
}

// Primary call that sets up lexer and context automatically.
static wtu_instr_t* wtu_parse(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx);

// Forward declarations for mutual recursion
// TODO: Make these functions return wtu_instr_t* linked lists.
static wtu_instr_t* wtu_parse_program(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx);
static wtu_instr_t* wtu_parse_expression(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx);
static wtu_instr_t* wtu_parse_function(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx);
static wtu_instr_t* wtu_parse_builtin(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx);
static wtu_instr_t* wtu_parse_literal(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx);
static wtu_instr_t* wtu_parse_type(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx);
static wtu_instr_t* wtu_parse_fntype(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx);
static wtu_instr_t* wtu_parse_assertion(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx);
static wtu_instr_t* wtu_parse_swizzle(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx);

// Convenience functions
typedef bool (*wtu_parser_pred_t)(wtu_instr_t);  // Used for parser predicates

static bool wtu_is_builtin(wtu_instr_t instr) {
	return
		// Logical
		instr.kind == WTU_OR || instr.kind == WTU_AND || instr.kind == WTU_NOT ||

		// Arithmetic
		instr.kind == WTU_ADD || instr.kind == WTU_SUB || instr.kind == WTU_MUL || instr.kind == WTU_DIV ||

		// Other
		instr.kind == WTU_COND || instr.kind == WTU_APPLY || instr.kind == WTU_EQUAL;
}

static bool wtu_is_literal(wtu_instr_t instr) {
	return instr.kind == WTU_NUMBER || instr.kind == WTU_STRING;
}

static bool wtu_is_primitive(wtu_instr_t instr) {
	return instr.kind == WTU_TYPE_NUMBER || instr.kind == WTU_TYPE_STRING || instr.kind == WTU_TYPE_ANY;
}

static bool wtu_is_expression(wtu_instr_t instr) {
	return wtu_is_literal(instr) || wtu_is_builtin(instr) || instr.kind == WTU_IDENT ||  // Call
		instr.kind == WTU_LBRACKET ||                                                    // Function
		instr.kind == WTU_TYPE ||                                                        // Type assertion
		instr.kind == WTU_LPAREN;                                                        // Swizzle
}

// Automatically handle peeking
static bool wtu_peek_is_kind(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_kind_t kind) {
	wtu_instr_t instr;
	wtu_lexer_peek(log, lx, &instr);

	return instr.kind == kind;
}

static bool wtu_peek_is(wtu_logger_t* log, wtu_lexer_t* lx, wtu_parser_pred_t cond) {
	wtu_instr_t instr;
	wtu_lexer_peek(log, lx, &instr);

	return cond(instr);
}

static bool wtu_peek_is_expression(wtu_logger_t* log, wtu_lexer_t* lx) {
	return wtu_peek_is(log, lx, wtu_is_expression);
}

static bool wtu_peek_is_builtin(wtu_logger_t* log, wtu_lexer_t* lx) {
	return wtu_peek_is(log, lx, wtu_is_builtin);
}

static bool wtu_peek_is_literal(wtu_logger_t* log, wtu_lexer_t* lx) {
	return wtu_peek_is(log, lx, wtu_is_literal);
}

static bool wtu_peek_is_primitive(wtu_logger_t* log, wtu_lexer_t* lx) {
	return wtu_peek_is(log, lx, wtu_is_primitive);
}

static bool wtu_peek_is_type(wtu_logger_t* log, wtu_lexer_t* lx) {
	return wtu_peek_is_primitive(log, lx) || wtu_peek_is_kind(log, lx, WTU_TYPE_FN);
}

// Parsing utilities
// TODO: `wtu_expect_kind` should call `wtu_expect_kind` to avoid redundant code.
static void wtu_expect_kind(wtu_logger_t* log, wtu_lexer_t* lx, wtu_instr_kind_t kind, const char* fmt, ...) {
	// TODO: Report line info from deck lexer. (use wtu_log_info function).

	if (wtu_peek_is_kind(log, lx, kind)) {
		return;
	}

	wtu_instr_t peeker;
	wtu_lexer_peek(log, lx, &peeker);

	WTU_INFO(log,
		"expected = %s, kind = %s, ptr = %p, end = %p, size = %lu, str = "
		"'%.*s'",
		WTU_INSTR_KIND_TO_STR[kind],
		WTU_INSTR_KIND_TO_STR[peeker.kind],
		(void*)peeker.str.ptr,
		(void*)peeker.str.end,
		wtu_ptrdiff(peeker.str.ptr, peeker.str.end),
		wtu_ptrdiff(peeker.str.ptr, peeker.str.end),
		peeker.str.ptr);

	va_list args;
	va_start(args, fmt);

	wtu_log(log, WTU_FAIL, fmt, args);

	va_end(args);

	exit(EXIT_FAILURE);
}

static void wtu_expect(wtu_logger_t* log, wtu_lexer_t* lx, wtu_parser_pred_t cond, const char* fmt, ...) {
	// TODO: Report line info from deck lexer. (use wtu_log_info function).

	if (wtu_peek_is(log, lx, cond)) {
		return;
	}

	va_list args;
	va_start(args, fmt);

	wtu_log_info_v(log, WTU_FAIL, NULL, NULL, NULL, fmt, args);

	va_end(args);

	exit(EXIT_FAILURE);
}

// Implementation of core parsing functions
static wtu_instr_t* wtu_parse(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx) {
	WTU_FUNCTION_ENTER(log);

	wtu_instr_t* program = wtu_parse_program(log, ctx, lx);

	return program;
}

// Core parsing functions
static wtu_instr_t* wtu_parse_program(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx) {
	WTU_FUNCTION_ENTER(log);

	wtu_instr_t* program = NULL;  // TODO: Use allocator API to construct linked list.

	while (wtu_peek_is_expression(log, lx)) {
		// TODO: Concatenate `expr` with `program` to build up a linked list of
		// instructions which will act as our intermediate representation in the
		// compiler.
		wtu_instr_t* expr = wtu_parse_expression(log, ctx, lx);
	}

	wtu_instr_t instr;
	wtu_lexer_peek(log, lx, &instr);

	wtu_expect_kind(log, lx, WTU_ENDFILE, "expected EOF but found: '%s'", WTU_INSTR_TO_STR[instr.kind]);

	return program;
}

static wtu_instr_t* wtu_parse_expression(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx) {
	WTU_FUNCTION_ENTER(log);

	wtu_expect(log, lx, wtu_is_expression, "expected an expression");

	wtu_instr_t* expression = NULL;  // TODO: Use allocator API to construct linked list.

	// Integers/strings
	if (wtu_peek_is_literal(log, lx)) {
		expression = wtu_parse_literal(log, ctx, lx);
	}

	// Builtin operators/functions
	else if (wtu_peek_is_builtin(log, lx)) {
		expression = wtu_parse_builtin(log, ctx, lx);
	}

	// Symbol reference
	else if (wtu_peek_is_kind(log, lx, WTU_IDENT)) {
		wtu_instr_t instr;
		wtu_lexer_take(log, lx, &instr);

		// TODO: Handle symbol reference that isn't a builtin
		WTU_WHEREAMI(log);
	}

	// Functions
	else if (wtu_peek_is_kind(log, lx, WTU_LBRACKET)) {
		expression = wtu_parse_function(log, ctx, lx);
	}

	// Swizzle
	else if (wtu_peek_is_kind(log, lx, WTU_LPAREN)) {
		expression = wtu_parse_swizzle(log, ctx, lx);
	}

	// Type assertion
	else if (wtu_peek_is_kind(log, lx, WTU_TYPE)) {
		// TODO: Should we even concatenate this expression? Maybe we should
		// just discard it.
		expression = wtu_parse_assertion(log, ctx, lx);
	}

	else {
		// TODO: Unreachable
		wtu_instr_t instr;
		wtu_lexer_peek(log, lx, &instr);

		wtu_log(log, WTU_FAIL, "unexpected token '%s'", WTU_INSTR_TO_STR[instr.kind]);

		exit(EXIT_FAILURE);
	}

	return expression;
}

static wtu_instr_t* wtu_parse_function(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx) {
	WTU_FUNCTION_ENTER(log);

	wtu_expect_kind(log, lx, WTU_LBRACKET, "expected '['");
	wtu_lexer_take(log, lx, NULL);

	wtu_instr_t* function = NULL;

	while (wtu_peek_is_expression(log, lx)) {
		// TODO: Concatenate `expr` with `function` to build up a linked list of
		// instructions which will act as our intermediate representation in the
		// compiler.
		wtu_instr_t* expr = wtu_parse_expression(log, ctx, lx);
	}

	wtu_expect_kind(log, lx, WTU_RBRACKET, "expected ']'");
	wtu_lexer_take(log, lx, NULL);

	return function;
}

static wtu_instr_t* wtu_parse_builtin(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx) {
	WTU_FUNCTION_ENTER(log);

	wtu_expect(log, lx, wtu_is_builtin, "expected a built-in");

	// TODO: Allocate this node on the heap and return the pointer.
	wtu_instr_t instr;
	wtu_lexer_take(log, lx, &instr);

	return NULL;
}

static wtu_instr_t* wtu_parse_literal(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx) {
	WTU_FUNCTION_ENTER(log);

	wtu_expect(log, lx, wtu_is_literal, "expected a literal");

	// TODO: Allocate this node on the heap and return the pointer.
	wtu_instr_t instr;
	wtu_lexer_take(log, lx, &instr);

	return NULL;
}

static wtu_instr_t* wtu_parse_type(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx) {
	WTU_FUNCTION_ENTER(log);

	wtu_instr_t instr;

	if (wtu_peek_is_kind(log, lx, WTU_TYPE_NUMBER)) {
		// TODO: Check top-of-stack is number.
		wtu_lexer_take(log, lx, &instr);
	}

	else if (wtu_peek_is_kind(log, lx, WTU_TYPE_STRING)) {
		// TODO: Check top-of-stack is string.
		wtu_lexer_take(log, lx, &instr);
	}

	else if (wtu_peek_is_kind(log, lx, WTU_TYPE_ANY)) {
		// TODO: Check that there is an element on the type stack. It doesn't
		// matter what it is.
		wtu_lexer_take(log, lx, &instr);
	}

	else if (wtu_peek_is_kind(log, lx, WTU_TYPE_FN)) {
		wtu_parse_fntype(log, ctx, lx);
	}

	return NULL;
}

static wtu_instr_t* wtu_parse_fntype(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx) {
	WTU_FUNCTION_ENTER(log);

	wtu_expect_kind(log, lx, WTU_TYPE_FN, "expected 'fn'");
	wtu_lexer_take(log, lx, NULL);

	wtu_expect_kind(log, lx, WTU_LPAREN, "expected '('");
	wtu_lexer_take(log, lx, NULL);

	// Parse input types
	while (wtu_peek_is_type(log, lx)) {
		wtu_parse_type(log, ctx, lx);
	}

	// Seperator
	wtu_expect_kind(log, lx, WTU_ARROW, "expected '->'");
	wtu_lexer_take(log, lx, NULL);

	// Parse output types
	while (wtu_peek_is_type(log, lx)) {
		wtu_parse_type(log, ctx, lx);
	}

	wtu_expect_kind(log, lx, WTU_RPAREN, "expected ')'");
	wtu_lexer_take(log, lx, NULL);

	return NULL;
}

static wtu_instr_t* wtu_parse_assertion(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx) {
	WTU_FUNCTION_ENTER(log);

	wtu_expect_kind(log, lx, WTU_TYPE, "expected '$'");
	wtu_lexer_take(log, lx, NULL);

	wtu_expect_kind(log, lx, WTU_LPAREN, "expected '('");
	wtu_lexer_take(log, lx, NULL);

	while (wtu_peek_is_type(log, lx)) {
		wtu_parse_type(log, ctx, lx);
	}

	wtu_expect_kind(log, lx, WTU_RPAREN, "expected ')'");
	wtu_lexer_take(log, lx, NULL);

	return NULL;  // This function doesn't return anything. It just uses the
				  // type-stack from the context to do assertions on what types
				  // are present. We might need to do some NULL checking from
				  // the caller level in this case.
}

static wtu_instr_t* wtu_parse_swizzle(wtu_logger_t* log, wtu_context_t* ctx, wtu_lexer_t* lx) {
	WTU_FUNCTION_ENTER(log);

	wtu_expect_kind(log, lx, WTU_LPAREN, "expected '('");
	wtu_lexer_take(log, lx, NULL);

	wtu_instr_t* swizzle = NULL;

	// TODO: Collect a list of input identifiers and a list of output
	// identifiers then match up where the old slots map to the new slots and
	// try to synthesize a series of primitive ops that encode the same mapping.

	// Parse at least one identifier on the left side. This is because we can
	// use swizzle to "drop" items from the stack where the right side is empty.
	wtu_expect_kind(log, lx, WTU_IDENT, "expected an identifier");

	do {
		// TODO: Figure out what swizzle should lower to. A series of primitive
		// dup, rot etc. or a single intrinsic.
		wtu_instr_t instr;
		wtu_lexer_take(log, lx, &instr);
	} while (wtu_peek_is_kind(log, lx, WTU_IDENT));

	// Seperator
	wtu_expect_kind(log, lx, WTU_ARROW, "expected '->'");
	wtu_lexer_take(log, lx, NULL);

	// Parse new stack slots.
	while (wtu_peek_is_kind(log, lx, WTU_IDENT)) {
		wtu_instr_t instr;
		wtu_lexer_take(log, lx, &instr);
	}

	wtu_expect_kind(log, lx, WTU_RPAREN, "expected ')'");
	wtu_lexer_take(log, lx, NULL);

	return swizzle;
}

#endif
