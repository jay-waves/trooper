/* 
 randomly sleep at indirect function call, like callback of ptr handler
0. pre-compile to object: `clang -c xx -o xx.o -fsanitize=address -fPIC`
1. just pass this object to clang in compile-time
2. pass this object to cmake args: `-DCMAKE_CXX_FLAGS="sleep-rt.o -fsanitize-coverage
=func,trace-pc-guard,indirect-calls"`
*/
#include <sanitizer/coverage_interface.h>
#include <cstdint>
#include <ctime>
#include <chrono>
#include <cstdio>
#include <random>
#include <thread>

using namespace std;

// with -fsanitize-coverage=indirect-calls, run before callee entry
extern "C" void __sanitizer_cov_trace_pc_indir(void *callee) {
  static mt19937 gen(random_device{}()); // init PRNG
  static uniform_real_distribution<> dis(0.0, 1.0);
  auto slp_time = dis(gen); 
  if (slp_time > 0.995) {
    // get a big sleep time with probability of 0.5%
    slp_time = 3.0 * dis(gen);
  } else {
    slp_time /= 10.0;
  }
  
  auto sec = static_cast<int>(slp_time);
  auto ns = static_cast<int>((slp_time - sec) * 1e9);
  this_thread::sleep_for(chrono::seconds(sec) + chrono::nanoseconds(ns));

  // get symbols
  void *pc = __builtin_return_address(0);
  char pc_descr[1024];
  __sanitizer_symbolize_pc(pc, "%p %F %L", pc_descr, sizeof(pc_descr));
  fprintf(stderr, "indirect call: %s\n", pc_descr);
}

// with -fsan...=trace-pc-guard
extern "C" void __sanitizer_cov_trace_pc_guard_init(uint32_t * start, uint32_t * stop) {
  // asan report path
  __sanitizer_set_report_path("/home/JayWaves/log/asan");
}

// with -fsan...=func,trace-pc-guard, run after func entry 
extern "C" void __sanitizer_cov_trace_pc_guard(uint32_t * guard) {
  // do nothing
}

  // the full list of available symbolization placeholders:
  //   %% - represents a '%' character;
  //   %n - frame number (copy of frame_no);
  //   %p - PC in hex format;
  //   %m - path to module (binary or shared object);
  //   %o - offset in the module in hex format;
  //   %f - function name;
  //   %q - offset in the function in hex format (*if available*);
  //   %s - path to source file;
  //   %l - line in the source file;
  //   %c - column in the source file;
  //   %F - if function is known to be <foo>, prints "in <foo>", possibly
  //        followed by the offset in this function, but only if source file
  //        is unknown;
  //   %S - prints file/line/column information;
  //   %L - prints location information: file/line/column, if it is known, or
  //        module+offset if it is known, or (<unknown module>) string.
  //   %M - prints module basename and offset, if it is known, or PC.
