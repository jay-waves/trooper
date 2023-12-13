
#include "./byte_mutator.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "./defs.h"

namespace trooper {

// rng * knobs: [0, 0, 0, 0, 0, 0] -->
// * same size mutate: filp bit, swap bytes, change byte, 
// overwrite from dictionary.
// * decrease size mutate: erase bytes.
// * increase size mutate: insert bytes, insert from dictionary.

bool ByteArrayMutator::Mutate(ByteArray &data) {
  // Individual mutator may fail to mutate and return false.
  // So we iterate a few times and expect one of the mutations will succeed.
  for (int iter = 0; iter < 15; iter++) {
    Fn mutator = nullptr;
    if (data.size() > max_len_) {
      // mutator = knobs[:-1]*rng_ 
      // choose from decrease size + rng
    } else if (data.size() == max_len_) {
      mutator = ...
      // choose from same size + decrease size + rng
    } else {
      mutator = ...
      // choose from same/decrease/increase size + rng
    }
    if ((this->*mutator)(data)) return true;
  }
  return false;
}

bool ByteArrayMutator::MutateSameSize(ByteArray &data) {
  auto mutator = ...
      // {&ByteArrayMutator::FlipBit, &ByteArrayMutator::SwapBytes,
      //  &ByteArrayMutator::ChangeByte,
      //  &ByteArrayMutator::OverwriteFromDictionary,
      //  &ByteArrayMutator::OverwriteFromCmpDictionary},
  return (this->*mutator)(data);
}


bool ByteArrayMutator::MutateIncreaseSize(ByteArray &data) {
  auto mutator = ...
      // {&ByteArrayMutator::InsertBytes, &ByteArrayMutator::InsertFromDictionary},
  return (this->*mutator)(data);
}

bool ByteArrayMutator::MutateDecreaseSize(ByteArray &data) {
  auto mutator = &ByteArrayMutator::EraseBytes;
  return (this->*mutator)(data);
}

bool ByteArrayMutator::FlipBit(ByteArray &data) {
  uintptr_t random = rng_();
  size_t bit_idx = random % (data.size() * 8);
  size_t byte_idx = bit_idx / 8;
  bit_idx %= 8;
  uint8_t mask = 1 << bit_idx;
  data[byte_idx] ^= mask;
  return true;
}

bool ByteArrayMutator::SwapBytes(ByteArray &data) {
  size_t idx1 = rng_() % data.size();
  size_t idx2 = rng_() % data.size();
  std::swap(data[idx1], data[idx2]);
  return true;
}

bool ByteArrayMutator::ChangeByte(ByteArray &data) {
  size_t idx = rng_() % data.size();
  data[idx] = rng_();
  return true;
}

bool ByteArrayMutator::InsertBytes(ByteArray &data) {
  // Don't insert too many bytes at once.
  const size_t kMaxInsertSize = 20;
  size_t num_new_bytes = rng_() % kMaxInsertSize + 1;
  num_new_bytes = RoundUpToAdd(data.size(), num_new_bytes);
  if (num_new_bytes > kMaxInsertSize) {
    num_new_bytes -= size_alignment_;
  }
  // There are N+1 positions to insert something into an array of N.
  size_t pos = rng_() % (data.size() + 1);
  // Fixed array to avoid memory allocation.
  std::array<uint8_t, kMaxInsertSize> new_bytes;
  for (size_t i = 0; i < num_new_bytes; i++) new_bytes[i] = rng_();
  data.insert(data.begin() + pos, new_bytes.begin(),
              new_bytes.begin() + num_new_bytes);
  return true;
}

bool ByteArrayMutator::EraseBytes(ByteArray &data) {
  if (data.size() <= size_alignment_) return false;
  // Ok to erase a sizable chunk since small inputs are good (if they
  // produce good features).
  size_t num_bytes_to_erase = rng_() % (data.size() / 2) + 1;
  num_bytes_to_erase = RoundDownToRemove(data.size(), num_bytes_to_erase);
  if (num_bytes_to_erase == 0) return false;
  size_t pos = rng_() % (data.size() - num_bytes_to_erase + 1);
  data.erase(data.begin() + pos, data.begin() + pos + num_bytes_to_erase);
  return true;
}

bool ByteArrayMutator::OverwriteFromDictionary(ByteArray &data) {
  if (dictionary_.empty()) return false;
  size_t dict_entry_idx = rng_() % dictionary_.size();
  const auto &dic_entry = dictionary_[dict_entry_idx];
  if (dic_entry.size() > data.size()) return false;
  size_t overwrite_pos = rng_() % (data.size() - dic_entry.size() + 1);
  std::copy(dic_entry.begin(), dic_entry.end(), data.begin() + overwrite_pos);
  return true;
}

bool ByteArrayMutator::InsertFromDictionary(ByteArray &data) {
  if (dictionary_.empty()) return false;
  size_t dict_entry_idx = rng_() % dictionary_.size();
  const auto &dict_entry = dictionary_[dict_entry_idx];
  // There are N+1 positions to insert something into an array of N.
  size_t pos = rng_() % (data.size() + 1);
  data.insert(data.begin() + pos, dict_entry.begin(), dict_entry.end());
  return true;
}

void ByteArrayMutator::CrossOverInsert(ByteArray &data,
                                       const ByteArray &other) {
  if ((data.size() % size_alignment_) + other.size() < size_alignment_) return;
  // insert other[first:first+size] at data[pos]
  size_t size = 1 + rng_() % other.size();
  size = RoundUpToAdd(data.size(), size);
  if (size > other.size()) {
    size -= size_alignment_;
  }
  size_t first = rng_() % (other.size() - size + 1);
  size_t pos = rng_() % (data.size() + 1);
  data.insert(data.begin() + pos, other.begin() + first,
              other.begin() + first + size);
}

void ByteArrayMutator::CrossOverOverwrite(ByteArray &data,
                                          const ByteArray &other) {
  // Overwrite data[pos:pos+size] with other[first:first+size].
  // Overwrite no more than half of data.
  size_t max_size = std::max(1UL, data.size() / 2);
  size_t first = rng_() % other.size();
  max_size = std::min(max_size, other.size() - first);
  size_t size = 1 + rng_() % max_size;
  size_t max_pos = data.size() - size;
  size_t pos = rng_() % (max_pos + 1);
  std::copy(other.begin() + first, other.begin() + first + size,
            data.begin() + pos);
}

// const KnobId knob_cross_over_insert_or_overwrite =
//     Knobs::NewId("cross_over_insert_or_overwrite");

void ByteArrayMutator::CrossOver(ByteArray &data, const ByteArray &other) {
  if (data.size() >= max_len_) {
    CrossOverOverwrite(data, other);
  } else {
    if (knobs_.GenerateBool(knob_cross_over_insert_or_overwrite, rng_())) {
      CrossOverInsert(data, other);
    } else {
      CrossOverOverwrite(data, other);
    }
  }
}

void ByteArrayMutator::MutateMany(const std::vector<MutationInputRef> &inputs,
                                  size_t num_mutants,
                                  std::vector<ByteArray> &mutants) {
  if (inputs.empty()) abort();
  // TODO(xinhaoyuan): Consider metadata in other inputs instead of always the
  // first one.
  SetMetadata(inputs[0].metadata != nullptr ? *inputs[0].metadata
                                            : ExecutionMetadata());
  size_t num_inputs = inputs.size();
  mutants.resize(num_mutants);
  for (auto &mutant : mutants) {
    mutant = inputs[rng_() % num_inputs].data;
    if (mutant.size() <= max_len_ &&
        knobs_.GenerateBool(knob_mutate_or_crossover, rng_())) {
      // Do crossover only if the mutant is not over the max_len_.
      // Perform crossover with some other input. It may be the same input.
      const auto &other_input = inputs[rng_() % num_inputs].data;
      CrossOver(mutant, other_input);
    } else {
      // Perform mutation.
      Mutate(mutant);
    }
  }
}
} // namespace trooper