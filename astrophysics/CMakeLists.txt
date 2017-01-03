#------------------------------------------------------------------------------
# Copyright (c) 2015-, UT-Battelle, LLC
# All rights reserved.
#
# Author Contact: Jay Jay Billings, jayjaybillings <at> gmail <dot> com
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

#Set the package name
SET(PACKAGE_NAME "FireAstro")
#Set the description
SET(PACKAGE_DESCRIPTION "Fire Astrophysics Library")
#Set the library name
SET(LIBRARY_NAME "FireAstro")

#Collect all header filenames in this project 
#and glob them in HEADERS
file(GLOB HEADERS *.h)

#Grab all of the source files
file(GLOB SRC *.cpp)

#Add the source code to the library
#add_library(${LIBRARY_NAME} STATIC ${SRC})
    
# Get the test files
file(GLOB test_files tests/*Test.cpp)

# BUG Cannot use variables from another non-parent directory, 
# each new directory uses a new scope + parent's scope
# so try to find sundials even though we did it in solvers.

#Look for SUNDIALS if some hints are specified
if(SUNDIALS_ROOT OR SPACK_ROOT)
  # Find SUNDIALS
  message(STATUS "SUNDIALS support requested.")
  find_package(SUNDIALS)
  # SUNDIALS needs BLAS
  find_package(BLAS)
  # SUNDIALS needs LAPACK
  find_package(LAPACK)
  # SUNDIALS needs libm - the math library
  find_library(mathlib NAMES "m")
  set(SUNDIALS_LIBRARIES ${SUNDIALS_LIBRARIES} ${BLAS_LIBRARIES} 
      ${LAPACK_LIBRARIES} ${mathlib})

  # Add the libraries to the package library list
  set(FIRE_SOLVERS_LIBRARIES ${FIRE_SOLVERS_LIBRARIES} ${SUNDIALS_LIBRARIES}
      PARENT_SCOPE)
  # Add the include directories
  set(FIRE_SOLVERS_INCLUDE_DIRS ${FIRE_SOLVERS_INCLUDE_DIRS} 
      ${SUNDIALS_INCLUDE_DIRS} PARENT_SCOPE)
endif(SUNDIALS_ROOT OR SPACK_ROOT)

# If we don't have SUNDIALS, then we can't build the following 
# tests. 
if (NOT SUNDIALS_FOUND) 
  message (STATUS "No Sundials - Removing Astrophysics NetworkIntegration Test")
  message (STATUS "To use SUNDIALS: install blas-devel, lapack-devel, and sundials-devel, then set -DSUNDIALS_ROOT=/path/to/sundials")
  list (REMOVE_ITEM test_files "${CMAKE_CURRENT_SOURCE_DIR}/tests/NetworkIntegrationTest.cpp")
endif()

# Add the solver include libraries
set(FIRE_ASTRO_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${FIRE_SOLVERS_INCLUDE_DIRS})
# Add the tests
add_tests("${test_files}" "${FIRE_ASTRO_INCLUDE_DIRS}" "${FIRE_SOLVERS_LIBRARIES}")
  
# Copy the data file for Species parsing tests
configure_file(tests/CUDAnet_alpha.inp CUDAnet_alpha.inp COPYONLY)
# Copy the data file for Reaction parsing tests
configure_file(tests/rateLibrary_alpha.data rateLibrary_alpha.data COPYONLY)
# Copy the parameter file for the network test
configure_file(tests/alpha_gold.ini alpha_gold.ini COPYONLY)
#Install the Fire header files
install(FILES ${HEADERS} DESTINATION include)
