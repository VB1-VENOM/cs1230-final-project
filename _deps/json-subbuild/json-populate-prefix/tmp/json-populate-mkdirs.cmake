# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/jakestifelman/Library/Mobile Documents/com~apple~CloudDocs/Brown/Fall2024/CS1230/Projects/cs1230-final-project/_deps/json-src")
  file(MAKE_DIRECTORY "/Users/jakestifelman/Library/Mobile Documents/com~apple~CloudDocs/Brown/Fall2024/CS1230/Projects/cs1230-final-project/_deps/json-src")
endif()
file(MAKE_DIRECTORY
  "/Users/jakestifelman/Library/Mobile Documents/com~apple~CloudDocs/Brown/Fall2024/CS1230/Projects/cs1230-final-project/_deps/json-build"
  "/Users/jakestifelman/Library/Mobile Documents/com~apple~CloudDocs/Brown/Fall2024/CS1230/Projects/cs1230-final-project/_deps/json-subbuild/json-populate-prefix"
  "/Users/jakestifelman/Library/Mobile Documents/com~apple~CloudDocs/Brown/Fall2024/CS1230/Projects/cs1230-final-project/_deps/json-subbuild/json-populate-prefix/tmp"
  "/Users/jakestifelman/Library/Mobile Documents/com~apple~CloudDocs/Brown/Fall2024/CS1230/Projects/cs1230-final-project/_deps/json-subbuild/json-populate-prefix/src/json-populate-stamp"
  "/Users/jakestifelman/Library/Mobile Documents/com~apple~CloudDocs/Brown/Fall2024/CS1230/Projects/cs1230-final-project/_deps/json-subbuild/json-populate-prefix/src"
  "/Users/jakestifelman/Library/Mobile Documents/com~apple~CloudDocs/Brown/Fall2024/CS1230/Projects/cs1230-final-project/_deps/json-subbuild/json-populate-prefix/src/json-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/jakestifelman/Library/Mobile Documents/com~apple~CloudDocs/Brown/Fall2024/CS1230/Projects/cs1230-final-project/_deps/json-subbuild/json-populate-prefix/src/json-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/jakestifelman/Library/Mobile Documents/com~apple~CloudDocs/Brown/Fall2024/CS1230/Projects/cs1230-final-project/_deps/json-subbuild/json-populate-prefix/src/json-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
