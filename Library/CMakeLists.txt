list(APPEND SOURCES
  BigFix/ArchiveReader.cpp
  BigFix/ArchiveStream.cpp
  BigFix/ArchiveWriter.cpp
  BigFix/ArgParser.cpp
  BigFix/DataRef.cpp
  BigFix/DateTime.cpp
  BigFix/DeflateStream.cpp
  BigFix/Error.cpp
  BigFix/Filesystem.cpp
  BigFix/InflateStream.cpp
  BigFix/Number.cpp
  BigFix/Stream.cpp
  BigFix/UTF8.cpp
)

if(WIN32)
  list(APPEND SOURCES BigFix/WindowsFilesystem.cpp)
else()
  list(APPEND SOURCES BigFix/UnixFilesystem.cpp)
endif()

add_library(bigfix ${SOURCES})

target_compile_options(bigfix PRIVATE ${BIGFIX_COMPILE_OPTIONS})
target_include_directories(bigfix PUBLIC .)
target_link_libraries(bigfix zlibstatic)
