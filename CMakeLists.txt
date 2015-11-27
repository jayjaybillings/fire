#------------------------------------------------------------------------------
# Copyright (c) 2015-, Jay Jay Billings
# All rights reserved.
#
# Author Contact: Jay Jay Billings, billingsjj@ornl.gov
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of fern nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Author(s): Jay Jay Billings
#----------------------------------------------------------------------------*/

# Set a name for this project.
PROJECT ( FIRE CXX )
# Set the minimum required version of cmake for this project.
cmake_minimum_required(VERSION 2.8)

# Fire requires C++11 to use shared pointers
list( APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} $ENV{CXX_FLAGS}")

# Setup Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

# Include directories for the main program. Note that include 
# directories must come before add subdirectory!
include_directories("${CMAKE_SOURCE_DIR}/parsers")
include_directories("${CMAKE_SOURCE_DIR}/tpls/simpleini-4.17")

# Add the source directory
add_subdirectory(parsers)
# Add the third party libraries
ADD_SUBDIRECTORY(tpls)

# Setup the library list
set(FIRE_LIBS fireParsers)

# ---- Tests! ----- #

# Find Boost for unit tests
FIND_PACKAGE(Boost)
# Enable testing if boost is found.
if(Boost_FOUND)
    # Turn on the testing harness
	enable_testing()
    # Let the user know that tests will be built
    message(STATUS "Boost version ${Boost_VERSION} found.")
    message(STATUS "Tests will be built since Boost was found.")
    message(STATUS "Tests can be run with the 'make test' command.")
    # Include the header directory since BOOST tests
    INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIR}")
    #Get the test files
    file(GLOB tests parsers/tests/*Test.cpp)
    #Make executables and link libraries for testers
    foreach(test ${tests})
        message(STATUS "Making test ${test}")
        get_filename_component(testName ${test} NAME_WE)
        add_executable(${testName} ${test})
        add_test(${testName} ${testName})
        target_link_libraries(${testName} ${FIRE_LIBS}) 
        #add a label so the tests can be run separately
        set_property(TEST ${testName} PROPERTY LABELS ${PACKAGE_NAME})
    endforeach(test ${tests})
endif(Boost_FOUND)

# Add the tests is Boost is available

IF (Boost_FOUND)

ENDIF()
