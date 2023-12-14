#include "./knobs.h"

#include <cstdio>
#include <string_view>

namespace trooper {
size_t Knobs::next_id_ = 0;
std::string_view Knobs::knob_names_[kNumKnobs];

KnobId Knobs::NewId(std::string_view knob_name) {
  if (next_id_ >= kNumKnobs) {
    // If we've run out of IDs, log using stderr (don't use extra deps).
    fprintf(stderr, "Knobs::NewId: no more IDs left, aborting\n");
    __builtin_trap();
  }
  knob_names_[next_id_] = knob_name;
  return next_id_++;
}

}  // namespace trooper
