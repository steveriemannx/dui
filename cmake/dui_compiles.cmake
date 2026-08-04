# Used to detect some compile arguments
include(CheckCXXSourceCompiles)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Check AVX support
    set(CMAKE_REQUIRED_FLAGS "-mavx")
    check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m256 a = _mm256_set1_ps(0.0f);
            return 0;
        }
    " DUI_HAVE_AVX)

    # Check AVX2 support
    set(CMAKE_REQUIRED_FLAGS "-mavx2")
    check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m256i a = _mm256_set1_epi32(0);
            return 0;
        }
    " DUI_HAVE_AVX2)

    # Restore the detection flag to avoid polluting subsequent try_compile calls (e.g. CEF's compiler feature detection)
    set(CMAKE_REQUIRED_FLAGS "")
endif()