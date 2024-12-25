source ../gdb/chains.gdb

delete breakpoints
chain tests/unicode_test.cpp:6948
chain webpp/unicode/normalization.hpp:512
chain webpp/unicode/normalization.hpp:336
# rchain canonical_decompose_to
# rchain canonical_decompose
