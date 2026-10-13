include(GenerateExportHeader)

add_library(deluxe_lib.Utility STATIC
    "dependencies/manifest/Manifest.cpp"
)

target_include_directories(deluxe_lib.Utility
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include/utility
)

add_library(deluxe_lib::utility ALIAS deluxe_lib.Utility)

target_link_libraries(deluxe_lib.Utility 
  PRIVATE 
    deluxe_lib_options 
    deluxe_lib_warnings
    nlohmann_json::nlohmann_json
)

target_include_directories(deluxe_lib.Utility  
  ${WARNING_GUARD} 
    PUBLIC 
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
)

if (deluxe_lib_BUILD_FUZZ_TESTS)
  target_link_libraries(deluxe_lib.Utility PRIVATE -fsanitize=fuzzer-no-link)
  target_compile_options(deluxe_lib.Utility PRIVATE -fsanitize=fuzzer-no-link)
endif()

target_compile_features(deluxe_lib.Utility PUBLIC cxx_std_${CMAKE_CXX_STANDARD})

deluxe_lib_configure_linker(deluxe_lib.Utility)

set_target_properties(
  deluxe_lib.Utility
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(deluxe_lib.Utility EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/deluxe_lib/deluxe_lib_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(deluxe_lib.Utility PUBLIC deluxe_lib_STATIC_DEFINE)
endif()