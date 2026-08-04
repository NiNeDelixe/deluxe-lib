include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


include(CheckCXXSourceCompiles)


macro(deluxe_lib_supports_sanitizers)
  # Emscripten doesn't support sanitizers
  if(EMSCRIPTEN)
    set(SUPPORTS_UBSAN OFF)
    set(SUPPORTS_ASAN OFF)
  elseif((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)

    message(STATUS "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform")
    set(TEST_PROGRAM "int main() { return 0; }")

    # Check if UndefinedBehaviorSanitizer works at link time
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)

    if(HAS_UBSAN_LINK_SUPPORT)
      message(STATUS "UndefinedBehaviorSanitizer is supported at both compile and link time.")
      set(SUPPORTS_UBSAN ON)
    else()
      message(WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
      set(SUPPORTS_UBSAN OFF)
    endif()
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    if (NOT WIN32)
      message(STATUS "Sanity checking AddressSanitizer, it should be supported on this platform")
      set(TEST_PROGRAM "int main() { return 0; }")

      # Check if AddressSanitizer works at link time
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
      check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)

      if(HAS_ASAN_LINK_SUPPORT)
        message(STATUS "AddressSanitizer is supported at both compile and link time.")
        set(SUPPORTS_ASAN ON)
      else()
        message(WARNING "AddressSanitizer is NOT supported at link time.")
        set(SUPPORTS_ASAN OFF)
      endif()
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(deluxe_lib_setup_options)
  option(deluxe_lib_ENABLE_HARDENING "Enable hardening" ON)
  option(deluxe_lib_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    deluxe_lib_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    deluxe_lib_ENABLE_HARDENING
    OFF)

  deluxe_lib_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR deluxe_lib_PACKAGING_MAINTAINER_MODE)
    option(deluxe_lib_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(deluxe_lib_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(deluxe_lib_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(deluxe_lib_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(deluxe_lib_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(deluxe_lib_ENABLE_PCH "Enable precompiled headers" OFF)
    option(deluxe_lib_ENABLE_CACHE "Enable ccache" OFF)
  elseif(ENABLE_DEVELOPER_MODE)
    option(deluxe_lib_ENABLE_IPO "Enable IPO/LTO" ON)
    option(deluxe_lib_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(deluxe_lib_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(deluxe_lib_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(deluxe_lib_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(deluxe_lib_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(deluxe_lib_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(deluxe_lib_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(deluxe_lib_ENABLE_PCH "Enable precompiled headers" OFF)
    option(deluxe_lib_ENABLE_CACHE "Enable ccache" ON)
  else()
    option(deluxe_lib_ENABLE_IPO "Enable IPO/LTO" ON)
    option(deluxe_lib_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(deluxe_lib_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(deluxe_lib_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(deluxe_lib_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(deluxe_lib_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(deluxe_lib_ENABLE_PCH "Enable precompiled headers" OFF)
    option(deluxe_lib_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      deluxe_lib_ENABLE_IPO
      deluxe_lib_WARNINGS_AS_ERRORS
      deluxe_lib_ENABLE_SANITIZER_ADDRESS
      deluxe_lib_ENABLE_SANITIZER_LEAK
      deluxe_lib_ENABLE_SANITIZER_UNDEFINED
      deluxe_lib_ENABLE_SANITIZER_THREAD
      deluxe_lib_ENABLE_SANITIZER_MEMORY
      deluxe_lib_ENABLE_UNITY_BUILD
      deluxe_lib_ENABLE_CLANG_TIDY
      deluxe_lib_ENABLE_CPPCHECK
      deluxe_lib_ENABLE_LIZARD
      deluxe_lib_ENABLE_BLOATY
      deluxe_lib_ENABLE_COVERAGE
      deluxe_lib_ENABLE_PCH
      deluxe_lib_ENABLE_CACHE)
  endif()

  deluxe_lib_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (deluxe_lib_ENABLE_SANITIZER_ADDRESS OR deluxe_lib_ENABLE_SANITIZER_THREAD OR deluxe_lib_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(deluxe_lib_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(deluxe_lib_global_options)
  if(deluxe_lib_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    deluxe_lib_enable_ipo()
  endif()

  deluxe_lib_supports_sanitizers()

  if(deluxe_lib_ENABLE_HARDENING AND deluxe_lib_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR deluxe_lib_ENABLE_SANITIZER_UNDEFINED
       OR deluxe_lib_ENABLE_SANITIZER_ADDRESS
       OR deluxe_lib_ENABLE_SANITIZER_THREAD
       OR deluxe_lib_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${deluxe_lib_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${deluxe_lib_ENABLE_SANITIZER_UNDEFINED}")
    deluxe_lib_enable_hardening(deluxe_lib_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(deluxe_lib_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(deluxe_lib_warnings INTERFACE)
  add_library(deluxe_lib_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  deluxe_lib_set_project_warnings(
    deluxe_lib_warnings
    ${deluxe_lib_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  include(cmake/Linker.cmake)
  # Must configure each target with linker options, we're avoiding setting it globally for now

  if(NOT EMSCRIPTEN)
    include(cmake/Sanitizers.cmake)
    deluxe_lib_enable_sanitizers(
      deluxe_lib_options
      ${deluxe_lib_ENABLE_SANITIZER_ADDRESS}
      ${deluxe_lib_ENABLE_SANITIZER_LEAK}
      ${deluxe_lib_ENABLE_SANITIZER_UNDEFINED}
      ${deluxe_lib_ENABLE_SANITIZER_THREAD}
      ${deluxe_lib_ENABLE_SANITIZER_MEMORY})
  endif()

  set_target_properties(deluxe_lib_options PROPERTIES UNITY_BUILD ${deluxe_lib_ENABLE_UNITY_BUILD})

  if(deluxe_lib_ENABLE_PCH)
    target_precompile_headers(
      deluxe_lib_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(deluxe_lib_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    deluxe_lib_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(deluxe_lib_ENABLE_CLANG_TIDY)
    deluxe_lib_enable_clang_tidy(deluxe_lib_options ${deluxe_lib_WARNINGS_AS_ERRORS})
  endif()

  if(deluxe_lib_ENABLE_CPPCHECK)
    deluxe_lib_enable_cppcheck(${deluxe_lib_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()
  
  if(deluxe_lib_ENABLE_LIZARD)
    deluxe_lib_enable_lizard(${deluxe_lib_WARNINGS_AS_ERRORS})
  endif()
  
  if(deluxe_lib_ENABLE_BLOATY)
    deluxe_lib_enable_bloaty()
  endif()

  if(deluxe_lib_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    deluxe_lib_enable_coverage(deluxe_lib_options)
  endif()

  if(deluxe_lib_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(deluxe_lib_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(deluxe_lib_ENABLE_HARDENING AND NOT deluxe_lib_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR deluxe_lib_ENABLE_SANITIZER_UNDEFINED
       OR deluxe_lib_ENABLE_SANITIZER_ADDRESS
       OR deluxe_lib_ENABLE_SANITIZER_THREAD
       OR deluxe_lib_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    deluxe_lib_enable_hardening(deluxe_lib_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
