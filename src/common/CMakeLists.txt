# CMakeLists.txt for rAthena Hybrid P2P MMO Core Modules

add_library(rathena_common
    worker_pool.cpp
    entity_simulation_worker.cpp
    arena_allocator.cpp
    p2p_runtime_flags.cpp
    entity_state.hpp
    worker_pool.hpp
    entity_simulation_worker.hpp
    arena_allocator.hpp
    p2p_runtime_flags.hpp
)

add_executable(p2p_runtime_flags_test p2p_runtime_flags_test.cpp p2p_runtime_flags.cpp)
add_executable(p2p_runtime_flags_compat_test p2p_runtime_flags_compat_test.cpp p2p_runtime_flags.cpp)
add_executable(p2p_runtime_flags_atomic_commit_test p2p_runtime_flags_atomic_commit_test.cpp p2p_runtime_flags.cpp)
add_executable(arena_allocator_test arena_allocator_test.cpp arena_allocator.cpp)

add_executable(test_all test_all.sh)

target_link_libraries(p2p_runtime_flags_test rathena_common pthread)
target_link_libraries(p2p_runtime_flags_compat_test rathena_common pthread)
target_link_libraries(p2p_runtime_flags_atomic_commit_test rathena_common pthread)
target_link_libraries(arena_allocator_test rathena_common pthread)