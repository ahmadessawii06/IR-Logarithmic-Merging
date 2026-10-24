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
    for (const std::string& base : gen_) {
        if (!base.empty()) {
            readers_.forget(base);
            remove_index(base);
        }
    }
}

void LogarithmicIndex::flush() {
    if (z0_.empty()) return;

    std::string carry = new_temp_name();
    z0_.write_to(carry);
    z0_.clear();

    for (std::size_t i = 0; ; ++i) {
        if (i == gen_.size()) gen_.emplace_back();
        if (gen_[i].empty()) {
            gen_[i] = carry;
            return;
        }

        std::string merged = new_temp_name();
        // The existing generation contains older documents than the carry.
        merge_indexes({gen_[i], carry}, merged);
        stats_.merges++;

        readers_.forget(gen_[i]);
        readers_.forget(carry);
        remove_index(gen_[i]);
        remove_index(carry);
        gen_[i].clear();
        carry = merged;
    }
}

PostingsList LogarithmicIndex::lookup(const std::string& term) const {
    PostingsList result = z0_.lookup(term);
    for (const std::string& base : gen_) {
        if (!base.empty()) {
            result = union_postings(result, disk_lookup(readers_, base, term));
        }
    }
    return result;
}

void LogarithmicIndex::finalize(const std::string& out_base) {
    flush();

    std::vector<std::string> inputs;
    // Higher occupied levels contain older documents.
    for (auto it = gen_.rbegin(); it != gen_.rend(); ++it) {
        if (!it->empty()) inputs.push_back(*it);
    }

    if (inputs.empty()) {
        write_empty_index(out_base);
    } else if (inputs.size() == 1) {
        readers_.forget(inputs.front());
        rename_index(inputs.front(), out_base);
    } else {
        merge_indexes(inputs, out_base);
        stats_.merges++;
        for (const std::string& base : inputs) {
            readers_.forget(base);
            remove_index(base);
        }
    }
    gen_.clear();
}

std::size_t LogarithmicIndex::disk_indexes() const {
    std::size_t count = 0;
    for (const std::string& base : gen_) {
        if (!base.empty()) ++count;
    }
    return count;
}

std::uint64_t LogarithmicIndex::generation_mask() const {
    std::uint64_t mask = 0;
    // A uint64_t can represent generation levels 0 through 63.
    for (std::size_t i = 0; i < gen_.size() && i < 64; ++i) {
        if (!gen_[i].empty()) mask |= (std::uint64_t{1} << i);
    }
    return mask;
}

}  // namespace ir
