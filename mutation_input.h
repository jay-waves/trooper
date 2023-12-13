#ifndef THIRD_PARTY_TROOPER_MUTATION_INPUT_H_
#define THIRD_PARTY_TROOPER_MUTATION_INPUT_H_

#include <vector>

#include "./defs.h"
#include "./execution_metadata.h"

namespace trooper {

// {data (required), metadata (optional)} reference pairs as mutation inputs.
struct MutationInputRef {
  const ByteArray &data;
  const ExecutionMetadata *metadata = nullptr;
};

inline std::vector<ByteArray> CopyDataFromMutationInputRefs(
    const std::vector<MutationInputRef> &inputs) {
  std::vector<ByteArray> results;
  results.reserve(inputs.size());
  for (const auto &input : inputs) results.push_back(input.data);
  return results;
}

inline std::vector<MutationInputRef> GetMutationInputRefsFromDataInputs(
    const std::vector<ByteArray> &inputs) {
  std::vector<MutationInputRef> results;
  results.reserve(inputs.size());
  for (const auto &input : inputs) results.push_back({.data = input});
  return results;
}

}  // namespace trooper 

#endif  // THIRD_PARTY_TROOPER_MUTATION_INPUT_H_
