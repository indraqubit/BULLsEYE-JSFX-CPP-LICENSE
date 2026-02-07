#!/bin/bash

# ========================================================================
# BULLsEYE Test Runner Script
# ========================================================================
#
# This script compiles and runs all BULLsEYE tests including:
# - DSP unit tests
# - Integration tests
#
# Usage:
#   ./tests/run_tests.sh              # Run all tests
#   ./tests/run_tests.sh build        # Build tests only
#   ./tests/run_tests.sh run          # Run tests only (assumes already built)
#   ./tests/run_tests.sh clean        # Clean build directory
#   ./tests/run_tests.sh help         # Show this help message
#
# ========================================================================

set -e  # Exit on error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build/tests"
TEST_OUTPUT_DIR="${PROJECT_ROOT}/build/test_results"

# Test executable names
DSP_TEST_EXE="BULLsEYETests"
DSP_TEST_SOURCE="tests/CMakeLists.txt"

# ========================================================================
# HELPER FUNCTIONS
# ========================================================================

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_header() {
    echo ""
    echo "============================================================"
    echo " $1"
    echo "============================================================"
}

print_usage() {
    echo ""
    echo "BULLsEYE Test Runner"
    echo ""
    echo "Usage: $0 [command]"
    echo ""
    echo "Commands:"
    echo "  (none)    Build and run all tests"
    echo "  build     Build tests only"
    echo "  run       Run tests only (assumes already built)"
    echo "  clean     Clean build directory"
    echo "  help      Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0              # Build and run all tests"
    echo "  $0 build        # Build tests"
    echo "  $0 run          # Run tests"
    echo ""
}

# ========================================================================
# BUILD FUNCTIONS
# ========================================================================

check_dependencies() {
    print_header "Checking Dependencies"

    # Check CMake
    if ! command -v cmake &> /dev/null; then
        log_error "CMake not found. Please install CMake 3.15 or later."
        exit 1
    fi
    log_success "CMake found: $(cmake --version | head -n1)"

    # Check C++ compiler
    if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
        log_error "No C++ compiler found. Please install g++ or clang++."
        exit 1
    fi

    if command -v g++ &> /dev/null; then
        log_success "g++ found: $(g++ --version | head -n1)"
    fi

    if command -v clang++ &> /dev/null; then
        log_success "clang++ found: $(clang++ --version | head -n1)"
    fi

    # Check GoogleTest
    if ! command -v pkg-config &> /dev/null; then
        log_warning "pkg-config not found - will try to find GTest via CMake"
    fi

    # Check JUCE modules
    if [ ! -d "${PROJECT_ROOT}/modules/JUCE" ]; then
        log_error "JUCE modules not found at ${PROJECT_ROOT}/modules/JUCE"
        log_info "Please ensure JUCE is properly submodule or symlinked."
        exit 1
    fi
    log_success "JUCE modules found"
}

create_build_directory() {
    if [ -d "${BUILD_DIR}" ]; then
        log_info "Build directory exists: ${BUILD_DIR}"
    else
        log_info "Creating build directory: ${BUILD_DIR}"
        mkdir -p "${BUILD_DIR}"
    fi

    # Create test results directory
    if [ ! -d "${TEST_OUTPUT_DIR}" ]; then
        mkdir -p "${TEST_OUTPUT_DIR}"
    fi
}

configure_tests() {
    print_header "Configuring Tests"

    cd "${BUILD_DIR}"

    # Configure with CMake
    log_info "Running CMake configuration..."

    # Determine generator
    if [ -n "$CMAKE_GENERATOR" ]; then
        GENERATOR="$CMAKE_GENERATOR"
    elif command -v ninja &> /dev/null; then
        GENERATOR="Ninja"
    else
        GENERATOR="Unix Makefiles"
    fi

    log_info "Using generator: ${GENERATOR}"

    cmake \
        -G "${GENERATOR}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=17 \
        "${PROJECT_ROOT}" \
        -DCMAKE_MODULE_PATH="${PROJECT_ROOT}/tests" \
        || {
            log_error "CMake configuration failed"
            exit 1
        }

    log_success "CMake configuration complete"
}

build_tests() {
    print_header "Building Tests"

    cd "${BUILD_DIR}"

    # Get number of CPU cores
    if command -v nproc &> /dev/null; then
        NPROC=$(nproc)
    elif [ -f /proc/cpuinfo ]; then
        NPROC=$(grep -c ^processor /proc/cpuinfo)
    else
        NPROC=2
    fi

    log_info "Building with ${NPROC} parallel jobs..."

    # Build using appropriate command
    if command -v ninja &> /dev/null; then
        ninja -j${NPROC} BULLsEYETests || {
            log_error "Build failed"
            exit 1
        }
    else
        make -j${NPROC} BULLsEYETests || {
            log_error "Build failed"
            exit 1
        }
    fi

    # Verify test executable exists (check both possible locations)
    local test_exe_path
    if [ -f "${BUILD_DIR}/tests/BULLsEYETests" ]; then
        test_exe_path="${BUILD_DIR}/tests/BULLsEYETests"
    elif [ -f "${BUILD_DIR}/BULLsEYETests" ]; then
        test_exe_path="${BUILD_DIR}/BULLsEYETests"
    else
        log_error "Test executable not found at ${BUILD_DIR}/BULLsEYETests or ${BUILD_DIR}/tests/BULLsEYETests"
        exit 1
    fi

    log_success "Test build complete"
    log_success "Test executable: ${test_exe_path}"
}

# ========================================================================
# RUN FUNCTIONS
# ========================================================================

