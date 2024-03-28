/* 
 randomly sleep at indirect function call, like callback of ptr handler
0. pre-compile to object: `clang -c xx -o xx.o -fsanitize=address`
1. just pass this object to clang in compile-time
2. pass this object to cmake args: `-DCMAKE_CXX_FLAGS="sleep-rt.o -fsanitize-coverage
=func,trace-pc-guard,indirect-calls"`
*/
#include <sanitizer/coverage_interface.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <stdio.h>


// with -fsanitize-coverage=indirect-calls
extern "C" void __sanitizer_cov_trace_pc_indir(void *callee){ 
  double slp_time = (double)rand() / RAND_MAX; // between 0 and 1
  if (slp_time > 0.95){
    // get a big sleep time with probabiliry of 5%
    slp_time = 10.0 * ((double)rand() /RAND_MAX);
  }
  sleep(slp_time);
  //printf("sleep well!\n");
  return;
}

// with -fsan...=trace-pc-guard
extern "C" void __sanitizer_cov_trace_pc_guard_init(uint32_t * start, uint32_t * stop) {
  //printf("init random\n");
  srand(time(NULL));
}

// execute after trace_pc_indir in indirect calls
extern "C" void __sanitizer_cov_trace_pc_guard(uint32_t * guard) {
  //printf("get a guard\n");
  // do nothing
  return;
}

