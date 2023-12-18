#include "./knobs.h"

#include <cstdio>
#include <string_view>

namespace trooper {
  // notice that knobs is decoupled from the mutation method
  // knobs' value is decoupled from knobs_name (corresponding to a mehod) 

  size_t Knobs::NewId(std::string_view knob_name) {
    if (next_id_ >= kNumKnobs) {
      // run out the ids
      fprintf(stderr, "knobs::NewId: no more IDS left, aborting\n");
      __builtin_trap();
    }
    knob_names_[next_id_] = knob_name;
    return next_id_++;
  };

  // TODO: consider making this more similar to TossUp() and
  // requiring 1 knob fewer than choices.size().
  size_t Knobs::Choose(absl::Span<const size_t> knob_ids, uint64_t random) const {
    size_t sum = 0;
    for (auto knob_id : knob_ids) {
      sum += Value(knob_id);
    }
    if (sum == 0)
      return knob_ids[random % knob_ids.size()];
    random %= sum;
    size_t partial_sum = 0;
    size_t idx = 0;
    for (auto knob_id : knob_ids) {
      partial_sum += Value(knob_id);
      if (partial_sum > random)
        return knob_ids[idx];
      ++idx;
    }
    __builtin_unreachable();
  }
  // 用 TossUp 来实现 Choose, 概率算法
  // doi.org/10.48550/arXiv.1109.3627


  bool Knobs::TossUp(size_t knob_id, uint64_t random) const {
    uint8_t signed_value = Value(knob_id); // in [-128,127]
    uint8_t rand = random % 255 - 127;           // in [-127,127]
    // signed_value == 127 => always true.
    // signed_value == -128 => always false.
    // signed_value == 0 => true ~ half the time.
    return signed_value >= rand;
  }

} // namespace trooper
