#include "./execution_metadata.h"

namespace trooper {

bool ExecutionMetadata::AppendCmpEntry(ByteSpan a, ByteSpan b) {
  if (a.size() != b.size()) return false;
  // Size must fit in a byte.
  if (a.size() >= 256) return false;
  cmp_data.push_back(a.size());
  cmp_data.insert(cmp_data.end(), a.begin(), a.end());
  cmp_data.insert(cmp_data.end(), b.begin(), b.end());
  return true;
}

bool ExecutionMetadata::Write(Blob::SizeAndTagT tag,
                              BlobSequence &outputs_blobseq) const {
  return outputs_blobseq.Write({tag, cmp_data.size(), cmp_data.data()});
}

void ExecutionMetadata::Read(Blob blob) {
  cmp_data.assign(blob.data, blob.data + blob.size);
}

bool ExecutionMetadata::ForEachCmpEntry(
    std::function<void(ByteSpan, ByteSpan)> callback) const {
  size_t i = 0;
  while (i < cmp_data.size()) {
    auto size = cmp_data[i];
    if (i + 2 * size + 1 > cmp_data.size()) return false;
    ByteSpan a(cmp_data.data() + i + 1, size);
    ByteSpan b(cmp_data.data() + i + size + 1, size);
    i += 1 + 2 * size;
    callback(a, b);
  }
  return true;
}

}  // namespace trooper
