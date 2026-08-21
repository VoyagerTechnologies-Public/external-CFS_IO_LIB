##################################################################
#
# Unit Test build recipe
#
##################################################################

add_cfe_coverage_test(io_lib io_lib_init
  io_lib_init_tests.c
  ${CFS_IO_LIB_SOURCE_DIR}/fsw/src/io_lib_init.c
)

target_link_libraries(coverage-io_lib-io_lib_init-testrunner
  ut_psp_api_stubs
)

add_cfe_coverage_test(io_lib tm_sdlp
  tm_sdlp_tests.c
  ${CFS_IO_LIB_SOURCE_DIR}/fsw/src/services/tm_sdlp.c
  ${CFS_IO_LIB_SOURCE_DIR}/fsw/src/formats/tmtf.c
)

target_link_libraries(coverage-io_lib-tm_sdlp-testrunner
  ut_psp_api_stubs
)
