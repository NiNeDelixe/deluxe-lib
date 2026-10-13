include(GenerateExportHeader)

add_library(deluxe_lib.Core INTERFACE
)

target_include_directories(deluxe_lib.Core
    INTERFACE
        ${PROJECT_SOURCE_DIR}/include/
)

add_library(deluxe_lib::Core ALIAS deluxe_lib.Core)

target_link_libraries(deluxe_lib.Core
  INTERFACE 
    deluxe_lib_options 
    deluxe_lib_warnings
    nlohmann_json::nlohmann_json
)

target_include_directories(deluxe_lib.Core 
    ${WARNING_GUARD} 
        INTERFACE 
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

if (deluxe_lib_BUILD_FUZZ_TESTS)
  target_link_libraries(deluxe_lib.Core PRIVATE -fsanitize=fuzzer-no-link)
  target_compile_options(deluxe_lib.Core PRIVATE -fsanitize=fuzzer-no-link)
endif()

target_compile_features(deluxe_lib.Core INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

deluxe_lib_configure_linker(deluxe_lib.Core)

set_target_properties(
  deluxe_lib.Core
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(deluxe_lib.Core EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/deluxe_lib/deluxe_lib_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(deluxe_lib.Core INTERFACE deluxe_lib_STATIC_DEFINE)
endif()