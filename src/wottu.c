#include <wottu/wottu.h>

int main(int argc, const char* argv[]) {
	wtu_logger_t log = wtu_logger_create("toplevel");

	WTU_DIE(log, "foo");

	WTU_FUNCTION_ENTER(log);
	WTU_ASSERT(log, true, "expected TRUE");

	return 0;
}
