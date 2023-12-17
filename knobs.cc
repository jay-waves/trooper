#include "./knobs.h"

#include <cstdio>
#include <string_view>

namespace trooper {
  size_t Knobs::next_id_ = 0;
  std::string_view Knobs::knob_names_[kNumKnobs];

  // notice that knobs is decoupled from the mutation method
  // knobs' value is decoupled from knobs_name (corresponding to a mehod) 

  // TODO: consider making this more similar to TossUp() and
  // requiring 1 knob fewer than choices.size().
  size_t Knobs::Choose(absl::Span<const size_t> knob_ids,
    uint64_t random) const {
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

  bool Knobs::TossUp(size_t knob_id, uint64_t random) const {
    signed_value_type signed_value = SignedValue(knob_id); // in [-128,127]
    signed_value_type rand = random % 255 - 127;           // in [-127,127]
    // signed_value == 127 => always true.
    // signed_value == -128 => always false.
    // signed_value == 0 => true ~ half the time.
    return signed_value >= rand;
  }

  size_t Knobs::NewId(std::string_view knob_name) {
    if (next_id_ >= kNumKnobs) {
      // run out the ids
      fprintf(stderr, "knobs::NewId: no more IDS left, aborting\n");
      __builtin_trap();
    }
    knob_names_[next_id_] = knob_name;
    return next_id_++;
  };
} // namespace trooper
