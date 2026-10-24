# Assignment — Logarithmic Merging in C++ (starter code)

You implement one class, in one file:

| File | What |
|---|---|
| `src/logarithmic.cpp` | `LogarithmicIndex` — logarithmic merging (IIR Fig. 4.7) |

Everything else is provided and must not be changed: the on-disk format
(`index_io`), the in-memory auxiliary index (`MemoryIndex`), the k-way merge
(`merge.cpp`), the auxiliary + main index used as a baseline (`aux_main.cpp`),
the drivers in `tools/`, the tests and the experiment script.
Read `src/aux_main.cpp` first: it is a complete dynamic index built from the
same helpers you will use.

## Build

Requires a C++17 compiler and CMake 3.14+ (Linux, macOS, or Windows with
MSVC / MinGW).

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run

```
./build/test_main data/tiny.txt work_tiny          # 8 provided tests pass at the start; the 4 "log" tests are yours
./build/gen_corpus data/corpus_50k.txt 50000        # the assignment collection (seed 42)
./build/test_main data/corpus_50k.txt work_big      # the same tests on the big collection

./build/build_dynamic log data/corpus_50k.txt work 100000 out/log --trace log.csv
./build/build_dynamic aux data/corpus_50k.txt work 100000 out/aux   # the baseline
./build/verify data/corpus_50k.txt out/log          # compares with a naive index
./build/lookup out/log a bz zzz                     # prints postings

scripts/run_experiments.sh                          # all report experiments -> results/
```

On Windows, run the executables from `build\Release\` and the script from Git Bash.

## Files

```
include/ir/common.hpp        DocId, PostingsList, tokenizer, variable-byte codec
include/ir/index_io.hpp      IndexWriter, IndexCursor, IndexReader, CorpusReader, io_stats()
include/ir/memory_index.hpp  MemoryIndex, the auxiliary index Z0
include/ir/merge.hpp         merge_indexes, the provided k-way merge
include/ir/dynamic.hpp       DynamicIndex, ReaderCache, AuxMainIndex, LogarithmicIndex
include/ir/reference.hpp     naive indexer used by verify and the tests ONLY
src/logarithmic.cpp          YOUR FILE
src/                         everything else is provided
tools/                       build_dynamic, lookup, verify, gen_corpus
tests/test_main.cpp          12 tests; all must pass
data/tiny.txt                12 short documents for debugging
```

The auxiliary index size n is counted in **postings**, i.e. distinct
(term, docID) pairs, not in bytes. All I/O numbers in your report come from
`io_stats()`, which the provided writer and readers update automatically.
