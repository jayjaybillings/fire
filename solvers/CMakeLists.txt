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
SET(PACKAGE_NAME "FireSolvers")
#Set the description
SET(PACKAGE_DESCRIPTION "Fire Solver Library")
#Set the library name
SET(LIBRARY_NAME "FireSolvers")

# Log that this project will be built
MESSAGE(STATUS "----- Detected and building ${PACKAGE_NAME} -----")

# Collect all header filenames in this project 
# and glob them in HEADERS
file(GLOB HEADERS *.h)

# Grab all of the source files
file(GLOB SRC *.cpp)

# Add the library to the list of all the libraries
set(FIRE_SOLVERS_LIBRARIES ${LIBRARY_NAME})

# Add the source code to the library
add_library(${LIBRARY_NAME} STATIC ${SRC})

# Find LAPACK
find_package(LAPACK)
if(LAPACK_FOUND)
  message(STATUS "LAPACK found!")
  
  message(STATUS "${LAPACK_LINKER_FLAGS}" "${LAPACK_INCLUDE_DIR}")
  
  # Add a LAPACK example to the unit tests to make sure Fire can build against 
  # it correctly.
  message(STATUS "Adding a LAPACK example to the tests to test integration.")
  # Add the tests
  add_tests_with_flags("tests/lapack/example_DGESV_rowmajor.c" "${LAPACK_INCLUDE_DIR}" 
                       "${LAPACK_LIBRARIES}" "${LAPACK_LINKER_FLAGS}")
  # Add the libraries to the package library list
  set(FIRE_SOLVERS_LIBRARIES ${FIRE_SOLVERS_LIBRARIES} ${LAPACK_LIBRARIES})
  # Link the libraries to the Fire library
  target_link_libraries(${LIBRARY_NAME} ${LAPACK_LIBRARIES})
  
endif(LAPACK_FOUND)

# Look for MAGMA if the user set MAGMA_ROOT
if(MAGMA_ROOT)
  message(STATUS 
     "MAGMA support requested. Searching at MAGMA_ROOT=${MAGMA_ROOT}.")
  find_package(MAGMA)
  # Add a MAGMA example to the unit tests to make sure Fire can build against 
  # it correctly.
  message(STATUS "Adding a MAGMA example to the tests to test integration.")
  # Add the tests
  add_tests_with_flags("tests/magma/example_v2.c" "${MAGMA_INCLUDE_DIRS}" 
                       "${MAGMA_LIBRARIES}" "${MAGMA_CFLAGS}")
  # Add the libraries to the package library list
  set(FIRE_SOLVERS_LIBRARIES ${FIRE_SOLVERS_LIBRARIES} ${MAGMA_LIBRARIES})
  # Add the include directories
  set(FIRE_SOLVERS_INCLUDE_DIRS ${FIRE_SOLVERS_INCLUDE_DIRS} 
      ${MAGMA_INCLUDE_DIRS})
  # Link the libraries to the Fire library
  target_link_libraries(${LIBRARY_NAME} ${MAGMA_LIBRARIES})
endif(MAGMA_ROOT)

# Look for SUNDIALS if some hints are specified
if(SUNDIALS_ROOT OR SPACK_ROOT AND LAPACK_FOUND)
  # Find SUNDIALS
  message(STATUS "SUNDIALS support requested.")
  find_package(SUNDIALS)
  # SUNDIALS needs BLAS
  find_package(BLAS)
  # SUNDIALS needs libm - the math library
  find_library(mathlib NAMES "m")
  set(SUNDIALS_LIBRARIES ${SUNDIALS_LIBRARIES} ${BLAS_LIBRARIES} 
      ${LAPACK_LIBRARIES} ${mathlib})
  # Link the libraries to the Fire library
  target_link_libraries(${LIBRARY_NAME} ${SUNDIALS_LIBRARIES})
  # Add a SUNDIALS example to the unit tests to make sure Fire can build 
  # against it correctly.
  message(STATUS "Adding a SUNDIALS example to the tests to test integration.")
  add_tests("tests/sundials/cvAdvDiff_bnd.c" "${SUNDIALS_INCLUDE_DIRS}"
            "${SUNDIALS_LIBRARIES}")
  # Add the libraries to the package library list
  set(FIRE_SOLVERS_LIBRARIES ${FIRE_SOLVERS_LIBRARIES} ${SUNDIALS_LIBRARIES})
  # Add the include directories
  set(FIRE_SOLVERS_INCLUDE_DIRS ${FIRE_SOLVERS_INCLUDE_DIRS} 
      ${SUNDIALS_INCLUDE_DIRS})
  # Get the test files for sundials-specific classes
  file(GLOB test_files tests/sundials/*Test.cpp)
  # Add the tests for Fire's classes
  add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}" "${FIRE_SOLVERS_LIBRARIES}")  
endif(SUNDIALS_ROOT OR SPACK_ROOT AND LAPACK_FOUND)

# Add the variables to the global property list
set(FIRE_SOLVERS_LIBRARIES "${FIRE_SOLVERS_LIBRARIES}" CACHE INTERNAL "FIRE_SOLVERS_LIBRARIES")
set(FIRE_SOLVERS_INCLUDE_DIRS "${FIRE_SOLVERS_INCLUDE_DIRS}" CACHE INTERNAL "FIRE_SOLVERS_INCLUDE_DIRS")

# Get all remaining test files that don't require special libraries.
file(GLOB test_files tests/*Test.cpp)
# Add the tests for Fire's classes
add_tests("${test_files}" "${CMAKE_CURRENT_SOURCE_DIR}" "${FIRE_SOLVERS_LIBRARIES}")  

# Install the Fire header files
install(FILES ${HEADERS} DESTINATION include)
