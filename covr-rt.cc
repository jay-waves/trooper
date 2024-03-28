#include <vector>
#include <cstdint>
#include <cstdio> // use sprintf, avoid init of std::cout
#include <algorithm> // std::min
#include <cstdlib> // for std::atexit
#include <sanitizer/coverage_interface.h>

// do not use -fsanitize-coverage while compiling this file (infinite recursive).
#include <iostream>

namespace trooper {

class TCovr {
public:
	// 初始化bitmap大小，此处每个元素代表一个字节
	TCovr(size_t size) : size_(size) {
		bitmap_.resize(size_, 0); 
	}

	// __attribute__((no_sanitize("coverage")))
	void Hit(uint32_t* guard) {
		uint32_t guard_id = *guard;
		if (guard_id < size_)
			if (++bitmap_[guard_id] == 255)
				*guard = 0;
	}

	void Reset() {
		std::fill(bitmap_.begin(), bitmap_.end(), 0);
	}

	void Write(const char* fn) {
		FILE* f = fopen(fn, "wb");
		if (!f) {
			fprintf(stderr, "failed to open %s\n", fn);
			return;
		}
		fwrite(bitmap_.data(), 1, bitmap_.size(), f);
		fclose(f);
	}

private:
	std::vector<uint8_t> bitmap_; // 存储覆盖信息的bitmap, 每个 guard 占一字节
	size_t size_; // 总 guard 数量
};

} // namespace trooper

// global coverage
static trooper::TCovr* covr = nullptr;

// register callbck at exit
static void WriteCovAtExit(void) {
	if (covr) {
		covr->Write("coverage.cov");
		covr->Reset();
	}
}

extern "C" void __sanitizer_cov_trace_pc_guard_init(uint32_t * start, uint32_t * stop) {
	static uint64_t N = 0; 
	if (start == stop || *start) // 如果已初始化，直接返回
		return;

	if (!covr) {
		covr = new trooper::TCovr(stop - start);
		std::atexit(WriteCovAtExit);
	}

	for (uint32_t* x = start; x < stop; x++)
		*x = ++N; // 唯一编号

	// test
	printf("hit new dso!\n");
}

extern "C" void __sanitizer_cov_trace_pc_guard(uint32_t * guard) {
	// maybe this guard is hotspot, skip it
	if (!*guard) return;
	if (covr) {
		covr->Hit(guard); 
	}
	// test
	printf("hit new edge at %d!\n", *guard);
}
