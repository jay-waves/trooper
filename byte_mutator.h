
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string_view>
#include <vector>

#include "./knobs.h"

namespace trooper
{
// This class allows to mutate a ByteArray in different ways.
// All mutations expect and guarantee that `data` remains non-empty
// since there is only one possible empty input and it's uninteresting.
//
// This class is thread-compatible.
// Typical usage is to have one such object per thread.
class ByteArrayMutator
{
public:
  // CTOR. Initializes the internal RNG with `seed` (`seed` != 0).
  // Keeps a const reference to `knobs` throughout the lifetime.
  ByteArrayMutator(const Knobs& knobs, uintptr_t seed) : rng_(seed), knobs_(knobs)
  {
    if (seed == 0)
      __builtin_trap();  // We don't include logging.h here.
  }

  // Adds `dict_entries` to an internal dictionary.
  void AddToDictionary(const std::vector<ByteArray>& dict_entries);

  // Populates the internal CmpDictionary using execution `metadata`.
  // Returns false on failure, true otherwise.
  bool SetMetadata(const ExecutionMetadata& metadata)
  {
    return cmp_dictionary_.SetFromMetadata(metadata);
  }

  // Takes non-empty `inputs`, produces `num_mutants` mutations in `mutants`.
  // Old contents of `mutants` are discarded.
  void MutateMany(const std::vector<MutationInputRef>& inputs, size_t num_mutants, std::vector<ByteArray>& mutants);

  using CrossOverFn = void (ByteArrayMutator::*)(ByteArray&, const ByteArray&);

  // Mutates `data` by inserting a random part from `other`.
  void CrossOverInsert(ByteArray& data, const ByteArray& other);

  // Mutates `data` by overwriting some of it with a random part of `other`.
  void CrossOverOverwrite(ByteArray& data, const ByteArray& other);

  // Applies one of {CrossOverOverwrite, CrossOverInsert}.
  void CrossOver(ByteArray& data, const ByteArray& other);

  // Type for a Mutator member-function.
  // Every mutator function takes a ByteArray& as an input, mutates it in place
  // and returns true if mutation took place. In some cases mutation may fail
  // to happen, e.g. if EraseBytes() is called on a 1-byte input.
  // Fn is test-only public.
  using Fn = bool (ByteArrayMutator::*)(ByteArray&);

  // All public functions below are mutators.
  // They return true iff a mutation took place.

  // Applies some random mutation to data.
  bool Mutate(ByteArray& data);

  // Applies some random mutation that doesn't change size.
  bool MutateSameSize(ByteArray& data);

  // Applies some random mutation that decreases size.
  bool MutateDecreaseSize(ByteArray& data);

  // Applies some random mutation that increases size.
  bool MutateIncreaseSize(ByteArray& data);

  // Flips a random bit.
  bool FlipBit(ByteArray& data);

  // Swaps two bytes.
  bool SwapBytes(ByteArray& data);

  // Changes a random byte to a random value.
  bool ChangeByte(ByteArray& data);

  // Overwrites a random part of `data` with a random dictionary entry.
  bool OverwriteFromDictionary(ByteArray& data);

  // Overwrites a random part of `data` with an entry suggested by the internal
  // CmpDictionary.
  bool OverwriteFromCmpDictionary(ByteArray& data);

  // Inserts random bytes.
  bool InsertBytes(ByteArray& data);

  // Inserts a random dictionary entry at random position.
  bool InsertFromDictionary(ByteArray& data);

  // Erases random bytes.
  bool EraseBytes(ByteArray& data);

  // Set size alignment for mutants with modified sizes. Some mutators do not
  // change input size, but mutators that insert or erase bytes will produce
  // mutants with aligned sizes (if possible).
  //
  // Returns true if new size alignment was accepted. Returns false if max
  // length is not a multiple of the specified size alignment.
  bool set_size_alignment(size_t size_alignment)
  {
    if ((max_len_ != std::numeric_limits<size_t>::max()) && (max_len_ % size_alignment != 0))
    {
      return false;
    }
    size_alignment_ = size_alignment;
    return true;
  }

  // Set max length in bytes for mutants with modified sizes.
  //
  // Returns true if new max length was accepted. Returns false if specified max
  // length is not a multiple of size alignment.
  bool set_max_len(size_t max_len)
  {
    if ((max_len != std::numeric_limits<size_t>::max()) && (max_len % size_alignment_ != 0))
    {
      return false;
    }
    max_len_ = max_len;
    return true;
  }

private:
  // Size alignment in bytes to generate mutants.
  //
  // For example, if size_alignment_ is 1, generated mutants can have any
  // number of bytes. If size_alignment_ is 4, generated mutants will have sizes
  // that are 4-byte aligned.
  size_t size_alignment_ = 1;

  // Max length of a generated mutant in bytes.
  size_t max_len_ = std::numeric_limits<size_t>::max();

  Rng rng_;
  const Knobs& knobs_;
  std::vector<DictEntry> dictionary_;
  CmpDictionary cmp_dictionary_;
};
}  // namespace trooper