run_dsp_tests() {
    print_header "Running DSP Tests"

    cd "${BUILD_DIR}"

    # Find test executable (check both possible locations)
    local test_exe
    if [ -f "./tests/BULLsEYETests" ]; then
        test_exe="./tests/BULLsEYETests"
    elif [ -f "./BULLsEYETests" ]; then
        test_exe="./BULLsEYETests"
    else
        log_error "Test executable not found. Run './tests/run_tests.sh build' first."
        exit 1
    fi

    log_info "Executing DSP tests..."

    # Run tests with output formatting
    "${test_exe}" \
        --gtest_color=yes \
        --gtest_print_time=1 \
        --gtest_output="xml:${TEST_OUTPUT_DIR}/BULLsEYETests.xml" \
        --gtest_output="console:${TEST_OUTPUT_DIR}/BULLsEYETests_console.txt" \
        2>&1 | tee "${TEST_OUTPUT_DIR}/test_output.txt"

    local exit_code=${PIPESTATUS[0]}

    return $exit_code
}

generate_test_report() {
    print_header "Generating Test Report"

    if [ ! -f "${TEST_OUTPUT_DIR}/test_output.txt" ]; then
        log_warning "No test output found to generate report"
        return
    fi

    local report_file="${TEST_OUTPUT_DIR}/test_report.html"

    cat > "${report_file}" << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <title>BULLsEYE Test Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background: #333; color: white; padding: 20px; border-radius: 5px; }
        .pass { color: green; }
        .fail { color: red; }
        .summary { margin: 20px 0; padding: 15px; background: #f0f0f0; border-radius: 5px; }
        pre { background: #f5f5f5; padding: 15px; overflow-x: auto; border-radius: 5px; }
    </style>
</head>
<body>
    <div class="header">
        <h1>BULLsEYE Plugin Test Report</h1>
        <p>Generated: TIMESTAMP_PLACEHOLDER</p>
    </div>
    <div class="summary">
        <h2>Summary</h2>
        <p>See console output for detailed test results.</p>
    </div>
    <h2>Test Categories</h2>
    <ul>
        <li><strong>APVTS <-> DSP Binding:</strong> Tests parameter binding between APVTS and DSP core</li>
        <li><strong>State Management:</strong> Tests save/load and reset functionality</li>
        <li><strong>Sample Rate:</strong> Tests various sample rate configurations</li>
        <li><strong>Buffer Size:</strong> Tests various buffer size configurations</li>
        <li><strong>Channel Configuration:</strong> Tests stereo and mono processing</li>
        <li><strong>Parameter Propagation:</strong> Tests parameter change effects</li>
        <li><strong>True Peak Detection:</strong> Tests peak detection accuracy</li>
        <li><strong>LUFS Integration:</strong> Tests gated integration behavior</li>
        <li><strong>Normalization:</strong> Tests normalized value calculations</li>
        <li><strong>Deviation:</strong> Tests deviation calculations</li>
        <li><strong>Edge Cases:</strong> Tests edge case handling</li>
        <li><strong>Performance:</strong> Tests performance under load</li>
    </ul>
    <h2>Test Output</h2>
    <pre>TEST_OUTPUT_PLACEHOLDER</pre>
</body>
</html>
EOF

    # Replace placeholders
    sed -i "s/TIMESTAMP_PLACEHOLDER/$(date)/g" "${report_file}"

    # Include test output
    if [ -f "${TEST_OUTPUT_DIR}/test_output.txt" ]; then
        local output_content=$(cat "${TEST_OUTPUT_DIR}/test_output.txt" | head -1000)
        sed -i "s/TEST_OUTPUT_PLACEHOLDER/$(echo "${output_content}" | sed 's/$/\\n/' | tr '\n' ' ' | sed 's/\"/\\"/g')/g" "${report_file}"
    fi

    log_success "Report generated: ${report_file}"
}

# ========================================================================
# CLEAN FUNCTIONS
# ========================================================================

clean_build() {
    print_header "Cleaning Build Directory"

    if [ -d "${BUILD_DIR}" ]; then
        rm -rf "${BUILD_DIR}"
        log_success "Cleaned build directory: ${BUILD_DIR}"
    else
        log_info "Build directory already clean"
    fi

    if [ -d "${TEST_OUTPUT_DIR}" ]; then
        rm -rf "${TEST_OUTPUT_DIR}"
        log_success "Cleaned test results directory: ${TEST_OUTPUT_DIR}"
    fi
}

# ========================================================================
# MAIN
# ========================================================================

main() {
    local command="${1:-}"

    echo ""
    echo "============================================================"
    echo " BULLsEYE Test Runner"
    echo "============================================================"
    echo ""
    echo "Project Root: ${PROJECT_ROOT}"
    echo "Build Dir:    ${BUILD_DIR}"
    echo ""

    case "${command}" in
        help|--help|-h)
            print_usage
            exit 0
            ;;
        clean)
            clean_build
            exit 0
            ;;
        build)
            check_dependencies
            create_build_directory
            configure_tests
            build_tests
            exit 0
            ;;
        run)
            run_dsp_tests
            exit $?
            ;;
        "")
            check_dependencies
            create_build_directory
            configure_tests
            build_tests

            # Run tests
            local test_result=0
            run_dsp_tests || test_result=$?

            # Generate report
            generate_test_report

            if [ ${test_result} -eq 0 ]; then
                print_header "ALL TESTS PASSED"
                log_success "All BULLsEYE tests passed successfully!"
            else
                print_header "SOME TESTS FAILED"
                log_error "Some tests failed. Check output for details."
            fi

            exit ${test_result}
            ;;
        *)
            log_error "Unknown command: ${command}"
            print_usage
            exit 1
            ;;
    esac
}

# Run main function
main "$@"
