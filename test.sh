#!/bin/bash
set -e
cd "$(dirname "$0")"
mkdir -p build_test
cd build_test

# Configure and build
cmake .. -DBUILD_FOR=LINUX -DCMAKE_BUILD_TYPE=Debug
cmake --build . --target test_phase_loading test_dependency_finding test_dependency_sorting test_missing_dependency test_duplicate_phases

# Run tests (from project root for correct working directory)
cd ..
echo "=== Running Tests ==="
./build_test/tests/test_phase_loading && echo "✓ test_phase_loading passed" || echo "✗ test_phase_loading failed"
./build_test/tests/test_dependency_finding && echo "✓ test_dependency_finding passed" || echo "✗ test_dependency_finding failed"
./build_test/tests/test_dependency_sorting && echo "✓ test_dependency_sorting passed" || echo "✗ test_dependency_sorting failed"
./build_test/tests/test_missing_dependency && echo "✓ test_missing_dependency passed" || echo "✗ test_missing_dependency failed"
./build_test/tests/test_duplicate_phases && echo "✓ test_duplicate_phases passed" || echo "✗ test_duplicate_phases failed"
echo ""
