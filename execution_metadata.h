#ifndef THIRD_PARTY_EXECUTION_METADATA_H_
#define THIRD_PARTY_EXECUTION_METADATA_H_

#include <functional>

#include "./defs.h"
#include "./shared_memory_blob_sequence.h"

namespace trooper {

struct ExecutionMetadata {
  // Appends a CMP entry comparing `a` and `b` to the metadata. Returns false if
  // the entry cannot be appended. Return true otherwise.
  bool AppendCmpEntry(ByteSpan a, ByteSpan b);

  // Enumerates through all CMP entries in the metadata by calling
  // `callback` on each of them. Returns false if there are invalid
  // entries. Returns true otherwise.
  bool ForEachCmpEntry(std::function<void(ByteSpan, ByteSpan)> callback) const;

  // Writes the contents to `outputs_blobseq` with header `tag`. Returns true
  // iff successful.
  bool Write(Blob::SizeAndTagT tag, BlobSequence &outputs_blobseq) const;

  // Reads the contents from `blob`.
  //
  // Note that the method does not check the blob tag, it should be checked by
  // the method users.
  void Read(Blob blob);

  // CMP entries are stored in one large ByteArray to minimize RAM consumption.
  // One CMP arg pair is stored as
  //  * `size` (1-byte value)
  //  * `value0` (`size` bytes)
  //  * `value1` (`size` bytes)
  ByteArray cmp_data;
};

}  // namespace trooper 

#endif  // THIRD_PARTY_EXECUTION_METADATA_H_
