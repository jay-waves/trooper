#include <vector>
#include <cstdint>
#include <algorithm> // For std::min
#include <sanitizer/coverage_interface.h>

namespace trooper{

class Coverage {
public:
    // 初始化bitmap大小，此处每个元素代表一个字节
    Coverage(size_t size) : size_(size) {
        bitmap_.resize(size_, 0); // 每个guard对应一个字节
    }

    // 标记某个guard为已覆盖，增加对应字节的值
    void HitCount(uint32_t guardID) {
        if (guardID < size_) {
            if (bitmap_[guardID] < 255) { // 防止溢出
                ++bitmap_[guardID];
            }
        }
    }

    void reset() {
        std::fill(bitmap_.begin(), bitmap_.end(), 0);
    }

private:
    std::vector<uint8_t> bitmap_; // 存储覆盖信息的bitmap
    size_t size_; // 总的guard数量
};

// global coverage
static Coverage* coverage = nullptr;

extern "C" void __sanitizer_cov_trace_pc_guard_init(uint32_t* start, uint32_t* stop) {
    static uint64_t N = 0; // 用于guard编号
    if (start == stop || *start) return; // 如果已初始化，直接返回

    if (!coverage) 
        coverage = new Coverage(stop - start);

    for (uint32_t* x = start; x < stop; x++) 
        *x = ++N; // 唯一编号
}

extern "C" void __sanitizer_cov_trace_pc_guard(uint32_t* guard) {
    if (!*guard) return; 

    if (coverage) {
        coverage->HitCount(*guard - 1); // 编号从1开始
    }

    *guard = 0; // 归零guard，不重复统计
}

} // namespace trooper



