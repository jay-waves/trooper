#ifndef THIRD_PARTY_TROOPER_KNOBS_H_
#define THIRD_PARTY_TROOPER_KNOBS_H_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <string_view>

#include "absl/types/span.h"
#include "./defs.h"

namespace trooper
{

  class Knobs
  {
  public:
    static constexpr size_t kNumKnobs = 16;
    using value_type = uint8_t;
    using signed_value_type = int8_t;

    // associate a `knob_name` with new `knob_id`
    // Must be called at the process startup (assign the result to a global):
    //   static const KnobId knob_weight_of_foo = Knobs::NewId("weight_of_foo");
    // Will trap if runs out of IDs.
    static size_t NewId(std::string_view knob_name);

    // Returns the name associated with `knob_id`.
    static std::string_view Name(size_t knob_id)
    {
      return knob_names_[knob_id];
    }

    // Sets all knobs to the same value `value`.
    void Set(value_type value)
    {
      for (auto& knob : knobs_)
      {
        knob = value;
      }
    }

    // Sets the knobs to values from `values`. If `values.size() < kNumKnobs`,
    // only the first `values.size()` values will be set.
    void Set(absl::Span<const value_type> values)
    {
      size_t n = std::min(kNumKnobs, values.size());
      for (size_t i = 0; i < n; ++i)
      {
        knobs_[i] = values[i];
      }
    }

    // Returns the value associated with `knob_id`.
    value_type Value(size_t knob_id) const
    {
      if (knob_id >= kNumKnobs)
        __builtin_trap();
      return knobs_[knob_id];
    }

    // Returns the signed value associated with `knob_id`.
    signed_value_type SignedValue(size_t knob_id) const { return Value(knob_id); }

    // Calls `callback(Name, Value)` for every KnobId created by NewId().
    void ForEachKnob(
      const std::function<void(std::string_view, Knobs::value_type)>& callback)
      const
    {
      for (size_t i = 0; i < next_id_; ++i)
      {
        callback(Name(i), Value(i));
      }
    }

    // Uses knob values associated with knob_ids as probability weights for
    // respective choices.
    // E.g. if knobs.Value(knobA) == 100 and knobs.Value(knobB) == 10, then
    // Choose<...>({knobA, knobB}, {A, B}, rng()) is approximately 10x more likely
    // to return A than B.
    //
    // If all knob values are zero, behaves as if they were all 1.
    size_t Choose(absl::Span<const size_t> knob_ids,
      uint64_t random) const
    {
      size_t sum = 0;
      for (auto knob_id : knob_ids)
      {
        sum += Value(knob_id);
      }
      if (sum == 0)
        return knob_ids[random % knob_ids.size()];
      random %= sum;
      size_t partial_sum = 0;
      size_t idx = 0;
      for (auto knob_id : knob_ids)
      {
        partial_sum += Value(knob_id);
        if (partial_sum > random)
          return knob_ids[idx];
        ++idx;
      }
      __builtin_unreachable();
    }

    // Chooses between two strategies, i.e. returns true or false.
    // Treats the value of the knob associated with `knob_id` as signed integer.
    // If knob == -128, returns false. If knob == 127 returns true.
    // For other values, returns randomly true of false, with higher probability
    // of true for higher values of knob.
    // If knob == 0, returns true with a ~ 50% chance.
    // `random` is a random number used to produce random choice.
    bool GenerateBool(size_t knob_id, uint64_t random) const
    {
      signed_value_type signed_value = SignedValue(knob_id); // in [-128,127]
      signed_value_type rand = random % 255 - 127;           // in [-127,127]
      // signed_value == 127 => always true.
      // signed_value == -128 => always false.
      // signed_value == 0 => true ~ half the time.
      return signed_value >= rand;
    }

  private:
    static size_t next_id_;
    static std::string_view knob_names_[kNumKnobs];
    value_type knobs_[kNumKnobs] = {};
  };

} // namespace trooper

#endif // THIRD_PARTY_TROOPER_KNOBS_H_
