include(GenerateExportHeader)

add_library(deluxe_lib.Math STATIC
    "vectorization/DataVector.cpp"
)

target_include_directories(deluxe_lib.Math
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/math
)

add_library(deluxe_lib::Math ALIAS deluxe_lib.Math)

target_link_libraries(deluxe_lib.Math 
  PRIVATE 
    deluxe_lib_options 
    deluxe_lib_warnings
)

target_include_directories(deluxe_lib.Math  ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                                  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

if (deluxe_lib_BUILD_FUZZ_TESTS)
  target_link_libraries(deluxe_lib.Math PRIVATE -fsanitize=fuzzer-no-link)
  target_compile_options(deluxe_lib.Math PRIVATE -fsanitize=fuzzer-no-link)
endif()

target_compile_features(deluxe_lib.Math PUBLIC cxx_std_${CMAKE_CXX_STANDARD})

deluxe_lib_configure_linker(deluxe_lib.Math)

set_target_properties(
  deluxe_lib.Math
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(deluxe_lib.Math EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/deluxe_lib/deluxe_lib_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(deluxe_lib.Math PUBLIC deluxe_lib_STATIC_DEFINE)
endif()