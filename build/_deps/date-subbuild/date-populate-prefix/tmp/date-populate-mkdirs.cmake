# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/kirito/Documents/projects/stock_for_cpp/build/_deps/date-src")
  file(MAKE_DIRECTORY "/Users/kirito/Documents/projects/stock_for_cpp/build/_deps/date-src")
endif()
file(MAKE_DIRECTORY
  "/Users/kirito/Documents/projects/stock_for_cpp/build/_deps/date-build"
  "/Users/kirito/Documents/projects/stock_for_cpp/build/_deps/date-subbuild/date-populate-prefix"
  "/Users/kirito/Documents/projects/stock_for_cpp/build/_deps/date-subbuild/date-populate-prefix/tmp"
  "/Users/kirito/Documents/projects/stock_for_cpp/build/_deps/date-subbuild/date-populate-prefix/src/date-populate-stamp"
  "/Users/kirito/Documents/projects/stock_for_cpp/build/_deps/date-subbuild/date-populate-prefix/src"
  "/Users/kirito/Documents/projects/stock_for_cpp/build/_deps/date-subbuild/date-populate-prefix/src/date-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/kirito/Documents/projects/stock_for_cpp/build/_deps/date-subbuild/date-populate-prefix/src/date-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/kirito/Documents/projects/stock_for_cpp/build/_deps/date-subbuild/date-populate-prefix/src/date-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
