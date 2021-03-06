add_executable(run_unit EXCLUDE_FROM_ALL
  ArchiveReaderTest.cpp
  ArchiveWriterTest.cpp
  ArgParserTest.cpp
  DataRefTest.cpp
  DateTimeTest.cpp
  DeflateStreamTest.cpp
  ErrorTest.cpp
  FilesystemTest.cpp
  InflateStreamTest.cpp
  LocalPathToUTF8PathTest.cpp
  NumberTest.cpp
  StreamTest.cpp
  TestUtility.cpp
  UTF8Test.cpp
)

target_compile_options(run_unit PRIVATE ${BIGFIX_COMPILE_OPTIONS})
target_link_libraries(run_unit bigfix gtest ${BIGFIX_LINK_OPTIONS})

add_custom_target(unit
  DEPENDS run_unit

  # Reset the filesystem sandbox
  COMMAND ${CMAKE_COMMAND} -E remove_directory FilesystemTestSandbox
  COMMAND ${CMAKE_COMMAND} -E make_directory FilesystemTestSandbox

  # Run tests
  COMMAND run_unit --gtest_color=yes

  # Clean the filesystem sandbox
  COMMAND ${CMAKE_COMMAND} -E remove_directory FilesystemTestSandbox
)

add_custom_target(integration
  DEPENDS bfarchive unit
  COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/BFArchiveTest.py $<TARGET_FILE:bfarchive>
)

add_custom_target(test DEPENDS unit integration)
