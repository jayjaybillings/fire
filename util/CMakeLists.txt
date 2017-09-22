#------------------------------------------------------------------------------
# Copyright (c) 2016-, UT-Battelle, LLC
# All rights reserved.
#
# Author Contact: Alex McCaskey, mccaskeyaj <at> ornl <dot> gov
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
# Author(s): Alex McCaskey
#----------------------------------------------------------------------------*/

#Set the package name
SET(PACKAGE_NAME "FireUtils")
#Set the description
SET(PACKAGE_DESCRIPTION "Fire Utility Data Structures")

find_package(Boost COMPONENTS system regex)
find_package(OpenSSL)

if (Boost_FOUND AND OPENSSL_FOUND)
   message (STATUS "Boost ASIO and OpenSSL found")
   include_directories(${Boost_INCLUDE_DIRS})
   include_directories(${CMAKE_SOURCE_DIR}/tpls/simpleweb)
   include_directories(${OPENSSL_INCLUDE_DIR})
   
   # Collect all header filenames in this project 
   # and glob them in HEADERS
   file(GLOB HEADERS *.hpp)

   # Get the test files
   file(GLOB test_files tests/*Test.cpp)

   # Add the tests
   add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}" "${Boost_LIBRARIES};pthread;${OPENSSL_LIBRARIES}")

   #Install the Fire header files
   install(FILES ${HEADERS} DESTINATION include)
endif()

if (NOT Boost_FOUND OR NOT OPENSSL_FOUND)
  message(STATUS "Warning - Boost Asio or OpenSSL not found. Skipping AsioNetworkingTool build")
endif()