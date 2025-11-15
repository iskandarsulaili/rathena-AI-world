#!/bin/bash
# test_all.sh - Run all P2P hybrid MMO core module tests

set -e

echo "=== Running ArenaAllocator test ==="
./arena_allocator_test

echo "=== Running P2PRuntimeFlags test ==="
./p2p_runtime_flags_test

echo "=== Running P2PRuntimeFlags compatibility test ==="
./p2p_runtime_flags_compat_test

echo "=== Running P2PRuntimeFlags atomic commit test ==="
./p2p_runtime_flags_atomic_commit_test

echo "=== All core module tests completed successfully ==="