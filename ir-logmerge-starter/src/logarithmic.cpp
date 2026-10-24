// logarithmic.cpp — THE ASSIGNMENT TASK.   Write your implementation here.
// Logarithmic merging, IIR Figure 4.7 (LMERGEADDTOKEN).
//
// Available to you (see dynamic.hpp, memory_index.hpp and merge.hpp):
//   z0_              the in-memory auxiliary index (MemoryIndex):
//                    z0_.write_to(base), z0_.lookup(term), z0_.clear(), z0_.empty()
//   new_temp_name()  a fresh base path inside the work directory
//   merge_indexes()  the provided k-way merge: use it for every merge
//   readers_         ReaderCache; disk_lookup(readers_, base, term) reads one list.
//                    Call readers_.forget(base) BEFORE remove_index / rename_index
//   union_postings(a, b), remove_index(base), rename_index(from, to),
//   write_empty_index(base)
//   stats_.merges    increment it after every call to merge_indexes
// stats_.flushes and stats_.max_disk_indexes are updated for you.
//
// src/aux_main.cpp implements the auxiliary + main index with the same
// helpers. Read it first: it is a complete example of a DynamicIndex.
#include "ir/dynamic.hpp"

#include <stdexcept>

namespace ir {

LogarithmicIndex::~LogarithmicIndex() {
    // TODO 5. Delete every generation that still exists on disk.
}

void LogarithmicIndex::flush() {
    // TODO 1. LMERGEADDTOKEN, overflow branch (IIR Figure 4.7):
    //           Z = Z0 written to a temporary index; clear Z0
    //           for i = 0, 1, 2, ...
    //               if gen_[i] exists:  Z = merge(gen_[i], Z); delete both inputs
    //               else:               gen_[i] = Z; return
    //         Grow gen_ when i reaches gen_.size().
    //         gen_[i] always holds OLDER documents than Z: pass it first.
    throw std::logic_error("LogarithmicIndex::flush: not implemented yet");
}

PostingsList LogarithmicIndex::lookup(const std::string& term) const {
    // TODO 2. Combine the postings of EVERY existing generation and of Z0
    //         into one sorted list.
    (void)term;
    throw std::logic_error("LogarithmicIndex::lookup: not implemented yet");
}

void LogarithmicIndex::finalize(const std::string& out_base) {
    // TODO 3. Flush Z0, then produce ONE index at out_base:
    //         0 generations -> empty index; 1 -> rename it;
    //         several -> a single merge_indexes call over all of them.
    (void)out_base;
    throw std::logic_error("LogarithmicIndex::finalize: not implemented yet");
}

std::size_t LogarithmicIndex::disk_indexes() const {
    // TODO 4a. Number of generations that currently exist.
    return 0;
}

std::uint64_t LogarithmicIndex::generation_mask() const {
    // TODO 4b. Bit i is set iff gen_[i] exists.
    return 0;
}

}  // namespace ir
